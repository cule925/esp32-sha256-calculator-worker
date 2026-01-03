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

/* ============================== MACRO DEFINITIONS */

/** @brief SHA256 byte digest size */
#define SHA256_BYTE_DIGEST_SIZE         (32)

/* ============================== TYPE DEFINITIONS */

/**
 * @brief The input variables for the calculator.
 * 
 */
typedef struct __attribute__((packed)) {
    uint32_t input_offset;
    uint8_t target_solution_mask_offset;
    uint8_t target_solution[SHA256_BYTE_DIGEST_SIZE];
} sha256_input_variables_t;

/**
 * @brief The solution of the calculator.
 * 
 */
typedef struct __attribute__((packed)) {
    uint32_t offset_solution;
} sha256_offset_solution_t;

/* ============================== PUBLIC FUNCTION DECLARATIONS */

/**
 * @brief Initialize SHA256 calculator.
 * 
 */
void sha256_calculator_init(void);

/**
 * @brief Puts input variables the back of the input variable queue.
 * 
 * @param p_sha256_input_variables Pointer to the input variables which will be copied to the queue.
 */
void sha256_calculator_queue_input_put(sha256_input_variables_t *p_sha256_input_variables);

/**
 * @brief Gets offset solution from the solution queue.
 * 
 * @param p_sha256_input_variables Pointer to the offset solution which will be copied from the queue.
 */
void sha256_calculator_queue_solution_get(sha256_offset_solution_t *p_sha256_offset_solution);

#endif
