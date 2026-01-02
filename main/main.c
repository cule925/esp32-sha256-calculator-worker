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
#include "sha256_calculator.h"

/* ============================== MACRO DEFINITIONS */

/** @brief Log tag. */
#define LOG_TAG                 ("MAIN")

/** @brief Size of data I2C master writes. */
#define I2C_MASTER_W_DATA_SIZE      (37)

/** @brief Size of data I2C master reads. */
#define I2C_MASTER_R_DATA_SIZE      (4)

/* ============================== TYPE DEFINITIONS */

/* ============================== PRIVATE FUNCTION DECLARATIONS */

/* ============================== PRIVATE VARIABLES */

/** @brief Data I2C master writes. */
static uint8_t _g_i2c_master_w_data[I2C_MASTER_W_DATA_SIZE] = {0};

/** @brief Data I2C master reads. */
static uint8_t _g_i2c_master_r_data[I2C_MASTER_R_DATA_SIZE] = {0};

/* ============================== PUBLIC VARIABLES */

/* ============================== PUBLIC FUNCTION DEFINITIONS */

void app_main(void)
{
    ESP_LOGI(LOG_TAG, "Hello from main.");
    i2c_manager_slave_init();
    sha256_calculator_init();
    while (1) {
        ESP_LOGI(LOG_TAG, "Reading data that I2C master sent me.");
        i2c_manager_slave_get_written_data(_g_i2c_master_w_data, sizeof(_g_i2c_master_w_data));
        _g_i2c_master_w_data[I2C_MASTER_W_DATA_SIZE - 1] = '\0';
        ESP_LOGI(LOG_TAG, "Data: %s", _g_i2c_master_w_data);
        vTaskDelay(10000 / portTICK_PERIOD_MS);
        ESP_LOGI(LOG_TAG, "Writing data that I2C master will read.");
        i2c_manager_slave_set_data_to_be_read(_g_i2c_master_r_data, sizeof(_g_i2c_master_r_data));
        _g_i2c_master_r_data[I2C_MASTER_R_DATA_SIZE - 1] = '\0';
        ESP_LOGI(LOG_TAG, "Data: %s", _g_i2c_master_r_data);
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}

/* ============================== PRIVATE FUNCTION DEFINITIONS */

/* ============================== INTERRUPT FUNCTION DEFINITIONS */
