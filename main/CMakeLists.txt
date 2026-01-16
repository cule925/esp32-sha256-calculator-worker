idf_component_register(
    SRCS "comm_manager.c" "flow_control.c" "gpio_manager.c" "sha256_calculator.c" "main.c"
    INCLUDE_DIRS "include"
    PRIV_REQUIRES esp_driver_i2c
    PRIV_REQUIRES mbedtls
    PRIV_REQUIRES esp_driver_gpio
)

if(CONFIG_COMM_PROTOCOL_I2C)
    target_sources(${COMPONENT_LIB} PRIVATE "i2c_manager.c")
elseif(CONFIG_PROTOCOL_SPI)
endif()