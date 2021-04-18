/***************************************************


 ***************************************************
  © Copyright : see README.md
****************************************************/

/* PINOUT */
#define dhtPin D3
#define analogPin A0
#define muxOutput = A0;
const int muxControls[3] = {D0, D1, D2};
/* END PINOUT */

/* Multiplexer */
#define nbAnalogique 4
const int muxChannels[5][3] = {
    {0, 0, 0}, // Channel 0
    {1, 0, 0}, // Channel 1
    {0, 1, 0}, // Channel 2
    {1, 1, 0}, // Channel 3
    {0, 0, 1}  // Channel 4
};
float (*func[])(){
    getMoisture1,
    getMoisture2,
    getLDR1,
    getLDR2};
float data[4];
/* END Multiplexer */

/* Blynk Setup*/
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
char auth[] = "";
char ssid[] = "";
char pass[] = "";
BlynkTimer timer;
const int virtualPin[nbAnalogique] = {V0, V1, V2, V3};
/* END Blynk */

/* DHT11 sensor */
#include "dht11.h"
dht11 Dht;
float humidity;
float temperature;
/* END DHT11 */

/* TDS sensor */
#define tdsCoef 1
/* END TDS */

/* Moisture sensors */
#define MOISTURE_WATER 487
#define MOISTURE_AIR 250
float moisture1;
float moisture2;
/* END Moisture */

/* LDR */
#define LDR_MIN 0
#define LDR_MAX 1000
float ldr1;
float ldr2;
/* END LDR */

void setup()
{
    Serial.begin(9600);

    /* Blynk Setup */
    Blynk.begin(auth, ssid, pass, "192.168.1.100", 8080);
    timer.setInterval(1000L, getData);
    /* END Blynk */

    /* Mux setup */
    for (int i = 0; i < 3; i++)
    {
        pinMode(muxControls[i], OUTPUT);
    }
    /* END Mux */
}

void loop()
{
    Blynk.run();
    timer.run();
}

/**
    Point d'entrée de Blynk.

    @def Écrit chaque donnée sur les pins virtuelles
    de blynk.
*/
void getData()
{
    for (int i = 0; i < nbAnalogique; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            digitalWrite(muxControls[j], muxChannels[i][j]);
        }

        data[i] = (func[i])();
        Blynk.virtualWrite(virtualPin[i], data[i]);
    }

    humidity = getHum();
    temperature = getTemp();
    Blynk.virtualWrite(V4, humidity);
    Blynk.virtualWrite(V5, temperature);

    for (int j = 0; j < 3; j++)
    {
        digitalWrite(muxControls[j], muxChannels[nbAnalogique][j]);
    }
    float tds = getTds(temperature);
    Blynk.virtualWrite(V6, tds);
}

/**
    Get humidity from DHT11 sensor.

    @return Humidity from DHT11 in %.
*/
float getTemp()
{
    Dht.read(dhtPin);              // Retreive value
    return (float)Dht.temperature; // Return humidity
}

/**
    Get temperature from DHT11 sensor.

    @return Temperature from DHT11 in °C.
*/
float getHum()
{
    Dht.read(dhtPin);           // Retreive value
    return (float)Dht.humidity; // Return humidity
}

/**
    Get value from TDS sensor in ppm.

    @param temp Temperature at which measure was taken for compensation.
    @return Tds value of sensor in ppm.
*/
float getTds(float temp)
{
    float value;
    for (int i = 0; i < 100; i++) // Retrieving a hundred value for accuracy
    {
        value += analogRead(analogPin);
        delay(1);
    }

    float rawEc = value * 5 / 1024.0;            // Convert value to current
    float tempCoef = 1.0 + 0.02 * (temp - 25.0); // Find temperature compensation value

    float ec = (rawEc / tempCoef) * tdsCoef;                                  // Add temperature compensation and calibration coef
    float tds = (133.42 * pow(ec, 3) - 255.86 * ec * ec + 857.39 * ec) * 0.5; // Convert voltage to tds

    return tds;
}

/**
    Get soil moisture from sensor 1 in percent.

    @return Soil moisture from sensor 1 in %.
*/
float getMoisture1()
{
    for (int i = 0; i < 100; i++) // Retrieving a hundred value for accuracy
    {
        moisture1 += analogRead(analogPin);
        delay(1);
    }
    moisture1 = moisture1 / 100.0; // Get average value
    // Map value between 0 and 100 to get a percentage
    moisture1 = map(moisture1, MOISTURE_AIR, MOISTURE_WATER, 0, 100);
    return constrain(moisture1, 0, 100); // Constrain value inside [0, 100]
}

/**
    Get soil moisture from sensor 2 in percent.

    @return Soil moisture from sensor 2 in %.
*/
float getMoisture2()
{
    for (int i = 0; i < 100; i++) // Retrieving a hundred value for accuracy
    {
        moisture2 += analogRead(analogPin);
        delay(1);
    }
    moisture2 = moisture2 / 100.0; // Get average value
    // Map value between 0 and 100 to get a percentage
    moisture2 = map(moisture2, MOISTURE_AIR, MOISTURE_WATER, 0, 100);
    return constrain(moisture2, 0, 100); // Constrain value inside [0, 100]
}

/**
    Get light intensity from sensor 1 in percent.

    @return Light intensity from sensor 1 in %.
*/
float getLDR1()
{
    for (int i = 0; i < 100; i++)
    {
        ldr1 += analogRead(analogPin);
        delay(1);
    }
    ldr1 = ldr1 / 100.0;
    ldr1 = map(ldr1, LDR_MIN, LDR_MAX, 0, 100);
    return constrain(ldr1, 0, 100);
}

/**
    Get light intensity from sensor 2 in percent.

    @return Light intensity from sensor 2 in %.
*/
float getLDR2()
{
    for (int i = 0; i < 100; i++)
    {
        ldr2 += analogRead(analogPin);
        delay(1);
    }
    ldr2 = ldr2 / 100.0;
    ldr2 = map(ldr2, LDR_MIN, LDR_MAX, 0, 100);
    return constrain(ldr2, 0, 100);
}
