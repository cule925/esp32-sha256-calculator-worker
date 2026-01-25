/**
 * @file spi_manager.c
 * @author Iwan Ćulumović
 * @brief SPI manager module.
 * 
 * @copyright Copyright (c) 2026
 * 
 */

/* ============================== INCLUDES */

#include "esp_log.h"
#include "sdkconfig.h"
#include "comm/driver/spi_manager.h"
#include "driver/spi_slave.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "gpio/gpio_manager.h"

/* ============================== MACRO DEFINITIONS */

/** @brief Log tag. */
#define LOG_TAG                                         ("SPI_MANAGER")

/** @brief SPI transaction queue size. */
#define TRANSACTION_QUEUE_SIZE                          (32)

/** @brief SPI transaction size. */
#define TRANSACTION_SIZE                                (40)

/** @brief SPI receive buffer size. */
#define RX_BUF_SIZE                                     (39)

/** @brief SPI transmit buffer size. */
#define TX_BUF_SIZE                                     (5)

/** @brief SPI master command request data write. */
#define SPI_MASTER_CMD_REQUEST_DATA_WRITE               (0x11)

/** @brief SPI master command write data. */
#define SPI_MASTER_CMD_DATA_WRITE                       (0x22)

/** @brief SPI master command request data read. */
#define SPI_MASTER_CMD_REQUEST_DATA_READ                (0x33)

/** @brief SPI master command read data. */
#define SPI_MASTER_CMD_DATA_READ                        (0x44)

/** @brief SPI transaction enqueue task stack depth. */
#define TRANSACTION_ENQUEUE_CONTROL_STACK_DEPTH         (2048)

/** @brief SPI transaction enqueue task priority. Must be higher than other tasks. */
#define TRANSACTION_ENQUEUE_CONTROL_PRIORITY            (1)

/* ============================== TYPE DEFINITIONS */

/* ============================== PRIVATE FUNCTION DECLARATIONS */

/**
 * @brief Task that controls SPI transaction enqueueing.
 * 
 * @param p_task_params Task parameters (not used).
 */
static void _spi_transaction_enqueue_task(void *p_task_params);

/* ============================== PRIVATE VARIABLES */

/** @brief SPI host device. */
static spi_host_device_t _g_spi_host_device = SPI2_HOST;

/** @brief SPI bus configuration. */
static spi_bus_config_t _g_spi_bus_config =
{
    .mosi_io_num = CONFIG_SPI_MOSI_GPIO,            //! MOSI GPIO
    .miso_io_num = CONFIG_SPI_MISO_GPIO,            //! MISO GPIO
    .sclk_io_num = CONFIG_SPI_SCLK_GPIO,            //! SCLK GPIO
    .quadwp_io_num = -1,                            //! Write protect GPIO (not used)
    .quadhd_io_num = -1,                            //! Hold signal GPIO (not used)
    .data_io_default_level = false,                 //! Drives MISO low when not actively transmitting
    .max_transfer_sz = 0,                           //! Maximum transfer size will be defaulted to 4096 if DMA is enabled
    .flags = 0,                                     //! No specific SPI bus flags
    .isr_cpu_id = ESP_INTR_CPU_AFFINITY_AUTO,       //! Installs the SPI interrupt to any CPU core
    .intr_flags = ESP_INTR_FLAG_LEVEL3,             //! Interrupt priority
};

/** @brief SPI slave interface configuration. */
static spi_slave_interface_config_t _g_spi_slave_interface_config =
{
    .spics_io_num = CONFIG_SPI_CS_GPIO,                                     //! CS GPIO
    .flags = 0,                                                             //! No specific SPI slave interface flags
    .queue_size = TRANSACTION_QUEUE_SIZE,                                   //! Transaction queue size
    .mode = 0,                                                              //! SPI mode (CPOL = 0, CPHA = 0)
    .post_setup_cb = NULL,                                                  //! No post setup callback
    .post_trans_cb = NULL,                                                  //! No post transaction callback
};

/** @brief DMA channel for SPI. */
static spi_dma_chan_t _g_spi_dma_chan = SPI_DMA_CH1;

/** @brief SPI data receive buffer for transaction. */
static volatile uint8_t *_gp_spi_rx_buf = NULL;

/** @brief SPI data transmit buffer for transaction. */
static volatile uint8_t *_gp_spi_tx_buf = NULL;

/** @brief SPI data receive copy buffer. */
static uint8_t _g_spi_rx_buf_copy[RX_BUF_SIZE] = {0};

/** @brief SPI data transmit copy buffer. */
static uint8_t _g_spi_tx_buf_copy[TX_BUF_SIZE] = {0};

/** @brief SPI transaction. */
static spi_slave_transaction_t _g_spi_slave_transaction =
{
    .flags = 0,                                     //! No specific flags
    .length = 0,                                    //! Total data length in bits
    .trans_len = 0,                                 //! Transaction data length in bits
    .tx_buffer = NULL,                              //! Pointer to transmit buffer
    .rx_buffer = NULL,                              //! Pointer to receive buffer
    .user = NULL,                                   //! Pointer to user data
};

/** @brief SPI enqueue transaction task handle. */
static TaskHandle_t _g_task_handle_spi_transaction_enqueue = NULL;

/** @brief SPI data read semaphore handle. */
static SemaphoreHandle_t _g_sem_spi_data_read = NULL;

