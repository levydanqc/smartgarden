# SmartGarden

Smart garden monitoring for growing vegetables and fruits with an Arduino and Blynk platform.

## TDS Sensor

To enter in calibration mode, send via the Serial:

> - enter -> let you enter inside calibration mode
> - cal:value -> send known TDS value of solution (@25°C)
>   - _e.g._: cal:504
> - exit -> save the calibration into the sensor and exit calibration mode

## Soil Moisture Sensor

To calibrate the soil moisture sensor:

1. Put sensor in the air, retrieve value and set it to MOISTURE_AIR
2. Put sensor in water, retrieve value and set it to MOISTURE_WATER

## LDR Sensor

To calibrate the light dependant resistor (LDR):

1. Put the sensor in a dark room with as little light as possible, retrieve value and set it to LDR_MIN
2. Put the sensor in a bright sunny day in the direction of the Sun, retrieve value and set it to LDR_MAX


---

## © Copyright

[Adidax: DHT11] (https://github.com/adidax/dht11)
[DFRobot Gravity: Analog TDS Sensor/Meter](https://github.com/DFRobot/GravityTDS)
