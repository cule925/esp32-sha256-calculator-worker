idf_component_register(
    SRCS "i2c_manager.c" "main.c"
    INCLUDE_DIRS "include"
    PRIV_REQUIRES esp_driver_i2c
)
