#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <ESP8266WiFi.h>              //https://github.com/esp8266/Arduino
#include <WiFiManager.h>              //https://github.com/tzapu/WiFiManager
#include "Sender.h"
#include <DoubleResetDetector.h>      //https://github.com/khoih-prog/ESP_DoubleResetDetector
#include <Ticker.h>
#include "SI7021.h"                   //https://github.com/LowPowerLab/SI7021.git


//SW name & version
#define     VERSION                      "0.20"
#define     SW_NAME                      "VlhkomerSklenik"

//#define timers
#define verbose
//#define time
#ifdef time
#include <TimeLib.h>
#include <Timezone.h>
#endif

#define AUTOCONNECTPWD    "password"

//#define ota
#ifdef ota
#include <ArduinoOTA.h>
#define HOSTNAMEOTA       SW_NAME VERSION
#define AUTOCONNECTNAME   HOSTNAMEOTA
#else
#define AUTOCONNECTNAME   SW_NAME VERSION
#endif

/*
--------------------------------------------------------------------------------------------------------------------------

Version history:

--------------------------------------------------------------------------------------------------------------------------
HW
ESP8266 Wemos D1
HC-SR04
*/


#define verbose
#ifdef verbose
  #define DEBUG_PRINT(x)         Serial.print (x)
  #define DEBUG_PRINTDEC(x)      Serial.print (x, DEC)
  #define DEBUG_PRINTLN(x)       Serial.println (x)
  #define DEBUG_PRINTF(x, y)     Serial.printf (x, y)
  #define DEBUG_PRINTHEX(x)      Serial.print (x, HEX)
  #define PORTSPEED 115200
  #define SERIAL_BEGIN           Serial.begin(PORTSPEED);
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTDEC(x)
  #define DEBUG_PRINTLN(x)
  #define DEBUG_PRINTF(x, y)
#endif 


// Number of seconds after reset during which a
// subseqent reset will be considered a double reset.
#define DRD_TIMEOUT 2
// RTC Memory Address for the DoubleResetDetector to use
#define DRD_ADDRESS 0

#define CONFIG_PORTAL_TIMEOUT 60 //jak dlouho zustane v rezimu AP nez se cip resetuje
#define CONNECT_TIMEOUT 120 //jak dlouho se ceka na spojeni nez se aktivuje config portal

static const char* const      mqtt_server                    = "192.168.1.56";
static const uint16_t         mqtt_port                      = 1883;
static const char* const      mqtt_username                  = "datel";
static const char* const      mqtt_key                       = "hanka12";
static const char* const      mqtt_base                      = "/home/sklenik/esp10";


int                           voltage                        = 0; //voltage in mV
uint32_t                      lastRun                        = 0;
int                           temperature                    = 0;
uint8_t                       humidity                       = 0;
uint32_t                      deepSleepMicros                = 0;


#define TEMPERATURE_DIVIDOR                 100


#define SCL                                 14 //                       GPIO14
#define SDA                                 12 //                       GPIO12
//#define BUILTIN_LED                         1


#endif
