/**
 * @file comm_manager.c
 * @author Iwan Ćulumović
 * @brief Communication manager module.
 * 
 * @copyright Copyright (c) 2026
 * 
 */

/* ============================== INCLUDES */

#include "esp_log.h"
#include "sdkconfig.h"
#include "comm_manager.h"
#include "sha256_calculator.h"

#ifdef CONFIG_COMM_PROTOCOL_I2C
#include "i2c_manager.h"
#elif CONFIG_COMM_PROTOCOL_SPI

#endif

/* ============================== MACRO DEFINITIONS */

/** @brief Log tag. */
#define LOG_TAG                                 ("COMM_MANAGER")

/** @brief I2C on receive queue length. */
#define I2C_ON_RECEIVE_QUEUE_LENGTH         (10)

/* ============================== TYPE DEFINITIONS */

/* ============================== PRIVATE FUNCTION DECLARATIONS */

/* ============================== PRIVATE VARIABLES */

/* ============================== PUBLIC VARIABLES */

/* ============================== PUBLIC FUNCTION DEFINITIONS */

void comm_manager_init(void)
{
#ifdef CONFIG_COMM_PROTOCOL_I2C
    i2c_manager_slave_init(I2C_ON_RECEIVE_QUEUE_LENGTH, sizeof(sha256_input_variables_queue_element_t));
#elif CONFIG_COMM_PROTOCOL_SPI

#endif
}

void comm_manager_set_data_to_be_read(uint8_t *p_buf, size_t buf_size)
{
#ifdef CONFIG_COMM_PROTOCOL_I2C
    i2c_manager_slave_set_data_to_be_read(p_buf, buf_size);
#elif CONFIG_COMM_PROTOCOL_SPI

#endif
}

bool comm_manager_receive_data(uint8_t *p_buf, size_t buf_size)
{
    bool b_received_new_input = false;

#ifdef CONFIG_COMM_PROTOCOL_I2C
    b_received_new_input = i2c_manager_slave_receive_data(p_buf, buf_size);
#elif CONFIG_COMM_PROTOCOL_SPI

#endif
    return b_received_new_input;
}

/* ============================== PRIVATE FUNCTION DEFINITIONS */

/* ============================== INTERRUPT FUNCTION DEFINITIONS */
