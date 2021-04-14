/***************************************************


 ***************************************************
 © Copyright : see README.md
****************************************************/

/* PINOUT */
#define dhtPin 5
#define tdsPin A1
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
    Dht.read(dhtPin);
    return (float)Dht.humidity;
}

/**
 * Get temperature from DHT11 sensor in degrees celcius.
 * 
 * @return Air temperature in °C.
*/
float getTemperature()
{
    Dht.read(dhtPin);
    return (float)Dht.temperature;
}