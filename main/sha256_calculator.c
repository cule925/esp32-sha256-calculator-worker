/**
 * @file sha256_calculator.c
 * @author Iwan Ćulumović
 * @brief SHA256 calculator module.
 * 
 * @copyright Copyright (c) 2026
 * 
 */

/* ============================== INCLUDES */

#include "esp_log.h"
#include "sdkconfig.h"
#include "sha256_calculator.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

/* ============================== MACRO DEFINITIONS */

/** @brief Log tag. */
#define LOG_TAG                                 ("SHA256_CALC")

/* ============================== TYPE DEFINITIONS */

/* ============================== PRIVATE FUNCTION DECLARATIONS */

/* ============================== PRIVATE VARIABLES */

/* ============================== PUBLIC VARIABLES */

/* ============================== PUBLIC FUNCTION DEFINITIONS */

void sha256_calculator_init(void)
{
    ESP_LOGI(LOG_TAG, "Initialized calculator.");
}

/* ============================== PRIVATE FUNCTION DEFINITIONS */

/* ============================== INTERRUPT FUNCTION DEFINITIONS */
