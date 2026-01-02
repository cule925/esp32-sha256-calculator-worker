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
#include "i2c_manager.h"
#include "driver/i2c_slave.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

/* ============================== MACRO DEFINITIONS */

/** @brief Log tag. */
#define LOG_TAG                                 ("I2C_MANAGER")

/** @brief Send buffer depth. */
#define SEND_BUF_DEPTH                          (256)

/** @brief Send buffer transmit timeout. */
#define SEND_BUF_TRANSMIT_TIMEOUT_MS            (10000)

/* ============================== PRIVATE FUNCTION DECLARATIONS */

/**
 * @brief I2C data receive done callback. Runs in instruction RAM for extra speed.
 * 
 * @param i2c_slave_handle I2C slave handle of the I2C controller that caused the interrupt.
 * @param p_event_data I2C slave capture event data.
 * @param p_user_data User passed data on registration.
 * @return bool Returns true if a context switch is needed.
 */
static IRAM_ATTR bool _i2c_slave_receive_done_callback(i2c_slave_dev_handle_t i2c_slave_handle, const i2c_slave_rx_done_event_data_t *p_event_data, void *p_user_data);

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
    .intr_priority = 3,                             //! Interrupt priority (highest)
    .i2c_port = I2C_NUM_0,                          //! I2C port 0
};

/** @brief I2C slave handle. */
static i2c_slave_dev_handle_t _g_i2c_slave_handle = NULL;

/** @brief I2C read done semaphore handle */
static SemaphoreHandle_t _g_sem_i2c_read_done = NULL;

/** @brief I2C slave event callbacks. */
static i2c_slave_event_callbacks_t _g_i2c_slave_event_callbacks =
{
    .on_recv_done = _i2c_slave_receive_done_callback,
};

/* ============================== PUBLIC VARIABLES */

/* ============================== PUBLIC FUNCTION DEFINITIONS */

void i2c_manager_slave_init(void)
{
    _g_sem_i2c_read_done = xSemaphoreCreateBinary();
    if (NULL == _g_sem_i2c_read_done)
    {
        ESP_LOGE(LOG_TAG, "Failed to create binary semaphore for I2C read done. Aborting!");
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
    /* Send the data to the transmit ring buffer */
    ESP_ERROR_CHECK(i2c_slave_transmit(_g_i2c_slave_handle, p_buf, buf_size, SEND_BUF_TRANSMIT_TIMEOUT_MS));
}

void i2c_manager_slave_get_written_data(uint8_t *p_buf, size_t buf_size)
{
    /* Read the data */
    ESP_ERROR_CHECK(i2c_slave_receive(_g_i2c_slave_handle, p_buf, buf_size));

    /* Wait for ISR to signalize data arrival */
    xSemaphoreTake(_g_sem_i2c_read_done, portMAX_DELAY);
}

/* ============================== PRIVATE FUNCTION DEFINITIONS */

/* ============================== INTERRUPT FUNCTION DEFINITIONS */

static bool _i2c_slave_receive_done_callback(i2c_slave_dev_handle_t i2c_slave_handle, const i2c_slave_rx_done_event_data_t *p_event_data, void *p_user_data)
{
    BaseType_t higher_priority_task_woken = pdFALSE;
    bool b_require_context_switch = false;
    xSemaphoreGiveFromISR(_g_sem_i2c_read_done, &higher_priority_task_woken);

    if (higher_priority_task_woken == pdTRUE)
    {
        b_require_context_switch = true;
    }

    return b_require_context_switch;
}
