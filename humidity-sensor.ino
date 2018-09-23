/*
Vlhomer ESP8266-201
*/

#include <SI7021.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
//#include <ESP8266WebServer.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <WiFiManager.h> 

//for LED status
#include <Ticker.h>

unsigned long       start=0;
unsigned long       bootTime;

Ticker ticker;

void tick()
{
  //toggle state
  int state = digitalRead(BUILTIN_LED);  // get the current state of GPIO1 pin
  digitalWrite(BUILTIN_LED, !state);     // set pin to the opposite state
}
  
//#define PORADI1
#define PORADI2
  
#define verbose
#ifdef verbose
 #define DEBUG_PRINT(x)         Serial.print (x)
 #define DEBUG_PRINTDEC(x)      Serial.print (x, DEC)
 #define DEBUG_PRINTLN(x)       Serial.println (x)
 #define DEBUG_PRINTF(x, y)     Serial.printf (x, y)
 #define PORTSPEED 115200
#else
 #define DEBUG_PRINT(x)
 #define DEBUG_PRINTDEC(x)
 #define DEBUG_PRINTLN(x)
 #define DEBUG_PRINTF(x, y)
#endif 
  

#define AIO_SERVER      "192.168.1.56"
//#define AIO_SERVER      "178.77.238.20"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "datel"
#define AIO_KEY         "hanka12"

WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/
#ifdef PORADI1
#define MQTTBASE "/home/Vlhkomer/1/"
#endif
#ifdef PORADI2
#define MQTTBASE "/home/Vlhkomer/2/"
#endif
Adafruit_MQTT_Publish _temperature             = Adafruit_MQTT_Publish(&mqtt, MQTTBASE "Temperature");
Adafruit_MQTT_Publish _humidity                = Adafruit_MQTT_Publish(&mqtt, MQTTBASE "Humidity");
Adafruit_MQTT_Publish _bootTime                = Adafruit_MQTT_Publish(&mqtt, MQTTBASE "bootTime");
Adafruit_MQTT_Publish _voltage                 = Adafruit_MQTT_Publish(&mqtt, MQTTBASE "Voltage");
Adafruit_MQTT_Publish _versionSW               = Adafruit_MQTT_Publish(&mqtt, MQTTBASE "VersionSW");

#ifdef PORADI1
IPAddress _ip           = IPAddress(192, 168, 1, 111);
#endif
#ifdef PORADI2
IPAddress _ip           = IPAddress(192, 168, 1, 112);
#endif
IPAddress _gw           = IPAddress(192, 168, 1, 1);
IPAddress _sn           = IPAddress(255, 255, 255, 0);


void MQTT_connect(void);

extern "C" {
  #include "user_interface.h"
}

volatile float temperature;
volatile float humidity;
float voltage;

SI7021 sensor;

unsigned int sleepDelayInSeconds = 600; //10 min

#define TEMPERATURE_DIVIDOR 100
#define MILIVOLT_TO_VOLT 1000.0
#define MILISECOND_TO_SECOND 1000.0
#define MICROSECOND 1000000


#define SDA 12 // D6 - GPI12 on ESP-201 module
#define SCL 14 // D5 - GPI14 on ESP-201 module

//gets called when WiFiManager enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
  ticker.attach(0.2, tick);
}

ADC_MODE(ADC_VCC);

float versionSW                   = 0.2;
#ifdef PORADI1
String versionSWString            = "Vlhomer#1 v";
#endif
#ifdef PORADI2
String versionSWString            = "Vlhomer#2 v";
#endif
uint32_t heartBeat                = 0;

