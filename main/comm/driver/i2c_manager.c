/**
 * @file i2c_manager.c
 * @author Iwan Ćulumović
 * @brief I2C manager module.
 * 
 * @copyright Copyright (c) 2026
 * 
 */

/* ============================== INCLUDES */

#include "esp_log.h"
#include "sdkconfig.h"
#include "comm/driver/i2c_manager.h"
#include "driver/i2c_slave.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "gpio/gpio_manager.h"

/* ============================== MACRO DEFINITIONS */

/** @brief Log tag. */
#define LOG_TAG                                 ("I2C_MANAGER")

/** @brief Send buffer depth. */
#define SEND_BUF_DEPTH                          (256)

/** @brief Receive buffer depth. */
#define RECEIVE_BUF_DEPTH                       (256)

/** @brief Send buffer transmit timeout. */
#define SEND_BUF_TRANSMIT_TIMEOUT_MS            (10)

/* ============================== TYPE DEFINITIONS */

/* ============================== PRIVATE FUNCTION DECLARATIONS */

/**
 * @brief I2C on request callback.
 * 
 * @param i2c_slave_handle I2C slave handle of the I2C controller that caused the interrupt.
 * @param p_event_data I2C slave capture event data.
 * @param p_user_data User passed data on registration.
 * @return bool Returns true if a context switch is needed.
 */
static bool _i2c_slave_on_request_callback(i2c_slave_dev_handle_t i2c_slave_handle, const i2c_slave_request_event_data_t *p_event_data, void *p_user_data);

/**
 * @brief I2C on receive callback.
 * 
 * @param i2c_slave_handle I2C slave handle of the I2C controller that caused the interrupt.
 * @param p_event_data I2C slave capture event data.
 * @param p_user_data User passed data on registration.
 * @return bool Returns true if a context switch is needed.
 */
static bool _i2c_slave_on_receive_callback(i2c_slave_dev_handle_t i2c_slave_handle, const i2c_slave_rx_done_event_data_t *p_event_data, void *p_user_data);

/* ============================== PRIVATE VARIABLES */

/** @brief I2C slave configuration. */
static i2c_slave_config_t _g_i2c_slave_config =
{
    .addr_bit_len = I2C_ADDR_BIT_LEN_7,             //! 7 bit address length
    .slave_addr = CONFIG_I2C_SLAVE_ADDRESS,         //! Slave address
    .sda_io_num = CONFIG_I2C_SDA_GPIO,              //! SDA GPIO
    .scl_io_num = CONFIG_I2C_SCL_GPIO,              //! SCL GPIO
    .clk_source = I2C_CLK_SRC_APB,                  //! APB clock source
    .send_buf_depth = SEND_BUF_DEPTH,               //! Transmit buffer ring buffer depth
    .receive_buf_depth = RECEIVE_BUF_DEPTH,         //! Receive buffer depth
    .intr_priority = 3,                             //! Interrupt priority (highest)
    .i2c_port = I2C_NUM_0,                          //! I2C port 0
    .flags.enable_internal_pullup = 0,              //! Disable internal pullups
};

/** @brief I2C slave handle. */
static i2c_slave_dev_handle_t _g_i2c_slave_handle = NULL;

/** @brief I2C read done semaphore handle */
static SemaphoreHandle_t _g_sem_i2c_on_request_done = NULL;

/** @brief I2C on receive queue handle */
static QueueHandle_t _g_queue_i2c_on_receive = NULL;

/** @brief I2C on receive queue length */
static int _g_queue_i2c_on_receive_length = 0;

/** @brief I2C on receive queue item size */
static int _g_queue_i2c_on_receive_item_size = 0;

/** @brief I2C slave event callbacks. */
static i2c_slave_event_callbacks_t _g_i2c_slave_event_callbacks =
{
    .on_request = _i2c_slave_on_request_callback,
    .on_receive = _i2c_slave_on_receive_callback,
};

/* ============================== PUBLIC VARIABLES */

/* ============================== PUBLIC FUNCTION DEFINITIONS */

