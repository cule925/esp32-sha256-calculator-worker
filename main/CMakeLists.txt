idf_component_register(
    SRCS "flow_control.c" "gpio_manager.c" "sha256_calculator.c" "i2c_manager.c" "main.c"
    INCLUDE_DIRS "include"
    PRIV_REQUIRES esp_driver_i2c
    PRIV_REQUIRES mbedtls
    PRIV_REQUIRES esp_driver_gpio
)
