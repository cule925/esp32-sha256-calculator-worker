/**
 * @file spi_manager.c
 * @author Iwan Ćulumović
 * @brief SPI manager module.
 * 
 * @copyright Copyright (c) 2026
 * 
 */

/* ============================== INCLUDES */

#include "esp_log.h"
#include "sdkconfig.h"
#include "comm/driver/spi_manager.h"

/* ============================== MACRO DEFINITIONS */

/** @brief Log tag. */
#define LOG_TAG                                 ("SPI_MANAGER")

/* ============================== TYPE DEFINITIONS */

/* ============================== PRIVATE FUNCTION DECLARATIONS */

/* ============================== PRIVATE VARIABLES */

/* ============================== PUBLIC VARIABLES */

/* ============================== PUBLIC FUNCTION DEFINITIONS */

void spi_manager_slave_init(void)
{
    // TODO:

    ESP_LOGI(LOG_TAG, "Initialized slave.");
}

void spi_manager_slave_set_data_to_be_read(uint8_t *p_buf, size_t buf_size)
{
    // TODO:
}

bool spi_manager_slave_receive_data(uint8_t *p_buf, size_t buf_size)
{
    bool b_received_data = false;

    // TODO:

    return b_received_data;
}

/* ============================== PRIVATE FUNCTION DEFINITIONS */

/* ============================== INTERRUPT FUNCTION DEFINITIONS */
