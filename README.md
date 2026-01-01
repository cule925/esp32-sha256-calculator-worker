# ESP32 I2C slave SHA256 calculator

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
