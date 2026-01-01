/**
 * @file main.c
 * @author Iwan Ćulumović
 * @brief Main entrypoint.
 * 
 * @copyright Copyright (c) 2026
 * 
 */

/* ============================== INCLUDES */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "i2c_manager.h"

/* ============================== MACRO DEFINITIONS */

/** @brief Log tag. */
#define LOG_TAG                 ("MAIN")

/* ============================== PRIVATE FUNCTION DECLARATIONS */

/* ============================== PRIVATE VARIABLES */

/* ============================== PUBLIC VARIABLES */

/* ============================== PUBLIC FUNCTION DEFINITIONS */

void app_main(void)
{
    int ret = 0;

    ret = i2c_manager_slave_init();

    if (ret == 0) ESP_LOGI(LOG_TAG, "I2C slave initialized.");
    else ESP_LOGE(LOG_TAG, "I2C slave initialization error.");

    while (1) {
        ESP_LOGI(LOG_TAG, "Hello from main.");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

/* ============================== PRIVATE FUNCTION DEFINITIONS */

/* ============================== INTERRUPT FUNCTION DEFINITIONS */
