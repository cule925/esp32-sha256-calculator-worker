/**
 * @file comm_manager.h
 * @author Iwan Ćulumović
 * @brief See comm_manager.c file.
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef __COMM_MANAGER_H__
#define __COMM_MANAGER_H__

/* ============================== INCLUDES */
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/* ============================== MACRO DEFINITIONS */

/* ============================== TYPE DEFINITIONS */

/* ============================== PUBLIC FUNCTION DECLARATIONS */

/**
 * @brief Initialize communication manager.
 * 
 */
void comm_manager_init(void);

/**
 * @brief Send data to master.
 * 
 * @param p_buf Pointer to the buffer from where the data will be copied to the send buffer. Blocking function.
 * @param buf_size Size of the buffer.
 */
void comm_manager_set_data_to_be_read(uint8_t *p_buf, size_t buf_size);

/**
 * @brief Receive data from master.
 * 
 * @param p_buf Pointer to the buffer to where the data will be copied from the receive buffer.
 * @param buf_size Size of the buffer.
 * 
 * @return bool Returns true if new data came from master, else false.
 */
bool comm_manager_receive_data(uint8_t *p_buf, size_t buf_size);

#endif
