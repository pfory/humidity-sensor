unsigned long start=millis();

#include <ESP8266WiFi.h>
#include <Wire.h>
#include <SI7021.h>
#include <Xively.h>
#include <Ethernet.h>

const char* ssid = "Fusal2";
const char* password = "1860918609";

#define SDA 12 // GPI12 on ESP-201 module
#define SCL 14 // GPI14 on ESP-201 module
#define LED 1  // GPIO5 on ESP-201 module

SI7021 sensor;

char HumidityID[]         = "Humidity";
char TemperatureID[]      = "Temperature";
char VoltageID[]          = "Voltage";
char BootTimeID[]         = "BootTime";


XivelyDatastream datastreamsHumidity[] = {
  XivelyDatastream(HumidityID,        strlen(HumidityID),       DATASTREAM_FLOAT),
  XivelyDatastream(TemperatureID,     strlen(TemperatureID),    DATASTREAM_FLOAT),
  XivelyDatastream(VoltageID,         strlen(VoltageID),        DATASTREAM_FLOAT),
  XivelyDatastream(BootTimeID,        strlen(BootTimeID),       DATASTREAM_FLOAT)
};

char xivelyKey[]                = "QylzihyDT20L1A69HqfAgU6ZnN3DepSjN7B3mb6zpIbfK8Gr";
#define xivelyFeedHumidity         395999474

XivelyFeed feedHumidity(xivelyFeedHumidity,         datastreamsHumidity,       4);

WiFiClient client;
XivelyClient xivelyclientHumidity(client);

#define SLEEP_DELAY_IN_SECONDS 300 
#define ONECYCLE_IN_MS 1000 
#define TIMEOUTFORWIFI_IN_MS 10000 
#define MICROSECOND 1000000
#define PORTSPEED 115200
#define TEMPERATURE_DIVIDOR 100.0
#define MILLIS_TO_SEC 1000.0
#defirn MILIVOLT_TO_VOLT 1000.0

ADC_MODE(ADC_VCC);

#define staticIP
#ifdef staticIP
IPAddress ip(192,168,0,165);  //Node static IP
IPAddress gateway(192,168,0,1);
IPAddress subnet(255,255,255,0);
#endif
unsigned long bootTime = 0;

void setup() {
  bootTime = millis();
  Serial.begin(PORTSPEED);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  pinMode(5, OUTPUT);
  
#ifdef staticIP
  WiFi.config(ip, gateway, subnet);
#endif
  WiFi.begin(ssid, password);
  
  byte cyklu=0;  
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(5, LOW);
    cyklu++;
    delay(ONECYCLE_IN_MS);
    digitalWrite(5, HIGH);
    Serial.print(".");
    if (cyklu*ONECYCLE_IN_MS>TIMEOUTFORWIFI_IN_MS) {
      Serial.println();
      Serial.print("Wifi unavailable, go to deepsleep for 5 minutes");
      ESP.deepSleep(SLEEP_DELAY_IN_SECONDS * MICROSECOND, WAKE_RF_DEFAULT);
    }
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Gateway: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("Subnet: ");
  Serial.println(WiFi.subnetMask());
  sensor.begin(SDA,SCL);
}


void loop() {
  digitalWrite(5, LOW);
  Serial.println("Hlavni smycka...");
  float temperature=sensor.getCelsiusHundredths();
  float humidity=sensor.getHumidityPercent();
  Serial.print("Temperature:");
  Serial.print(temperature/TEMPERATURE_DIVIDOR);
  Serial.println("C");
  Serial.print("Humidity:");
  Serial.print(humidity);
  Serial.println("%");
  float voltage = (float)ESP.getVcc()/MILIVOLT_TO_VOLT;
  Serial.print("Voltage:");
  Serial.print(voltage);
  Serial.println("V");
  if (!client.connected()) {
    datastreamsHumidity[0].setFloat(humidity);
    datastreamsHumidity[1].setFloat(temperature/TEMPERATURE_DIVIDOR);
    datastreamsHumidity[2].setFloat(voltage);
    datastreamsHumidity[3].setFloat((float)(millis() - bootTime)/MILLIS_TO_SEC);
    Serial.println(F("Uploading data to Xively"));
    int ret = xivelyclientHumidity.put(feedHumidity, xivelyKey);
    
    Serial.print(F("xivelyclientHumidity.put returned "));
    Serial.println(ret);
  }
  Serial.print("Start duration:");
  Serial.print(millis()-start);
  Serial.println(" ms.");
  Serial.println("Entering deep sleep mode");
  ESP.deepSleep(SLEEP_DELAY_IN_SECONDS * MICROSECOND, WAKE_RF_DEFAULT);
  //delay(60000);
}

