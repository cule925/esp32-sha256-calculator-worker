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
    sha256_input_variables_queue_element_t sha256_input_variables_queue_element = {0};
    sha256_offset_solution_queue_element_t sha256_offset_solution_queue_element = {0};
    uint8_t current_puzzle_id = 0;
    bool b_received_new_input = false;
    bool b_received_solution = false;

    while (1)
    {
        /* Check for new input and reset flag */
        b_received_new_input = i2c_manager_slave_receive_data((uint8_t*)&sha256_input_variables_queue_element, sizeof(sha256_input_variables_queue_element));

        /* If input received */
        if (true == b_received_new_input)
        {
            ESP_LOGI(LOG_TAG, "Received new input! Puzzle ID: %d", sha256_input_variables_queue_element.puzzle_id);

            /* Set new puzzle id */
            current_puzzle_id = sha256_input_variables_queue_element.puzzle_id;

            /* Send data for calculation */
            sha256_calculator_queue_input_put(&sha256_input_variables_queue_element);
        }

        /* Check for solution */
        b_received_solution = sha256_calculator_queue_solution_get(&sha256_offset_solution_queue_element);

        /* If received solution and puzzle ID matches */
        if ((true == b_received_solution) && (current_puzzle_id == sha256_offset_solution_queue_element.puzzle_id))
        {
            ESP_LOGI(LOG_TAG, "Offset solution: %d", sha256_offset_solution_queue_element.sha256_offset_solution.offset_solution);

            /* Set data to be read and set flag */
            i2c_manager_slave_set_data_to_be_read((uint8_t *)&sha256_offset_solution_queue_element, sizeof(sha256_offset_solution_queue_element));
        }
    }
}

/* ============================== INTERRUPT FUNCTION DEFINITIONS */
