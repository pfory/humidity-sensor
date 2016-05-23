unsigned long start=millis();

#include <ESP8266WiFi.h>
#include <Wire.h>
#include <SI7021.h>
#include <Xively.h>
#include <Ethernet.h>

const char* ssid = "Datlovo";
const char* password = "Nu6kMABmseYwbCoJ7LyG";

#define SDA 12 // GPI12 on ESP-201 module
#define SCL 14 // GPI14 on ESP-201 module
#define LED 1  // GPIO5 on ESP-201 module

SI7021 sensor;

char HumidityID[]         = "Humidity";
char TemperatureID[]      = "Temperature";


XivelyDatastream datastreamsHumidity[] = {
  XivelyDatastream(HumidityID,        strlen(HumidityID),       DATASTREAM_FLOAT),
  XivelyDatastream(TemperatureID,     strlen(TemperatureID),    DATASTREAM_FLOAT)
};

char xivelyKey[]                = "VgoUxsEYlsN65fvTsBh8Sp9919cpMmv1cI2MoOzgHPnNADMa";
#define xivelyFeedHumidity         395999474

XivelyFeed feedHumidity(xivelyFeedHumidity,         datastreamsHumidity,       2);

WiFiClient client;
XivelyClient xivelyclientHumidity(client);

#define SLEEP_DELAY_IN_SECONDS 300

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);
  delay(1000);
  digitalWrite(5, HIGH);
  delay(1000);
  digitalWrite(5, LOW);
  delay(1000);
  
  WiFi.begin(ssid, password);
  
  byte cyklu=0;  
  while (WiFi.status() != WL_CONNECTED) {
    cyklu++;
    delay(500);
    Serial.print(".");
    if (cyklu*500>10000) {
      Serial.print("Wifi unavailable, go to deepsleep for 5 minutes");
      ESP.deepSleep(300 * 1000000, WAKE_RF_DEFAULT);
    }
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  sensor.begin(SDA,SCL);
}


void loop() {
  Serial.println("Hlavni smycka...");
  float temperature=sensor.getCelsiusHundredths();
  float humidity=sensor.getHumidityPercent();
  Serial.print("Temperature:");
  Serial.print(temperature/100);
  Serial.println("C");
  Serial.print("Humidity:");
  Serial.print(humidity);
  Serial.println("%");
  if (!client.connected()) {
    datastreamsHumidity[0].setFloat(humidity);
    datastreamsHumidity[1].setFloat(temperature/100.0);
    Serial.println(F("Uploading data to Xively"));
    int ret = xivelyclientHumidity.put(feedHumidity, xivelyKey);
    
    Serial.print(F("xivelyclientHumidity.put returned "));
    Serial.println(ret);
  }
  Serial.print("Start duration:");
  Serial.print(millis()-start);
  Serial.println(" ms.");
  Serial.println("Entering deep sleep mode");
  ESP.deepSleep(SLEEP_DELAY_IN_SECONDS * 1000000, WAKE_RF_DEFAULT);
  //delay(60000);
}

