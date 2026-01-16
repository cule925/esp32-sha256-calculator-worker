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

## Enter configuration

Enter the `menuconfig` configuration screen by executing:

```
idf.py menuconfig
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

To have multiple ESP32 slave devices on the same I2C bus, each of their I2C slave addresses need to be unique. To add a new ESP32 slave device onto the bus, enter the `menuconfig`, go to `App setup`, select `I2C` under `Communication protocol`, enter the `I2C setup` submenu and edit the `I2C slave address` to a desired new unused address. Save the changes, rebuild the firmware and flash it onto the new ESP32 device to be added to the I2C bus. Repeat this step with each new device you wish to add.

Saved changes in menuconfig edit the `sdkconfig` file.

## SPI - multiple slave devices

**TODO:**
