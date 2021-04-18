# SmartGarden

Smart garden monitoring for growing vegetables and fruits with an Arduino and Blynk platform.

## Connections

Because there's actually a lot of sensors (and I'm not that good with layout..), below is all the connections for the project.

| Board   | Vin | Gnd |     A0     |   D0   |   D1   |   D2   |     D3     |    D8    |
| ------- | :-: | :-: | :--------: | :----: | :----: | :----: | :--------: | :------: |
| NodeMcu | 12V | Gnd | MUX:COM(Z) | MUX:S0 | MUX:S1 | MUX:S2 | DHT11:Data | NPN:Base |

| Components  | Vcc | Gnd | COM (Z) | S0  | S1  | S2  | CH0 | CH1 | CH2  | CH3  | CH4 |
| ----------- | :-: | :-: | :-----: | :-: | :-: | :-: | :-: | :-: | :--: | :--: | :-: |
| Multiplexer | 12V | Gnd |   A0    | D0  | D1  | D2  | SM1 | SM2 | LDR1 | LDR2 | TDS |

| Components     |   Base   | Emmiter | Collector |
| -------------- | :------: | :-----: | :-------: |
| NPN transistor | D8 (1kΩ) |   Gnd   | Valve[-]  |

| Components      | Positive | Negative | Data | Infos |
| --------------- | :------: | :------: | :--: | :---: |
| Power supply    |   Vin    |   Gnd    |      |       |
|                 |  Valve   |   Gnd    |      |       |
|                 | SD:In[+] |   Gnd    |      |       |
| Valve           |   12V    |   Gnd    |      |       |
| Diode           | Valve[+] | Valve[-] |      |       |
| DHT11           |    5V    |   Gnd    |  D3  | PU-10 |
| Soil Moisture 1 |    5V    |   Gnd    | CH0  |       |
| Soil Moisture 2 |    5V    |   Gnd    | CH1  |       |
| Photocell 1     |    5V    |   Gnd    | CH2  | PD-1  |
| Photocell 2     |    5V    |   Gnd    | CH3  | PD-1  |
| TDS Sensor      |    5V    |   Gnd    | CH4  |       |

<dl>
  <dt>SM</dt>
  <dd>Soil Moisture Sensor.</dd>
  <dt>LDR</dt>
  <dd>Light Dependant Resistor (Photocell).</dd>
  <dt>MUX</dt>
  <dd>Multiplexer.</dd>
  <dt>SD</dt>
  <dd>Step-Down Converter.</dd>
  <dt>PU-Value</dt>
  <dd>Pull-Up Resistors of _Value_ kΩ.</dd>
  <dt>PD-Value</dt>
  <dd>Pull-Down Resistors of _Value_ kΩ.</dd>
</dl>

_12V refers to the power supply._ | _5V to the output of the Step-Down._
_Ground connections are not important as long as they are wired together._ <br>
**Please make sure to double check your wiring before connecting the power supply as a wrong connection could fry your components. Also make sure to check all pull-down or pull-up resistors on the schema.**

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

## Solenoide Water Valve

The water valve need a 12V power supply and about 600 mA, but each items is different
so check your datasheet to be sure. Also, there is no polarity so you can connect either pin
to ground and positive.

## DC-DC Step-Down Converter

Because the solenoide water valve need 12V, we will use a power supply of 12V.
The NodeMcu can be power with the same power supply via the Vin pin that can take
from 6V to 12V because of the on-board 3.3V regulator.
However, because of the analog reading of all the sensors, powering them with the 3.3V
pins of the NodeMcu would limit the range of values to 675 possibilities instead of the
1024 normally. That's why a DC-DC Step-Down converter is used, to output 5V from the 12V
power supply for all the sensors.

## Multiplexer CD74HC4051

The NodeMcu only has one analog input, but there's 5 analog sensors to connect. That's when the multiplexer enter!
A multiplexer gives the possibility to connect multiples analog input to only one output. It does so by controlling which
sensor is poweredm (one at a time), reading its value and sending it before continuing to the next one.
This one has 8 channel, this means we can connect up to 8 analog inputs through the 8 pins _Y_ or _CH_ and output the value
through the _COM_ or _Z_ pin. We can tell it what channel to read from by controlling the state of the 3 _S_ pins, by pulling them
up in a specific way, it powered the matching channel.
Below is a table for each combination (it is basically a binary table with 3 bits):

| S0  | S1  | S2  | Channel |
| --- | :-: | :-: | :-----: |
| 0   |  0  |  0  |    0    |
| 1   |  0  |  0  |    1    |
| 0   |  1  |  0  |    2    |
| 1   |  1  |  0  |    3    |
| 0   |  0  |  1  |    4    |
| 1   |  0  |  1  |    5    |
| 0   |  1  |  1  |    6    |
| 1   |  1  |  1  |    7    |

# Important

To actually work on a NodeMcu, we have to change a value inside the GravityTDS library file. By opening the file: _GravityTDS.cpp_
and changes the value of line 39: `this->pin = A1;` to `this->pin = A0;` because of the one and only analog pin of the NodeMcu.
This file is inside the folder GravityTDS.

## [Installation of Blynk Local Server](https://github.com/blynkkk/blynk-server)

For my project I use the Blynk Local Server. The installation can be found [on their GitHub](https://github.com/blynkkk/blynk-server) and it is pretty straight-forward.
The advantages of using your own local server is that you have a full control on what's happening, on the data that is being retrieved (could be
useful if like GPS was enabled on the app) so that your data isn't stored inside blynk server. Also, it is actually faster to use a local server
because of the distance the signal has to travel and more importantly, you can have as much Blynk Energy you want (so that you can customize your app by
adding an infinite number of widgets).

---

## © Copyright

<dl>
  <dt>Adidax: DHT11 Library</dt>
  <dd><a href="https://github.com/adidax/dht11">Github</a></dd>
  <dt>Blynkkk: Blynk-library</dt>
  <dd><a href="https://github.com/blynkkk/blynk-library">Github</a></dd>
  <dt>How2Electronics: TDS Meter</dt>
  <dd><a href="https://how2electronics.com/iot-based-tds-meter-using-esp8266-for-water-quality-monitoring/">Github</a></dd>
</dl>
