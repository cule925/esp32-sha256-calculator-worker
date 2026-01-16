/**
 * @file sha256_calculator.h
 * @author Iwan Ćulumović
 * @brief See sha256_calculator.c file.
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef __SHA256_CALCULATOR_H__
#define __SHA256_CALCULATOR_H__

/* ============================== INCLUDES */
#include <stdbool.h>
#include <stdint.h>

/* ============================== MACRO DEFINITIONS */

/** @brief SHA256 byte digest size */
#define SHA256_BYTE_DIGEST_SIZE         (32)

/* ============================== TYPE DEFINITIONS */

/**
 * @brief Calculator input variables.
 * 
 */
typedef struct __attribute__((packed)) {
    uint32_t input_offset;
    uint8_t target_solution_mask_offset;
    uint8_t target_solution[SHA256_BYTE_DIGEST_SIZE];
} sha256_input_variables_t;

/**
 * @brief Calculator input variables queue element.
 * 
 */
typedef struct __attribute__((packed)) {
    sha256_input_variables_t sha256_input_variables;
    uint8_t puzzle_id;
} sha256_input_variables_queue_element_t;

/**
 * @brief Calculator solution.
 * 
 */
typedef struct __attribute__((packed)) {
    uint32_t offset_solution;
} sha256_offset_solution_t;

/**
 * @brief Calculator solution queue element.
 * 
 */
typedef struct __attribute__((packed)) {
    sha256_offset_solution_t sha256_offset_solution;
    uint8_t puzzle_id;
} sha256_offset_solution_queue_element_t;

/* ============================== PUBLIC FUNCTION DECLARATIONS */

/**
 * @brief Initialize SHA256 calculator.
 * 
 */
void sha256_calculator_init(void);

/**
 * @brief Puts input variables the back of the input variable queue. Blocking function.
 * 
 * @param p_sha256_input_variables_queue_element Pointer to the input variables queue element which will be copied to the queue.
 */
void sha256_calculator_queue_input_put(sha256_input_variables_queue_element_t *p_sha256_input_variables_queue_element);

/**
 * @brief Gets offset solution from the solution queue if there is any. Non-blocking function.
 * 
 * @param p_sha256_offset_solution_queue_element Pointer to the offset solution queue element which will be copied from the queue.
 * 
 * @return bool Returns true if solution to a puzzle found, else false.
 */
bool sha256_calculator_queue_solution_get(sha256_offset_solution_queue_element_t *p_sha256_offset_solution_queue_element);

#endif
