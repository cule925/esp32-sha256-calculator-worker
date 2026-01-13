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
#include "gpio_manager.h"
#include "flow_control.h"

/* ============================== MACRO DEFINITIONS */

/** @brief Log tag. */
#define LOG_TAG                 ("MAIN")

/* ============================== TYPE DEFINITIONS */

/* ============================== PRIVATE FUNCTION DECLARATIONS */

/* ============================== PRIVATE VARIABLES */

/* ============================== PUBLIC VARIABLES */

/* ============================== PUBLIC FUNCTION DEFINITIONS */

void app_main(void)
{
    ESP_LOGI(LOG_TAG, "Initializing.");
    gpio_manager_init();
    i2c_manager_slave_init(1, sizeof(sha256_input_variables_t));
    sha256_calculator_init();
    flow_control_init();
}

/* ============================== PRIVATE FUNCTION DEFINITIONS */

/* ============================== INTERRUPT FUNCTION DEFINITIONS */
