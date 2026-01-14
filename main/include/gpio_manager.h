/**
 * @file gpio_manager.h
 * @author Iwan Ćulumović
 * @brief See gpio_manager.c file.
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef __GPIO_MANAGER_H__
#define __GPIO_MANAGER_H__

/* ============================== INCLUDES */

/* ============================== MACRO DEFINITIONS */

/* ============================== TYPE DEFINITIONS */

/* ============================== PUBLIC FUNCTION DECLARATIONS */

/**
 * @brief Initialize GPIOs.
 * 
 */
void gpio_manager_init(void);

/**
 * @brief Activates interrupt line.
 * 
 * @param p_buf Pointer to the buffer from where the data will be copied in the send ring buffer.
 * @param buf_size Size of the buffer.
 */
void gpio_set_interrupt_out(void);

/**
 * @brief Deactivates interrupt line.
 * 
 * @param p_buf Pointer to the buffer where the data will be copied when written.
 * @param buf_size Size of the buffer.
 */
void gpio_reset_interrupt_out(void);

#endif