void setup() {
  start = millis();
#ifdef verbose
  Serial.begin(PORTSPEED);
#endif
  DEBUG_PRINTLN();
  DEBUG_PRINT(versionSWString);
  DEBUG_PRINTLN(versionSW);
  DEBUG_PRINT("START:");
  DEBUG_PRINTLN(start);
  //set led pin as output
  pinMode(BUILTIN_LED, OUTPUT);
  // start ticker with 0.5 because we start in AP mode and try to connect
  ticker.attach(0.6, tick);
  
  DEBUG_PRINTLN(ESP.getResetReason());
  if (ESP.getResetReason()=="Software/System restart") {
    heartBeat=1;
  } else if (ESP.getResetReason()=="Power on") {
    heartBeat=2;
  } else if (ESP.getResetReason()=="External System") {
    heartBeat=3;
  } else if (ESP.getResetReason()=="Hardware Watchdog") {
    heartBeat=4;
  } else if (ESP.getResetReason()=="Exception") {
    heartBeat=5;
  } else if (ESP.getResetReason()=="Software Watchdog") {
    heartBeat=6;
  } else if (ESP.getResetReason()=="Deep-Sleep Wake") {
    heartBeat=7;
  }

  //WiFiManager
  WiFiManager wifiManager;
  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setSTAStaticIPConfig(_ip, _gw, _sn);
  wifiManager.setAPCallback(configModeCallback);
  
  #ifdef PORADI1
  if (!wifiManager.autoConnect("Vlhkomer1", "password")) {
  #endif
  #ifdef PORADI2
  if (!wifiManager.autoConnect("Vlhkomer2", "password")) {
  #endif
    DEBUG_PRINTLN("failed to connect, we should reset as see if it connects");
    delay(3000);
    ESP.reset();
    delay(5000);
  }

	DEBUG_PRINTLN("");
	DEBUG_PRINT("Connected to ");
	DEBUG_PRINT("IP address: ");
	DEBUG_PRINTLN(WiFi.localIP());
  
  sensor.begin(SDA,SCL);
  
  ticker.detach();
  //keep LED on
  digitalWrite(BUILTIN_LED, HIGH);
}

void loop() {
  MQTT_connect();

  DEBUG_PRINT("Meassurement....");
  temperature=sensor.getCelsiusHundredths();
  humidity=sensor.getHumidityPercent();
  DEBUG_PRINTLN("DONE");
  DEBUG_PRINT("Temperature:");
  DEBUG_PRINT(temperature/TEMPERATURE_DIVIDOR);
  DEBUG_PRINTLN("C");
  DEBUG_PRINT("Humidity:");
  DEBUG_PRINT(humidity);
  DEBUG_PRINTLN("%");
  voltage = (float)ESP.getVcc()/MILIVOLT_TO_VOLT;
  DEBUG_PRINT("Voltage:");
  DEBUG_PRINT(voltage);
  DEBUG_PRINTLN("V");
  DEBUG_PRINTLN();

  if (! _temperature.publish(temperature/TEMPERATURE_DIVIDOR)) {
    DEBUG_PRINTLN("Temperature failed");
  } else {
    DEBUG_PRINTLN("Temperature OK!");
  }  
  if (! _humidity.publish(humidity)) {
    DEBUG_PRINTLN("Humidity failed");
  } else {
    DEBUG_PRINTLN("Humidity OK!");
  }  
  if (! _voltage.publish(voltage)) {
    DEBUG_PRINTLN("Voltage failed");
  } else {
    DEBUG_PRINTLN("Voltage OK!");
  }  
  if (! _versionSW.publish(versionSW)) {
    DEBUG_PRINTLN("Version SW failed");
  } else {
    DEBUG_PRINTLN("Version SW OK!");
  }  
  
  //boot time
  bootTime = millis()-start;
  DEBUG_PRINT("END:");
  DEBUG_PRINTLN(millis());
  if (! _bootTime.publish((float)(bootTime/MILISECOND_TO_SECOND))) {
    DEBUG_PRINTLN("BootTime failed");
  } else {
    DEBUG_PRINTLN("BootTime OK!");
  }  
    
  DEBUG_PRINT("Start duration:");
  DEBUG_PRINT(bootTime);
  DEBUG_PRINTLN(" ms.");
  DEBUG_PRINTLN("Entering deep sleep mode.");
  DEBUG_PRINT("Wake in ");
  DEBUG_PRINT(sleepDelayInSeconds);
  DEBUG_PRINTLN(" s.");
  delay(1000);
  ESP.deepSleep(sleepDelayInSeconds * MICROSECOND, WAKE_RF_DEFAULT);
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  DEBUG_PRINT("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    DEBUG_PRINTLN(mqtt.connectErrorString(ret));
    DEBUG_PRINTLN("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  DEBUG_PRINTLN("MQTT Connected!");
}