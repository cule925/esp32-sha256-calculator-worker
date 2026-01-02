idf_component_register(
    SRCS "sha256_calculator.c" "i2c_manager.c" "main.c"
    INCLUDE_DIRS "include"
    PRIV_REQUIRES esp_driver_i2c
)
