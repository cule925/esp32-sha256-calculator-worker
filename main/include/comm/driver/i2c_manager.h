/**
 * @file i2c_manager.h
 * @author Iwan Ćulumović
 * @brief See i2c_manager.c file.
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef __I2C_MANAGER_H__
#define __I2C_MANAGER_H__

/* ============================== INCLUDES */

/* ============================== MACRO DEFINITIONS */

/* ============================== TYPE DEFINITIONS */

/* ============================== PUBLIC FUNCTION DECLARATIONS */

/**
 * @brief Initialize I2C slave.
 * 
 * @param on_receive_queue_length On receive queue length.
 * @param on_receive_queue_item_size On receive queue item size.
 */
void i2c_manager_slave_init(int on_receive_queue_length, int on_receive_queue_item_size);

/**
 * @brief Sets data in the send ring buffer that will be read when master issues a read request. Blocking function.
 * 
 * @param p_buf Pointer to the buffer from where the data will be copied to the send buffer.
 * @param buf_size Size of the buffer.
 */
void i2c_manager_slave_set_data_to_be_read(uint8_t *p_buf, size_t buf_size);

/**
 * @brief Reads new data if master wrote it. Non-blocking function.
 * 
 * @param p_buf Pointer to the buffer to where the data will be copied from the receive buffer.
 * @param buf_size Size of the buffer.
 * 
 * @return bool Returns true if new data came from master, else false.
 */
bool i2c_manager_slave_receive_data(uint8_t *p_buf, size_t buf_size);

#endif
