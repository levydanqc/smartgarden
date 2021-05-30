/***************************************************

 * Smart Garden Monitoring


 ** https://gitlab.com/levydanqc/smartgarden 

 ****************** Configuration ******************
Line 25:    SSID "PASSWORD_FOR_THE_ARDUINO"             -->     Name of your network
Line 26:    PASSW "YOUR_WIFI_PASSWORD":                 -->     Password of your network
Line 27:    ArduinoOTAHostname "NAME_FOR_THE_ARDUINO":  -->     Hostname for the OTA PORT setting
Line 28:    ArduinoOTAPASSW "PASSWORD_FOR_THE_ARDUINO": -->     Password for the OTA upload
Line 35:    char auth[] = "YOUR_BLYNK_TOKEN":           -->     Authentication token of the Blynk project

 ****************************************************
  © Copyright : see README.md
****************************************************/
/* 
 * Uncomment next line for debugging connection issues.
 * It will print to serial information about WIFI connection
 * and local server. 
*/
#define BLYNK_PRINT Serial

// ESP32
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <TimeLib.h>
#include <WidgetRTC.h>

// For OTA upload
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include "dht11.h"

/* OTA Setup */
#ifndef SSID
#define SSID "YOUR_WIFI_SSID"                      // Enter the name of your network as it is in you WIFI settings
#define PASSW "YOUR_WIFI_PASSWORD"                 // Enter the password of your network
#define ArduinoOTAHostname "NAME_FOR_THE_ARDUINO"  // Enter the hostname for the board (Optional)
#define ArduinoOTAPASSW "PASSWORD_FOR_THE_ARDUINO" // Enter the password for the board (Optional)
#endif
const char *OtaSSID = SSID;
const char *OtaPassw = PASSW;
/* END OTA */

/* Blynk Setup*/
char auth[] = "YOUR_BLYNK_TOKEN";
char ssid[] = SSID;
char pass[] = PASSW;
BlynkTimer timer;
WidgetTerminal terminal(V13);
WidgetRTC rtc;
byte sliderValue;
float inputValue;
/* END Blynk */

/* Pinout */
#define valvePin 33
#define moisturePin1 32
#define moisturePin2 34
#define ldrPin 35
#define flowPin 39
#define dhtPin 36
/* END Pinout */

/* Function */
const byte inputs = 5;
float (*func[])(){
    getMoisture1,
    getMoisture2,
    getLDR,
    getFlow,
    getTemp,
    getHum};
/* END Function */

/* Valve */
unsigned long startValve;
int duration;
int valveIsActivated;
/* END Valve */

/* Flow Sensor */
volatile byte flowCounter;
float flowQ = 7.5;
float startFlow;
float flowRate;
/* END Flow */

/* DHT11 Sensor */
dht11 Dht;
float humidity;
float temperature;
/* END DHT11 */

/* Moisture Sensor */
#define MOISTURE_WATER 300
#define MOISTURE_AIR 670
float moisture1;
float moisture2;
/* END Moisture */

/* LDR */
#define LDR_MIN 0
#define LDR_MAX 1000
float ldr1;
/* END LDR */

void setup()
{
    Serial.begin(9600);

    /* OTA Setup */
    WiFi.mode(WIFI_STA);
    WiFi.begin(OtaSSID, OtaPassw);
    while (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
        Serial.println("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
    }
    ArduinoOTA.setHostname(ArduinoOTAHostname);
    ArduinoOTA.setPassword(ArduinoOTAPASSW);
    /* END OTA */

    /* Blynk Setup */
    Blynk.begin(auth, ssid, pass, "192.168.1.100", 8080);
    timer.setInterval(1000L, sendData);
    terminal.println("Connecté!");
    /* END Blynk */

    /* OTA Setup */
    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
        {
            type = "sketch";
        }
        else
        { // U_FS
            type = "filesystem";
        }

        // NOTE: if updating FS this would be the place to unmount FS using FS.end()
        Serial.println("Start updating " + type);
    });
    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR)
        {
            Serial.println("Auth Failed");
        }
        else if (error == OTA_BEGIN_ERROR)
        {
            Serial.println("Begin Failed");
        }
        else if (error == OTA_CONNECT_ERROR)
        {
            Serial.println("Connect Failed");
        }
        else if (error == OTA_RECEIVE_ERROR)
        {
            Serial.println("Receive Failed");
        }
        else if (error == OTA_END_ERROR)
        {
            Serial.println("End Failed");
        }
    });
    ArduinoOTA.begin();
    terminal.print("IP address: ");
    terminal.println(WiFi.localIP());
    /* END OTA */

    /* Flow Sensor Setup */
    attachInterrupt(digitalPinToInterrupt(flowPin), FlowIncrement, RISING); // Create interrupts
    noInterrupts();                                                         // Disable interrupts at beginning
    /* END Flow */

    terminal.flush();
}

void loop()
{
    ArduinoOTA.handle();
    Blynk.run();
    timer.run();
}

