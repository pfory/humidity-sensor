//Vlhkomer napajeny bateriemi s uspavanim

#include "Configuration.h"


volatile float temperature;
volatile float humidity;

SI7021 sensor;

WiFiClient espClient;
PubSubClient client(espClient);


ADC_MODE(ADC_VCC);


/////////////////////////////////////////////   S  E  T  U  P   ////////////////////////////////////
void setup() {
  SERIAL_BEGIN;
  //keep LED on
  digitalWrite(BUILTIN_LED, LOW);

  DEBUG_PRINT(F(SW_NAME));
  DEBUG_PRINT(F(" "));
  DEBUG_PRINTLN(F(VERSION));

  client.setServer(mqtt_server, mqtt_port);

  WiFiManager wifiManager;

  IPAddress _ip,_gw,_sn;
  _ip.fromString(static_ip);
  _gw.fromString(static_gw);
  _sn.fromString(static_sn);

  wifiManager.setSTAStaticIPConfig(_ip, _gw, _sn);
  
  DEBUG_PRINTLN(_ip);
  DEBUG_PRINTLN(_gw);
  DEBUG_PRINTLN(_sn);

  
  if (!wifiManager.autoConnect(AUTOCONNECTNAME, AUTOCONNECTPWD)) { 
    DEBUG_PRINTLN("failed to connect and hit timeout");
    ESP.deepSleep(SLEEPDELAYINSECONDS * MICROSECOND, WAKE_RF_DEFAULT);
  } 

 
  if (client.connect(mqtt_base, mqtt_username, mqtt_key)) { // connect will return 0 for connected
 
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
    DEBUG_PRINT("Voltage:");
    DEBUG_PRINT((float)ESP.getVcc()/MILIVOLT_TO_VOLT);
    DEBUG_PRINTLN("V");
    DEBUG_PRINTLN();

    //publish meassurements
    DEBUG_PRINTLN(F(" - I am sending data..."));

    SenderClass sender;
    sender.add("VersionSW", VERSION);
    sender.add("Napeti",  ESP.getVcc());
    sender.add("RSSI", WiFi.RSSI());
    sender.add("temperature", temperature/TEMPERATURE_DIVIDOR);
    sender.add("humidity", humidity);
    sender.add("bootTime", (float)(millis()/MILISECOND_TO_SECOND));
    
    sender.sendMQTT(mqtt_server, mqtt_port, mqtt_username, mqtt_key, mqtt_base);

    DEBUG_PRINT("Start duration:");
    DEBUG_PRINT(millis());
    DEBUG_PRINTLN(" ms.");
    DEBUG_PRINTLN("Entering deep sleep mode.");
    DEBUG_PRINT("Wake in ");
    DEBUG_PRINT(SLEEPDELAYINSECONDS);
    DEBUG_PRINTLN(" s.");
  } else {
    DEBUG_PRINTLN("MQQT connect failed. ");
    DEBUG_PRINTLN("Entering deep sleep mode.");
    DEBUG_PRINT("Wake in ");
    DEBUG_PRINT(SLEEPDELAYINSECONDS);
    DEBUG_PRINTLN(" s.");
  }
  //keep LED off
  digitalWrite(BUILTIN_LED, HIGH);

  delay(1000);
  ESP.deepSleep(SLEEPDELAYINSECONDS * MICROSECOND, WAKE_RF_DEFAULT);
 
}

void loop() {}