#include <ESP8266WiFi.h>
#include <Wire.h>
#include <SI7021.h>
#include <Xively.h>
#include <Ethernet.h>

const char* ssid = "Datlovo";
const char* password = "Nu6kMABmseYwbCoJ7LyG";

#define SDA 0 // GPIO0 on ESP-01 module
#define SCL 2 // GPIO2 on ESP-01 module

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
  
  WiFi.begin(ssid, password);
    
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  sensor.begin(SDA,SCL);
}


void loop() {
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
 
  Serial.println("Entering deep sleep mode");
  ESP.deepSleep(SLEEP_DELAY_IN_SECONDS * 1000000, WAKE_RF_DEFAULT);
  //delay(60000);
}

