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

/** @brief Calculate main controller task stack depth. */
#define TASK_MAIN_CONTROL_STACK_DEPTH       (1024)

/** @brief Calculate SHA256 task priority. */
#define TASK_MAIN_CONTROL_PRIORITY          (0)

/* ============================== TYPE DEFINITIONS */

/* ============================== PRIVATE FUNCTION DECLARATIONS */

/**
 * @brief Task that controls data flow.
 * 
 * @param p_task_params Task parameters (not used).
 */
static void _main_control_task(void *p_task_params);

/* ============================== PRIVATE VARIABLES */

/** @brief Data I2C master writes. */
static sha256_input_variables_t _g_sha256_input_variables = {0};

/** @brief Data I2C master reads. */
static sha256_offset_solution_t _g_sha256_offset_solution = {0};

/** @brief Main control task handle. */
static TaskHandle_t _g_task_handle_main_control = NULL;

/* ============================== PUBLIC VARIABLES */

/* ============================== PUBLIC FUNCTION DEFINITIONS */

void app_main(void)
{
    BaseType_t result = pdPASS;

    ESP_LOGI(LOG_TAG, "Hello from main.");
    i2c_manager_slave_init();
    sha256_calculator_init();

    result = xTaskCreate(_main_control_task, "MAIN_CTRL", TASK_MAIN_CONTROL_STACK_DEPTH, NULL, TASK_MAIN_CONTROL_PRIORITY, &_g_task_handle_main_control);
    if (pdPASS != result)
    {
        ESP_LOGE(LOG_TAG, "Failed to create task for main control. Aborting!");
        abort();
    }

    while (1)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

/* ============================== PRIVATE FUNCTION DEFINITIONS */

static void _main_control_task(void *p_task_params)
{
    // TODO: Remove custom hash
    _g_sha256_input_variables.input_offset = 1000;
    _g_sha256_input_variables.target_solution_mask_offset = 14;
    _g_sha256_input_variables.target_solution[0] = 0x54;
    _g_sha256_input_variables.target_solution[1] = 0x82;

    while (1)
    {
        //i2c_manager_slave_get_written_data((uint8_t *)_g_sha256_input_variables, sizeof(sha256_input_variables_t));
        sha256_calculator_queue_input_put(&_g_sha256_input_variables);
        //i2c_manager_slave_set_data_to_be_read((uint8_t *)_g_sha256_offset_solution, sizeof(sha256_offset_solution_t));
        sha256_calculator_queue_solution_get(&_g_sha256_offset_solution);
        ESP_LOGI(LOG_TAG, "Offset solution %d", _g_sha256_offset_solution.offset_solution);
    }
}

/* ============================== INTERRUPT FUNCTION DEFINITIONS */
