/***************************************************


 ***************************************************
 © Copyright : see README.md
****************************************************/

/* PINOUT */

#define tdsPin A1
/* END PINOUT */


/* TDS Sensor */
#include "GravityTDS.h"
#include <EEPROM.h>

GravityTDS Tds;
/* END TDS Sensor */

void setup()
{
    Serial.begin(9600);

    /* TDS Sensor */
    Tds.setPin(tdsPin); // Pin of sensor
    Tds.setAref(5.0) // Reference voltage on ADC
    Tds.setAdcRange(1024) // 10 bit ADC for Uno and Nano
    Tds.begin()
    /* END TDS Sensor */
}

void loop()
{
    
}

/**
 * Get value from TDS Sensor in ppm.
 * 
 * @param temperature Temperature at which measure was taken.
 * @return Value of sensor in ppm.
*/
void tdsValue(float temperature)
{
    Tds.setTemperature(temperature);  // Set temperature for compensation
    Tds.update();  // Retrieve value from sensor
    return Tds.getTdsValue();  // Return TDS value
}