void i2c_manager_slave_init(int on_receive_queue_length, int on_receive_queue_item_size)
{
    _g_queue_i2c_on_receive_length = on_receive_queue_length;
    _g_queue_i2c_on_receive_item_size = on_receive_queue_item_size;

    _g_sem_i2c_on_request_done = xSemaphoreCreateBinary();
    if (NULL == _g_sem_i2c_on_request_done)
    {
        ESP_LOGE(LOG_TAG, "Failed to create binary semaphore for I2C on request done. Aborting!");
        abort();
    }

    _g_queue_i2c_on_receive = xQueueCreate(_g_queue_i2c_on_receive_length, _g_queue_i2c_on_receive_item_size);
    if (NULL == _g_queue_i2c_on_receive)
    {
        ESP_LOGE(LOG_TAG, "Failed to create queue for I2C on receive. Aborting!");
        abort();
    }

    ESP_ERROR_CHECK(i2c_new_slave_device(&_g_i2c_slave_config, &_g_i2c_slave_handle));
    ESP_ERROR_CHECK(i2c_slave_register_event_callbacks(_g_i2c_slave_handle, &_g_i2c_slave_event_callbacks, NULL));

    ESP_LOGI(LOG_TAG, "Initialized slave with address %x, on GPIO SDA %d and SCL %d.",
        CONFIG_I2C_SLAVE_ADDRESS,
        CONFIG_I2C_SDA_GPIO,
        CONFIG_I2C_SCL_GPIO);
}

void i2c_manager_slave_set_data_to_be_read(uint8_t *p_buf, size_t buf_size)
{
    uint32_t write_len = 0;

    /* Send the data to the FIFO transmit buffer */
    ESP_ERROR_CHECK(i2c_slave_write(_g_i2c_slave_handle, p_buf, buf_size, &write_len, SEND_BUF_TRANSMIT_TIMEOUT_MS));

    /* Signalize data ready to master */
    gpio_set_interrupt_out();
    vTaskDelay(10 / portTICK_PERIOD_MS);
    gpio_reset_interrupt_out();

    /* Wait for ISR to signalize a master request */
    xSemaphoreTake(_g_sem_i2c_on_request_done, portMAX_DELAY);
}

bool i2c_manager_slave_receive_data(uint8_t *p_buf, size_t buf_size)
{
    bool b_received_data = false;
    BaseType_t ret = errQUEUE_EMPTY;

    if (buf_size != _g_queue_i2c_on_receive_item_size)
    {
        ESP_LOGE(LOG_TAG, "Buffer size to be read doesn't match. Aborting!");
        abort();
    }

    /* Check if ISR put data */
    ret = xQueueReceive(_g_queue_i2c_on_receive, p_buf, 0);
    if (pdPASS == ret) b_received_data = true;

    return b_received_data;
}

/* ============================== PRIVATE FUNCTION DEFINITIONS */

/* ============================== INTERRUPT FUNCTION DEFINITIONS */

static bool _i2c_slave_on_request_callback(i2c_slave_dev_handle_t i2c_slave_handle, const i2c_slave_request_event_data_t *p_event_data, void *p_user_data)
{
    BaseType_t higher_priority_task_woken = pdFALSE;
    bool b_require_context_switch = false;

    xSemaphoreGiveFromISR(_g_sem_i2c_on_request_done, &higher_priority_task_woken);

    if (higher_priority_task_woken == pdTRUE)
    {
        b_require_context_switch = true;
    }

    return b_require_context_switch;
}

static bool _i2c_slave_on_receive_callback(i2c_slave_dev_handle_t i2c_slave_handle, const i2c_slave_rx_done_event_data_t *p_event_data, void *p_user_data)
{
    BaseType_t higher_priority_task_woken = pdFALSE;
    bool b_require_context_switch = false;

    xQueueSendToBackFromISR(_g_queue_i2c_on_receive, p_event_data->buffer, &higher_priority_task_woken);

    if (higher_priority_task_woken == pdTRUE)
    {
        b_require_context_switch = true;
    }

    return b_require_context_switch;
}
