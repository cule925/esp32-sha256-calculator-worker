/**
 * @file gpio_manager.c
 * @author Iwan Ćulumović
 * @brief GPIO manager module.
 * 
 * @copyright Copyright (c) 2026
 * 
 */

/* ============================== INCLUDES */

#include "esp_log.h"
#include "sdkconfig.h"
#include "gpio_manager.h"
#include "driver/gpio.h"

/* ============================== MACRO DEFINITIONS */

/** @brief Log tag. */
#define LOG_TAG                                 ("GPIO_MANAGER")

/** @brief GPIO interrupt out pin mask. */
#define GPIO_INTERRUPT_OUT_MASK                 (1ULL << CONFIG_GPIO_INTERRUPT_OUT)

/* ============================== TYPE DEFINITIONS */

/* ============================== PRIVATE FUNCTION DECLARATIONS */

/* ============================== PRIVATE VARIABLES */

/** @brief GPIO configuration. */
static gpio_config_t _g_gpio_interrupt_out_config =
{
    .pin_bit_mask = (GPIO_INTERRUPT_OUT_MASK),
    .mode = GPIO_MODE_OUTPUT,
    .pull_up_en = GPIO_PULLUP_DISABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type = GPIO_INTR_DISABLE,
};

/* ============================== PUBLIC VARIABLES */

/* ============================== PUBLIC FUNCTION DEFINITIONS */

void gpio_manager_init(void)
{
    ESP_ERROR_CHECK(gpio_config(&_g_gpio_interrupt_out_config));

    ESP_LOGI(LOG_TAG, "Initialized GPIOs.");
}

void gpio_set_interrupt_out(void)
{
    gpio_set_level(CONFIG_GPIO_INTERRUPT_OUT, 1);
}

void gpio_reset_interrupt_out(void)
{
    gpio_set_level(CONFIG_GPIO_INTERRUPT_OUT, 0);
}

/* ============================== PRIVATE FUNCTION DEFINITIONS */

/* ============================== INTERRUPT FUNCTION DEFINITIONS */
