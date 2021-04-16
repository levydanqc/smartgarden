/***************************************************


 ***************************************************
  © Copyright : see README.md
****************************************************/

/* PINOUT */
#define dhtPin 5
#define tdsPin A2
#define moisture1Pin A0
#define moisture2Pin A1
#define ldr1Pin A3
#define ldr2Pin A4
/* END PINOUT */

/* Blynk Setup*/
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
char auth[] = "_Vgk1QOn3lLHxauDHBVv-R-Zk3-FEHXU";
char ssid[] = "cavabienaller";
char pass[] = "PRIVELEVY";
BlynkTimer timer;
/* END Blynk */

/* DHT11 sensor */
#include "dht11.h"
dht11 Dht;
float humidity;
float temperature;
/* END DHT11 */

/* TDS sensor */
#include "GravityTDS.h"
#include <EEPROM.h>
GravityTDS Tds;
float tds;
/* END TDS */

/* Moisture sensors */
#define MOISTURE_WATER 250
#define MOISTURE_AIR 487
float moisture1;
float moisture2;
/* END Moisture */

/* LDR */
#define LDR_MAX 0
#define LDR_MIN 0
float ldr1;
float ldr2;
/* END LDR */

/* Blynk Configuration */
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
char auth[] = "REVIEW:";
char ssid[] = "REVIEW:";
char pass[] = "REVIEW:";
/* END Blynk */

void setup()
{
    Serial.begin(9600);

    /* TDS sensor */
    Tds.setPin(tdsPin);    // Pin of sensor
    Tds.setAref(5.0);      // Reference voltage on ADC
    Tds.setAdcRange(1024); // 10 bit ADC for Uno and Nano
    Tds.begin();
    /* END TDS sensor */

    /* Blynk Setup */
    Blynk.begin(auth, ssid, pass, "192.168.1.100", 8080);
    timer.setInterval(1000L, getData());
    /* END Blynk */
}

void loop()
{
    Blynk.run();
    timer.run();
}

void getData()
{
    float h = getHum();
    float t = getTemp();
    float tds = getTds();
    float m1 = getMoisture1();
    float m2 = getMoisture2();
    float l1 = getLDR1();
    float l2 = getLDR2();
    Blynk.virtualWrite(V5, h);
    Blynk.virtualWrite(V6, t);
}

/**
  * Get value from TDS sensor in ppm.
  *
  * @param temperature Temperature at which measure was taken.
  * @return Tds value of sensor in ppm.
*/
float getTds(float temperature)
{
    Tds.setTemperature(temperature); // Set temperature for compensation
    Tds.update();                    // Retrieve value from sensor
    return Tds.getTdsValue();        // Return TDS value
}

/**
  * Get humidity from DHT11 sensor.
  *
  * @return Humidity from DHT11 in %.
*/
void getTemp()
{
    Dht.read(dhtPin);              // Retreive value
    return (float)Dht.temperature; // Return humidity
}

/**
  * Get temperature from DHT11 sensor.
  *
  * @return Temperature from DHT11 in °C.
*/
void getHum()
{
    Dht.read(dhtPin);           // Retreive value
    return (float)Dht.humidity; // Return humidity
}

/**
  * Get soil moisture from sensor 1 in percent.
  *
  * @return Soil moisture from sensor 1 in %.
*/
float getMoisture1()
{
    for (int i = 0; i <= 100; i++) // Retrieving a hundred value for accuracy
    {
        moisture1 += analogRead(moisture1Pin);
        delay(1);
    }
    moisture1 = moisture1 / 100.0 // Get average value
                // Map value between 0 and 100 to get a percentage
                moisture1 = map(moisture1, MOISTURE_AIR, MOISTURE_WATER, 0, 100);
    return constrain(moisture1, 0, 100); // Constrain value inside [0, 100]
}

/**
  * Get soil moisture from sensor 2 in percent.
  *
  * @return Soil moisture from sensor 2 in %.
*/
float getMoisture2()
{
    for (int i = 0; i <= 100; i++) // Retrieving a hundred value for accuracy
    {
        moisture2 += analogRead(moisture2Pin);
        delay(1);
    }
    moisture2 = moisture2 / 100.0 // Get average value
                // Map value between 0 and 100 to get a percentage
                moisture2 = map(moisture2, MOISTURE_AIR, MOISTURE_WATER, 0, 100);
    return constrain(moisture2, 0, 100); // Constrain value inside [0, 100]
}

/**
  * Get light intensity from sensor 1 in percent.
  *
  * @return Light intensity from sensor 1 in %.
*/
float getLDR1()
{
    for (int i = 0; i <= 100; i++)
    {
        ldr1 += analogRead(ldr1Pin);
        delay(1)
    }
    ldr1 = ldr1 / 100.0 ldr1 = map(ldr1, LDR_MIN, LDR_MAX, 0, 100);
    return constrain(ldr1, 0, 100);
}

/**
  * Get light intensity from sensor 2 in percent.
  *
  * @return Light intensity from sensor 2 in %.
*/
float getLDR2()
{
    for (int i = 0; i <= 100; i++)
    {
        ldr2 += analogRead(ldr2Pin);
        delay(1)
    }
    ldr2 = ldr2 / 100.0 ldr2 = map(ldr2, LDR_MIN, LDR_MAX, 0, 100);
    return constrain(ldr2, 0, 100);
}