/** @brief SPI data written semaphore handle. */
static SemaphoreHandle_t _g_sem_spi_data_written = NULL;

/* ============================== PUBLIC VARIABLES */

/* ============================== PUBLIC FUNCTION DEFINITIONS */

void spi_manager_slave_init(void)
{
    BaseType_t result = pdPASS;

    /* Allocate DMA capable transmit and receive buffers for SPI transactions */
    _gp_spi_rx_buf = heap_caps_malloc(RX_BUF_SIZE, MALLOC_CAP_DMA);
    if (NULL == _gp_spi_rx_buf)
    {
        ESP_LOGE(LOG_TAG, "Failed to allocate RX buffer for SPI transaction. Aborting!");
        abort();
    }

    _gp_spi_tx_buf = heap_caps_malloc(TX_BUF_SIZE, MALLOC_CAP_DMA);
    if (NULL == _gp_spi_tx_buf)
    {
        ESP_LOGE(LOG_TAG, "Failed to allocate TX buffer for SPI transaction. Aborting!");
        abort();
    }

    /* Fill transaction information */
    _g_spi_slave_transaction.length = TRANSACTION_SIZE * 8;             //! Total transaction length in bits
    _g_spi_slave_transaction.tx_buffer = (void *)_gp_spi_tx_buf;        //! Pointer to transmit buffer
    _g_spi_slave_transaction.rx_buffer = (void *)_gp_spi_rx_buf;        //! Pointer to receive buffer

    /* Initialize semaphores */
    _g_sem_spi_data_read = xSemaphoreCreateBinary();
    _g_sem_spi_data_written = xSemaphoreCreateBinary();
    if ((NULL == _g_sem_spi_data_read) || (NULL == _g_sem_spi_data_written))
    {
        ESP_LOGE(LOG_TAG, "Failed to create binary semaphores for SPI. Aborting!");
        abort();
    }

    ESP_ERROR_CHECK(spi_slave_initialize(_g_spi_host_device, &_g_spi_bus_config, &_g_spi_slave_interface_config, _g_spi_dma_chan));

    /* Create transaction enqueueing task */
    result = xTaskCreate(_spi_transaction_enqueue_task, "TRANS_QUEUE", TRANSACTION_ENQUEUE_CONTROL_STACK_DEPTH, NULL, TRANSACTION_ENQUEUE_CONTROL_PRIORITY, &_g_task_handle_spi_transaction_enqueue);
    if (pdPASS != result)
    {
        ESP_LOGE(LOG_TAG, "Failed to create task for enqueueing SPI transactions. Aborting!");
        abort();
    }

    ESP_LOGI(LOG_TAG, "Initialized slave.");
}

void spi_manager_slave_set_data_to_be_read(uint8_t *p_buf, size_t buf_size)
{
    if (buf_size > TX_BUF_SIZE)
    {
        ESP_LOGE(LOG_TAG, "Buffer size to be written into is too small. Aborting!");
        abort();
    }

    /* Prepare data to be read */
    memcpy(_g_spi_tx_buf_copy, p_buf, buf_size);

    /* Signalize data ready to master */
    gpio_set_interrupt_out();
    vTaskDelay(10 / portTICK_PERIOD_MS);
    gpio_reset_interrupt_out();

    xSemaphoreTake(_g_sem_spi_data_read, portMAX_DELAY);
}

bool spi_manager_slave_receive_data(uint8_t *p_buf, size_t buf_size)
{
    bool b_received_data = false;
    BaseType_t ret = pdFALSE;

    if (buf_size > (RX_BUF_SIZE - 1))
    {
        ESP_LOGE(LOG_TAG, "Buffer size to be read from is too small. Aborting!");
        abort();
    }

    ret = xSemaphoreTake(_g_sem_spi_data_written, 0);
    if (pdTRUE == ret)
    {
        b_received_data = true;

        /* Receive written data */
        memcpy(p_buf, &_g_spi_rx_buf_copy[1], buf_size);
    }

    return b_received_data;
}

/* ============================== PRIVATE FUNCTION DEFINITIONS */

static void _spi_transaction_enqueue_task(void *p_task_params)
{
    while (1)
    {
        /* Queue an SPI transaction */
        ESP_ERROR_CHECK(spi_slave_transmit(_g_spi_host_device, &_g_spi_slave_transaction, portMAX_DELAY));

        /* If data needs to be written */
        if (SPI_MASTER_CMD_REQUEST_DATA_WRITE == _gp_spi_rx_buf[0])
        {
            /* Do nothing */
        }

        /* If data was written */
        if (SPI_MASTER_CMD_DATA_WRITE == _gp_spi_rx_buf[0])
        {
            memcpy(_g_spi_rx_buf_copy, (void *)_gp_spi_rx_buf, RX_BUF_SIZE);
            xSemaphoreGive(_g_sem_spi_data_written);
        }

        /* If data needs to be read */
        if (SPI_MASTER_CMD_REQUEST_DATA_READ == _gp_spi_rx_buf[0])
        {
            memcpy((void *)_gp_spi_tx_buf, _g_spi_tx_buf_copy, TX_BUF_SIZE);
        }

        /* If data was read */
        if (SPI_MASTER_CMD_DATA_READ == _gp_spi_rx_buf[0])
        {
            xSemaphoreGive(_g_sem_spi_data_read);
        }
    }
}

/* ============================== INTERRUPT FUNCTION DEFINITIONS */
