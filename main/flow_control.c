/**
 * @file flow_control.c
 * @author Iwan Ćulumović
 * @brief Flow control module.
 * 
 * @copyright Copyright (c) 2026
 * 
 */

/* ============================== INCLUDES */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "flow_control.h"
#include "sha256_calculator.h"
#include "gpio_manager.h"
#include "i2c_manager.h"

/* ============================== MACRO DEFINITIONS */

/** @brief Log tag. */
#define LOG_TAG                             ("FLOW_CONTROL")

/** @brief Flow control task stack depth. */
#define TASK_FLOW_CONTROL_STACK_DEPTH       (2048)

/** @brief Flow control task priority. */
#define TASK_FLOW_CONTROL_PRIORITY          (0)

/* ============================== TYPE DEFINITIONS */

/* ============================== PRIVATE FUNCTION DECLARATIONS */

/**
 * @brief Task that controls data flow.
 * 
 * @param p_task_params Task parameters (not used).
 */
static void _flow_control_task(void *p_task_params);

/* ============================== PRIVATE VARIABLES */

/** @brief Data for I2C master when it writes. */
static sha256_input_variables_t _g_sha256_input_variables = {0};

/** @brief Data for I2C master when it reads. */
static sha256_offset_solution_t _g_sha256_offset_solution = {0};

/** @brief Flow control task handle. */
static TaskHandle_t _g_task_handle_flow_control = NULL;

/* ============================== PUBLIC VARIABLES */

/* ============================== PUBLIC FUNCTION DEFINITIONS */

void flow_control_init(void)
{
    BaseType_t result = pdPASS;

    result = xTaskCreate(_flow_control_task, "MAIN_CTRL", TASK_FLOW_CONTROL_STACK_DEPTH, NULL, TASK_FLOW_CONTROL_PRIORITY, &_g_task_handle_flow_control);
    if (pdPASS != result)
    {
        ESP_LOGE(LOG_TAG, "Failed to create task for main control. Aborting!");
        abort();
    }

    ESP_LOGI(LOG_TAG, "Initialized flow control.");
}

/* ============================== PRIVATE FUNCTION DEFINITIONS */

static void _flow_control_task(void *p_task_params)
{
    while (1)
    {
        /* Receive data and reset flag */
        i2c_manager_slave_get_written_data((uint8_t *)&_g_sha256_input_variables, sizeof(sha256_input_variables_t));
        gpio_reset_interrupt_out();

        /* Send data for calculation */
        sha256_calculator_queue_input_put(&_g_sha256_input_variables);
        sha256_calculator_queue_solution_get(&_g_sha256_offset_solution);

        /* Set data to be read and set flag */
        i2c_manager_slave_set_data_to_be_read((uint8_t *)&_g_sha256_offset_solution, sizeof(sha256_offset_solution_t));
        gpio_set_interrupt_out();
        ESP_LOGI(LOG_TAG, "Offset solution %d", _g_sha256_offset_solution.offset_solution);

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

/* ============================== INTERRUPT FUNCTION DEFINITIONS */
