# Compile and flash using arduino-cli

## Install arduino-cli

```
curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sudo sh
```

## Compile

Compile **device** sketch using arduino-cli. Target board esp8266 **wemos d1 mini**. Output directory **build**.

```
arduino-cli compile --fqbn esp8266:esp8266:d1_mini --output-dir build device
```

## Flash

```
arduino-cli upload --fqbn esp8266:esp8266:d1_mini -p /dev/ttyUSB0 --input-file build/device.ino.bin
```
