/**
 * @file sha256_calculator.c
 * @author Iwan Ćulumović
 * @brief SHA256 calculator module.
 * 
 * @copyright Copyright (c) 2026
 * 
 */

/* ============================== INCLUDES */

#include <stdio.h>
#include "esp_log.h"
#include "sdkconfig.h"
#include "sha256_calculator.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "mbedtls/sha256.h"

/* ============================== MACRO DEFINITIONS */

/** @brief Log tag. */
#define LOG_TAG                                 ("SHA256_CALC")

/** @brief SHA256 input queue size. */
#define SHA256_INPUT_QUEUE_SIZE                 (1)

/** @brief SHA256 solution queue size. */
#define SHA256_SOLUTION_QUEUE_SIZE              (1)

/** @brief Calculate SHA256 task stack depth. */
#define TASK_SHA256_CALC_STACK_DEPTH            (2048)

/** @brief Calculate SHA256 task priority. */
#define TASK_SHA256_CALC_PRIORITY               (0)

/* ============================== TYPE DEFINITIONS */

/* ============================== PRIVATE FUNCTION DECLARATIONS */

/**
 * @brief Task that calculates SHA256 given the input variables.
 * 
 * @param p_task_params Task parameters (not used).
 */
static void _calculate_sha256_task(void *p_task_params);

/* ============================== PRIVATE VARIABLES */

/** @brief SHA256 input queue. */
static QueueHandle_t _g_queue_sha256_input = NULL;

/** @brief SHA256 solution queue. */
static QueueHandle_t _g_queue_sha256_solution = NULL;

/** @brief SHA256 calculate task handle. */
static TaskHandle_t _g_task_handle_sha256_calc = NULL;

/* ============================== PUBLIC VARIABLES */

/* ============================== PUBLIC FUNCTION DEFINITIONS */

void sha256_calculator_init(void)
{
    BaseType_t result = pdPASS;

    _g_queue_sha256_input = xQueueCreate(SHA256_INPUT_QUEUE_SIZE, sizeof(sha256_input_variables_t));
    if (NULL == _g_queue_sha256_input)
    {
        ESP_LOGE(LOG_TAG, "Failed to create queue for SHA256 input. Aborting!");
        abort();
    }

    _g_queue_sha256_solution = xQueueCreate(SHA256_SOLUTION_QUEUE_SIZE, sizeof(sha256_offset_solution_t));
    if (NULL == _g_queue_sha256_solution)
    {
        ESP_LOGE(LOG_TAG, "Failed to create queue for SHA256 solution. Aborting!");
        abort();
    }

    result = xTaskCreate(_calculate_sha256_task, "SHA256_CALC", TASK_SHA256_CALC_STACK_DEPTH, NULL, TASK_SHA256_CALC_PRIORITY, &_g_task_handle_sha256_calc);
    if (pdPASS != result)
    {
        ESP_LOGE(LOG_TAG, "Failed to create task for SHA256 calculation. Aborting!");
        abort();
    }

    ESP_LOGI(LOG_TAG, "Initialized calculator.");
}

void sha256_calculator_queue_input_put(sha256_input_variables_t *p_sha256_input_variables)
{
    xQueueSendToBack(_g_queue_sha256_input, (void*)p_sha256_input_variables, portMAX_DELAY);
}

void sha256_calculator_queue_solution_get(sha256_offset_solution_t *p_sha256_offset_solution)
{
    xQueueReceive(_g_queue_sha256_solution, (void*)p_sha256_offset_solution, portMAX_DELAY);
}

/* ============================== PRIVATE FUNCTION DEFINITIONS */

static void _calculate_sha256_task(void *p_task_params)
{
    sha256_input_variables_t sha256_input_variables = {0};
    sha256_offset_solution_t sha256_offset_solution = {0};
    BaseType_t result = pdFALSE;
    uint8_t hash[SHA256_BYTE_DIGEST_SIZE] = {0};
    uint8_t full_bytes = 0;
    uint8_t remaining_bits = 0;
    uint8_t remaining_bits_mask = 0x00;
    int byte_cmp = 1;
    int bit_cmp = 1;

    while (1)
    {
        /* Try to read from queue, non-blocking */
        result = xQueueReceive(_g_queue_sha256_input, (void*)(&sha256_input_variables), 0);

        /* If new inputs read, recalculate parameters */
        if (pdTRUE == result)
        {
            full_bytes = (sha256_input_variables.target_solution_mask_offset + 1) / 8;
            remaining_bits = (sha256_input_variables.target_solution_mask_offset + 1) % 8;
            remaining_bits_mask = 0xFF << (8 - remaining_bits);

            byte_cmp = (0 == full_bytes) ? 0 : 1;
            bit_cmp = (0 == remaining_bits) ? 0 : 1;
        }

        /* Hash the input offset */
        mbedtls_sha256((uint8_t *)(&sha256_input_variables.input_offset), sizeof(sha256_input_variables.input_offset), (unsigned char *)&hash, 0);

        /* Compare the output hash with the target */
        if (0 != full_bytes)
        {
            byte_cmp = memcmp(hash, sha256_input_variables.target_solution, full_bytes);
        }
        if (0 != remaining_bits)
        {
            bit_cmp = ((hash[full_bytes] & remaining_bits_mask) == (sha256_input_variables.target_solution[full_bytes] & remaining_bits_mask)) ? 0 : 1;
        }

        /* Send discovered current offset if match, non-blocking */
        if ((0 == byte_cmp) && (0 == bit_cmp))
        {
            sha256_offset_solution.offset_solution = sha256_input_variables.input_offset;
            xQueueSendToBack(_g_queue_sha256_solution, (void*)(&sha256_offset_solution), 0);

            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        /* Increment the offset if no match */
        else
        {
            sha256_input_variables.input_offset++;
        }
    }
}

/* ============================== INTERRUPT FUNCTION DEFINITIONS */
