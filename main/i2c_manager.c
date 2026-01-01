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

/* ============================== MACRO DEFINITIONS */

/** @brief Log tag. */
#define LOG_TAG                 ("I2C_MANAGER")

/* ============================== PRIVATE FUNCTION DECLARATIONS */

/* ============================== PRIVATE VARIABLES */

/* ============================== PUBLIC VARIABLES */

/* ============================== PUBLIC FUNCTION DEFINITIONS */

int i2c_manager_slave_init(void)
{
    int ret = 0;

    ESP_LOGI(LOG_TAG, "Initializing slave with address %x, on GPIO SDA %d and SCL %d.",
        CONFIG_I2C_SLAVE_ADDRESS,
        CONFIG_I2C_SDA_GPIO,
        CONFIG_I2C_SCL_GPIO);

    return ret;
}

/* ============================== PRIVATE FUNCTION DEFINITIONS */

/* ============================== INTERRUPT FUNCTION DEFINITIONS */
