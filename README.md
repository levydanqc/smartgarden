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

## DC-DC Step-down Converter

Because the solenoide water valve need 12V, we will use a power supply of 12V.
The arduino nano can be power with the same power supply via the Vin pin that can take
from 6V to 12V because of the on-board 5V regulator. Unfortunately, the ESP8266 cannot be powered by the same
power supply as it need 3.3V. The step-down converter is used to convert 12V to 3.3V and can power up to 3A.

> Note: The ESP8266 cannot be powered by the 3.3V pin of the arduino (as well as the 5V pin) because
> it need betweem 400mA to 600mA compared to the 20mA-40mA that output the arduino.
> That is why a external power supply (in this project an AC to DC 12V 600mA was used) is being used to power
> the arduino and the valve independently.

---

## © Copyright

[Adidax: DHT11] (https://github.com/adidax/dht11)
[DFRobot Gravity: Analog TDS Sensor/Meter](https://github.com/DFRobot/GravityTDS)
[Blynkkk: Blynk-library] (https://github.com/blynkkk/blynk-library)