/**
    Point d'entrée de Blynk.

    Écrit chaque donnée sur les pins virtuelles
    de blynk.
*/
void sendData()
{
    // Get data from sensors
    for (int i = 0; i <= inputs; i++)
    {
        Blynk.virtualWrite(i, (func[i])());
    }

    // Check valve opening duration
    if (valveIsActivated && duration != -1 && now() >= (startValve + duration))
    {
        valveIsActivated = false;
        terminal.println("Closed valve after duration.");
        terminal.flush();
        toggleValve();
    }
    // Check flow rate to verify that the valve was opened
    if (valveIsActivated && flowRate < 5)
    {
        //Blynk.notify("Erreur d'ouverture de la valve. Débit trop faible.");
    }

    // Notify if valve opened for a certain time (like if user inputs is too big)
    int openedDuration = now() - startValve;
    if (valveIsActivated && openedDuration > 0 && (openedDuration % (60 * 60) == 0)) // Every hour
    {
        //Blynk.notify("La valve est ouverte depuis " + String(openedDuration / 60) + " minutes.");
    }
}

/**
    Get soil moisture from sensor 1 in percent.

    @return Soil moisture from sensor 1 in %.
*/
float getMoisture1()
{
    for (int i = 0; i < 100; i++) // Retrieving a hundred value for accuracy
    {
        moisture1 += analogRead(moisturePin1);
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
        moisture2 += analogRead(moisturePin2);
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
float getLDR()
{
    for (int i = 0; i < 100; i++)
    {
        ldr1 += analogRead(ldrPin);
        delay(1);
    }
    ldr1 = ldr1 / 100.0;
    ldr1 = map(ldr1, LDR_MIN, LDR_MAX, 0, 100);
    return constrain(ldr1, 0, 100);
}

/**
   Increment the number of loop of the magnet by one.
*/
ICACHE_RAM_ATTR void FlowIncrement()
{
    flowCounter++;
}

/**
   Get flow rate from sensor in L/min.

    @return Flow rate in L/min or 0 if valve isn't opened yet.
*/
float getFlow()
{
    if (valveIsActivated)
    {
        if (startFlow == 0)
        {
            startFlow = now();
            interrupts();
            return flowRate * valveIsActivated; // Return 0 if valve is closed, flow rate otherwise
        }
        else
        {
            noInterrupts();
            flowRate = ((1000.0 / (now() - startFlow)) * flowCounter) / flowQ; // Flow rate in L/min
            startFlow = 0;
            return flowRate * valveIsActivated; // Return 0 if valve is closed, flow rate otherwise
        }
    }
    return 0;
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
    Togge between state (open/close) of valve and print it to terminal.
*/
void toggleValve()
{
    String state;
    switch (valveIsActivated)
    {
    case 0:
        state = "closed";
        break;
    case 1:
        state = "opened";
        break;
    }

    terminal.println("Valve " + state + " @ " + getTime());
    terminal.flush();

    digitalWrite(valvePin, valveIsActivated);  // Open/Close valve
    Blynk.virtualWrite(V14, valveIsActivated); // Blynk chart of valve state
    if (duration != -1)
    {
        Blynk.virtualWrite(V10, valveIsActivated);
    }
    else
    {
        Blynk.virtualWrite(V10, 0);
    }
}

/**
   Return current date and time inside a string.
*/
String getTime()
{
    String date = String(day()) + "/" + month() + "/" + year();
    String time = String(hour()) + ":" + minute() + ":" + second();
    return date + ' ' + time;
}

/* Blynk functions  */
/**
    Run when connected to Blynk.
    Synchronize clock.
*/
BLYNK_CONNECTED()
{
    rtc.begin();
    for (byte i = 0; i < 13; i++)
    {
        Blynk.virtualWrite(i, 0);
    }
}

/**
    Run when changes on numeric input.
    Store input value for duration in hours.
*/
BLYNK_WRITE(V7) // Numeric Input
{
    inputValue = param.asFloat();
    int totalMinutes = (sliderValue % 60) + (inputValue - (int)inputValue) * 60;
    Blynk.virtualWrite(V11, ((int)inputValue + (sliderValue / 60) + (totalMinutes / 60)));
    Blynk.virtualWrite(V12, (totalMinutes % 60));
}

/**
    Run when changes on slider.
    Store slider value for duration in minutes.
*/
BLYNK_WRITE(V8) // Slider
{
    sliderValue = param.asInt();
    int totalMinutes = (sliderValue % 60) + (inputValue - (int)inputValue) * 60;
    Blynk.virtualWrite(V11, ((int)inputValue + (sliderValue / 60) + (totalMinutes / 60)));
    Blynk.virtualWrite(V12, (totalMinutes % 60));
}

/**
    Run when activation button is pressed.
    Activate/Deactivate valve.
*/
BLYNK_WRITE(V10) // Input Activation button
{
    if (duration != -1) // Timer not running
    {
        valveIsActivated = param.asInt();
        if (valveIsActivated) // Start valve
        {

            duration = (sliderValue + (int)(inputValue * 60)) * 60; // Total duration in seconds
            startValve = now();                                     // Get unix time (in seconds)
            terminal.println("Manually opened valve.");
            terminal.flush();
            toggleValve();
        }
        else
        {
            duration = 0;
            startValve = 0;
            terminal.println("Manually closed valve.");
            terminal.flush();
            toggleValve();
        }
    }
    else
    {
        Blynk.notify("Timer currently running.");
    }
}

/**
   Run when timer is activated.
   Adapt duration for timer and open/close valve.
*/
BLYNK_WRITE(V9) // Timer button
{
    if (startValve == 0) // Activation button not running
    {
        valveIsActivated = param.asInt();

        duration = 0 - valveIsActivated; // duration = -1 when running, 0 otherwise
        terminal.println("Timer toggled valve.");
        terminal.flush();
        toggleValve();
    }
    else
    {
        Blynk.notify("Manual button currently running.");
    }
}
