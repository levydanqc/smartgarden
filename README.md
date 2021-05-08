Quick links:

- [Latest release](https://gitlab.com/levydanqc/smartgarden/-/archive/v.1.0.0/smartgarden-v.1.0.0.zip)
- [Arduino Projet](https://arduino-esp8266.readthedocs.io/en/latest/)

===========================================

# SmartGardener

Smart garden monitoring system for growing vegetables and fruits with an NodeMcu and Blynk platform.

## My Story

At some points in life, we all tried to do some sort of gardening. Whether it is to embellish our house, to tried to grow some food or to prove to ourselves that we're able to take care of something else and not let it die, the process of planting some seeds in the ground isn't easy.
The problem is that humankind isn't the most reliable. How many of us forgot to take the cake out of the oven, to put the lid back on the peanut butter jar or even to water the plants and thought "Man ! Not again.". What if I told you there is an other way ? Today's technology let us automate virtually anything, from turning on and off the lights, brewing coffee each morning or cooking delicious meal.
Combining automation, micro-controllers and plants isn't new, there's plenty of projects on the internet about people trying to create their own garden monitoring system. Mine isn't that much different than theirs except that for one, almost everything is combined together. I've searched for a project where there would be soil moisture sensors, lights sensors, solenoid water valve, etc. as well as being able to control everything remotely before deciding to build my own.
It does work like I wanted, but there is so much more sensors and logic that I could and would like to add to this project, like TDS sensor (for water quality), controlling multiples valve for multiples garden, but one thing at a time.

## Connections

Because there's actually a lot of sensors (and I'm not that good with layout..), below is all the connections for the project.
_In the Schemas folder, there is a schema for the connections of each sensors._

| Board   | Vin | Gnd |     A0     |    D0     |     D1     |     D2      |   D5   |   D6   |   D7   |
| ------- | :-: | :-: | :--------: | :-------: | :--------: | :---------: | :----: | :----: | :----: |
| NodeMcu | 12V | Gnd | MUX:COM(Z) | Flow:Data | DHT11:Data | Mosfet:Gate | MUX:S0 | MUX:S1 | MUX:S2 |

| Components  | Vcc | Gnd | COM (Z) | S0  | S1  | S2  | CH0 | CH1 | CH2  | CH3  |
| ----------- | :-: | :-: | :-----: | :-: | :-: | :-: | :-: | :-: | :--: | :--: |
| Multiplexer | 12V | Gnd |   A0    | D5  | D6  | D7  | SM1 | SM2 | LDR1 | LDR2 |

| Components |    Gate    | Source |  Drain   |
| ---------- | :--------: | :----: | :------: |
| Mosfet     | D2 (PU-10) |  Gnd   | Valve[-] |

| Components      | Positive | Negative | Data | Infos |
| --------------- | :------: | :------: | :--: | :---: |
| Power supply    |   Vin    |   Gnd    |      |       |
|                 |  Valve   |   Gnd    |      |       |
|                 | SD:In[+] |   Gnd    |      |       |
| Valve           |   12V    |   Gnd    |      |       |
| Diode           | Valve[+] | Valve[-] |      |       |
| DHT11           |    3V    |   Gnd    |  D1  | PU-10 |
| Soil Moisture 1 |    3V    |   Gnd    | CH0  |       |
| Soil Moisture 2 |    3V    |   Gnd    | CH1  |       |
| Photocell 1     |    3V    |   Gnd    | CH2  | PD-1  |
| Photocell 2     |    3V    |   Gnd    | CH3  | PD-1  |
| Flow Sensor     |    3V    |   Gnd    |  D0  |       |

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

_12V refers to the power supply._ | _3V to the output of the NodeMcu._
_Ground connections are not important as long as they are wired together._ <br>
**Please make sure to double check your wiring before connecting the power supply as a wrong connection could fry your components. Also make sure to check all pull-down or pull-up resistors on the schema.**

## Configuration

To configure the project, all you need to do is enter your own informations from line 17 to 20. You will need the name and password of your WIFI and a hostname for the board as well as a password that you can create (It is optional, but for security purposes we were more comfortable using one).
The hostname will appear in the PORT setting of the Arduino IDE when you want to upload code and the password will be asked after compiling, just before uploading.

## Over-the-Air (OTA) Upload

The NodeMcu will be placed inside a waterproof and weather-resistant box near the plants and a power outlet so it won't be
easily accessible. A library called ArduinoOTA (Over-the-Air) let us upload new code to the board without being physically
connected to it, we only need to be in the same network for that to work.
The first upload need, obviously, to be wired to the board, but after that we can select the NodeMcu in the PORT configuration of the Arduino IDE to be connected to it.

**It is important to upload the OTA code and librairies each time to be sure it will still be accessible.**

## Flow Sensor

The flow sensor is being used primarily for testing the opening command of the valve.
It precision or accuracy isn't really what matters most as any value means the valve was opened and the system is working well.
His working principle is based on the Hall Effect: there is a rotor inside that makes loop when water goes though it with a magnet on one of the propeller and it outputs a signal each time the magnet complete one loop.

In the datasheet of the sensor there is coefficient often written like: `F = 7.5Q`. This number represents the quantity of water that has passed for each loop and is used to calculate the flow rate.

The formula is quite simple:

```
Flow rate (L/min) = Nb of loop for 1 sec / flow coefficient.
```

In this program, we used `attachInterrupt()` function from the arduino library that can asynchronously do a task when a physical pin receive a signal. Because of the way Blynk works we cannot use the `delay()` function to wait 1 second so instead, we began the interrupts (so that it increment a variable each time the signal is received) and at the next iteration we stop the interrupts and start calculate the flow rate using the actual time passed from the moment started it.

The adapted formula is then:

```
Flow rate (L/min) = ((1000.0 / (now() - start)) * count) / flowQ.
```

_We need to convert the number of loop from the nuber of seconds it was measure to 1 second._
_Where now() is the current time in unix (since epoch), start is the time we start counting signals (also in unix), count is the number of loop the magnet did and flowQ is the flow sensor coefficient._

## Soil Moisture Sensor

To calibrate the soil moisture sensor:

1. Put sensor in the air, retrieve value and set it to MOISTURE_AIR
2. Put sensor in water, retrieve value and set it to MOISTURE_WATER

## Solenoide Water Valve

The water valve need a 12V power supply and about 600 mA, but each items is different
so check your datasheet to be sure. Also, there is no polarity so you can connect either pin
to ground and positive.

## Mosfet IRF540N

A simple NPN transistor cannot take the power the solenoid valve needs, that is why a mosfet transistor is used. We control the solenoid valve with the Gate pin of the mosfet that, when HIGH, let current flow between Source and Drain.

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

## [Installation of Blynk Local Server](https://github.com/blynkkk/blynk-server)

For my project we used the Blynk Local Server. The installation can be found [on their GitHub](https://github.com/blynkkk/blynk-server) and it is pretty straight-forward.
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
  <dt>The HookUp: GPIO_Limitations_ESP8266_NodeMCU.jpg</dt>
  <dd><a href="https://github.com/thehookup/Wireless_MQTT_Doorbell/blob/master/GPIO_Limitations_ESP8266_NodeMCU.jpg">Github</a></dd>
</dl>
