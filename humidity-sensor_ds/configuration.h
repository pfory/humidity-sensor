#ifndef CONFIGURATION_H
#define CONFIGURATION_H

//SW name & version
#define     VERSION                      "0.5"
#define     SW_NAME                      "Vlhkomer"

#define AUTOCONNECTNAME   "vlhkomer"
#define AUTOCONNECTPWD    "password"

//#define timers
#define verbose

/*
--------------------------------------------------------------------------------------------------------------------------

Version history:

--------------------------------------------------------------------------------------------------------------------------
HW
ESP8266-201
sensor Si7021
*/

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <Ticker.h>
#include "Sender.h"
#include <Wire.h>
#include <SI7021.h>

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

static const char* const      mqtt_server                    = "192.168.1.56";
static const uint16_t         mqtt_port                      = 1883;
static const char* const      mqtt_username                  = "datel";
static const char* const      mqtt_key                       = "hanka12";
static const char* const      mqtt_base                      = "/home/sklenik/esp10/";
static const char* const      static_ip                      = "192.168.1.155";
static const char* const      static_gw                      = "192.168.1.1";
static const char* const      static_sn                      = "255.255.255.0";

#define SDA 12 // D6 - GPI12 on ESP-201 module
#define SCL 14 // D5 - GPI14 on ESP-201 module

#define TEMPERATURE_DIVIDOR         100
#define MILIVOLT_TO_VOLT            1000.0
#define MILISECOND_TO_SECOND        1000.0
#define MICROSECOND                 1000000


#define SLEEPDELAYINSECONDS         3600 //1 hod


#endif
