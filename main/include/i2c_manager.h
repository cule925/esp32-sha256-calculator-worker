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

/* ============================== PUBLIC FUNCTION DECLARATIONS */

/**
 * @brief Initialize I2C slave.
 * 
 */
void i2c_manager_slave_init(void);

/**
 * @brief Sets data in the send ring buffer that will be read when master issues a read command.
 * 
 * @param p_buf Pointer to the buffer from where the data will be copied in the send ring buffer.
 * @param buf_size Size of the buffer.
 */
void i2c_manager_slave_set_data_to_be_read(uint8_t *p_buf, size_t buf_size);

/**
 * @brief Gets data that master wrote.
 * 
 * @param p_buf Pointer to the buffer where the data will be copied when written.
 * @param buf_size Size of the buffer.
 */
void i2c_manager_slave_get_written_data(uint8_t *p_buf, size_t buf_size);

#endif
