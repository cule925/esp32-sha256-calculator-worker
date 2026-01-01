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

/* ============================== MACRO DEFINITIONS */

/** @brief Log tag. */
#define LOG_TAG                 ("I2C_MANAGER")

/** @brief Send buffer depth. */
#define SEND_BUF_DEPTH          (256)

/* ============================== PRIVATE FUNCTION DECLARATIONS */

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

/* ============================== PUBLIC VARIABLES */

/* ============================== PUBLIC FUNCTION DEFINITIONS */

void i2c_manager_slave_init(void)
{
    ESP_ERROR_CHECK(i2c_new_slave_device(&_g_i2c_slave_config, &_g_i2c_slave_handle));

    ESP_LOGI(LOG_TAG, "Initialized slave with address %x, on GPIO SDA %d and SCL %d.",
        CONFIG_I2C_SLAVE_ADDRESS,
        CONFIG_I2C_SDA_GPIO,
        CONFIG_I2C_SCL_GPIO);
}

/* ============================== PRIVATE FUNCTION DEFINITIONS */

/* ============================== INTERRUPT FUNCTION DEFINITIONS */
