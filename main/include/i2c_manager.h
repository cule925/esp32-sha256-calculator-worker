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
 * @return int Returns 0 on success, else -1.
 */
int i2c_manager_slave_init(void);

#endif
