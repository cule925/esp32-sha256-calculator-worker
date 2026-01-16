# ESP32 SHA256 calculator worker

To setup your ESP-IDF version 5.5.2 environment in your terminal, execute:

```
source ~/esp/v5.5.2/esp-idf/export.sh
```

## Setting the target

Set the target to ESP32 by executing:

```
idf.py set-target esp32
```

## Building the binary

Build the project by executing:

```
idf.py build
```

## Flashing the binary

Flash the binary with the following command:

```
idf.py -p <port> flash
```

## Monitoring the device

If you wish to monitor the device over USB serial, execute:

```
idf.py -p <port> monitor -b <Baud rate>
```

Default Baud rate is 115200. Press `Ctrl + ]` to exit the monitor.

## I2C - multiple slave devices

To have multiple ESP32 slave devices on the same I2C bus, they must have different I2C slave addresses. Enter the menuconfig using the following command:

```
idf.py menuconfig
```

Go to "App setup" and edit the "I2C slave address" to a desired new unused address. **Build and flash** to the new ESP32 device to be added to the I2C bus. Repeat this step with each new added device.

Saved changes in menuconfig edit the `sdkconfig` file.
