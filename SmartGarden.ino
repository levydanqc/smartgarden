/***************************************************


 ***************************************************
 © Copyright : see README.md
****************************************************/

/* PINOUT */
#define dhtPin 5
#define tdsPin A2
#define moisture1Pin A0
#define moisture2Pin A1
/* END PINOUT */


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

void setup()
{
    Serial.begin(9600);

    /* TDS sensor */
    Tds.setPin(tdsPin); // Pin of sensor
    Tds.setAref(5.0) // Reference voltage on ADC
    Tds.setAdcRange(1024) // 10 bit ADC for Uno and Nano
    Tds.begin()
    /* END TDS sensor */
}

void loop()
{

}

/**
 * Get value from TDS sensor in ppm.
 * 
 * @param temperature Temperature at which measure was taken.
 * @return Tds value of sensor in ppm.
*/
float getTds(float temperature)
{
    Tds.setTemperature(temperature);  // Set temperature for compensation
    Tds.update();  // Retrieve value from sensor
    return Tds.getTdsValue();  // Return TDS value
}

/**
 * Get humidity from DHT11 sensor in percent.
 * 
 * @return Humidity in %.
*/
float getHumidity()
{
    Dht.read(dhtPin); // Retreive value
    return (float)Dht.humidity; // Return humidity
}

/**
 * Get temperature from DHT11 sensor in degrees celcius.
 * 
 * @return Air temperature in °C.
*/
float getTemperature()
{
    Dht.read(dhtPin); // Retreive value
    return (float)Dht.temperature; // Return temperature
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