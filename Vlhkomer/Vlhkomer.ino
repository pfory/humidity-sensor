unsigned long start=millis();

#include <ESP8266WiFi.h>
#include <SI7021.h>
#include <WiFiClient.h> 
#include <FS.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

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

ADC_MODE(ADC_VCC);

SI7021 sensor;

#define TEMPERATURE_DIVIDOR 100
#define MILIVOLT_TO_VOLT 1000.0
#define ONECYCLE_IN_MS 1000 
#define TIMEOUTFORWIFI_IN_MS 10000 
#define MICROSECOND 1000000
#define PORTSPEED 115200
#define MILLIS_TO_SEC 1000.0

#define SDA 12 // D6 - GPI12 on ESP-201 module
#define SCL 14 // D5 - GPI14 on ESP-201 module

#include <ESP8266WebServer.h>
ESP8266WebServer serverA(80);

unsigned long const sendDelay = 60000;
unsigned long lastSend = sendDelay;
unsigned long const dispDelay = 5000;
unsigned long lastDisp = dispDelay;
String sleepDelayInSeconds = "60"; 
String ssid;
String passw;
String ip;
String mask;
String gate;

//#define AIO_SERVER      "192.168.1.56"
#define AIO_SERVER      "178.77.268.20"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "datel"
#define AIO_KEY         "hanka12"

String brokerIP;
String brokerPort;
String brokerUser;
String brokerPassword;

WiFiClient client;

#define MQTTBASE "/holcik/Vlhkomer1/"

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish _temperature             = Adafruit_MQTT_Publish(&mqtt, MQTTBASE "Temperature");
Adafruit_MQTT_Publish _humidity                = Adafruit_MQTT_Publish(&mqtt, MQTTBASE "Humidity");
Adafruit_MQTT_Publish _bootTime                = Adafruit_MQTT_Publish(&mqtt, MQTTBASE "bootTime");
Adafruit_MQTT_Publish _voltage                 = Adafruit_MQTT_Publish(&mqtt, MQTTBASE "Voltage");
Adafruit_MQTT_Publish _versionSW               = Adafruit_MQTT_Publish(&mqtt, MQTTBASE "VersionSW");

#define MaxHeaderLength 1600
byte mode=1;
//D1 - prepinac 1 na GPIO5
//D2 - prepinac 2 na GPIO4
//byte mode=1;  //flash
//byte mode=2;  //setup   D1=0 D2=0
//byte mode=3;  //test    D1=1 D2=0
//byte mode=4;  //mereni  D1=1 D2=1

#include "FS.h"


String HttpHeader = String(MaxHeaderLength);
String HttpHeaderBak = String(MaxHeaderLength);

volatile float temperature;
volatile float humidity;
float voltage;

WiFiServer server(80);

float versionSW=1.1;
char versionSWString[] = "Vlhkoměr v"; //SW name & version

void handleRoot() {
	char temp[600];

	snprintf ( temp, 600,
      "<html>\
        <head>\
          <meta http-equiv='refresh' content='5'/>\
          <meta charset='UTF-8'>\
          <title>Vlhkomer</title>\
          <style>\
            body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
          </style>\
        </head>\
        <body>\
          <span style='font-weight:bold; font-size:20pt;'>Vlhkoměr</span><span style='font-style:italic;'> Verze FW:1.1 Topic base: %s</span><br/><br/>\
          <p>Teplota: %3d,%02d °C Vlhkost: %3d \%Rh Napětí: %2d,%02d V</p>\
        </body>\
      </html>",
     MQTTBASE,
		 (int)(temperature/TEMPERATURE_DIVIDOR), ((int)temperature)%TEMPERATURE_DIVIDOR, (int)humidity, (int)voltage, ((int)(voltage*100.0))%100
	);
	serverA.send ( 200, "text/html", temp );
}

int readConfig(void);
bool startsWith(const char *, const char *);
String getValue(String, char, int);



void setup() {
  Serial.begin(PORTSPEED);
  DEBUG_PRINT(versionSWString);
  DEBUG_PRINTLN(versionSW);
  //mode
  pinMode(5, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  int d1 = digitalRead(5);
  int d2 = digitalRead(4);
  
  if (d1==0 && d2==0) {
    mode=2;
  } else if (d1==1 && d2==0) {
    mode = 3;
  } else if (d1==1 && d2==1) {
    mode = 4;
  }
  DEBUG_PRINT("Mode:");
  DEBUG_PRINTLN(mode);

  if (mode==2) { //setup
    delay(1000);
    /* Set these to your desired credentials. */
    const char *ssid;
    const char *password;

    ssid = "ESPHum";
    password = "humidity007";

    DEBUG_PRINTLN();
    DEBUG_PRINT("Configuring access point...");
    /* You can remove the password parameter if you want the AP to be open. */
    WiFi.softAP(ssid, password);

    IPAddress myIP = WiFi.softAPIP();
    DEBUG_PRINT("AP IP address: ");
    DEBUG_PRINTLN(myIP);
    DEBUG_PRINT("SSID: ");
    DEBUG_PRINTLN(ssid);
    DEBUG_PRINT("Password: ");
    DEBUG_PRINTLN(password);
    server.begin();
    DEBUG_PRINTLN("HTTP server started");
    HttpHeader = "";
    
    readConfig();
    
  } else if (mode==3 || mode==4) { //test a mereni
    if (readConfig()>0) {
      DEBUG_PRINTLN("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      DEBUG_PRINTLN("!!!! Zařízení není nakonfigurované, končím !!!!!");
      DEBUG_PRINTLN("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      return;
    }
  
    char *pch;
    byte s[HttpHeaderBak.length()];
    HttpHeaderBak.getBytes(s, HttpHeaderBak.length());
    DEBUG_PRINTLN(HttpHeaderBak);
    pch = strtok((char*)s,"?&=");
    while (pch != NULL) {
     DEBUG_PRINTLN(pch);
    if (startsWith("ssid",pch)) {
      ssid=strchr(pch,'=')+1;
    }
    if (startsWith("passw",pch)) {
      passw=strchr(pch,'=')+1;
    }
    if (startsWith("ip",pch)) {
      ip=strchr(pch,'=')+1;
    }
    if (startsWith("mask",pch)) {
      mask=strchr(pch,'=')+1;
    }
    if (startsWith("gateway",pch)) {
      gate=strchr(pch,'=')+1;
    }

    // if (startsWith("brokerIP",pch)) {
      // brokerIP=strchr(pch,'=')+1;
    // }
    // if (startsWith("brokerPort",pch)) {
      // brokerPort=strchr(pch,'=')+1;
    // }
    // if (startsWith("brokerUser",pch)) {
      // brokerUser=strchr(pch,'=')+1;
    // }
    // if (startsWith("brokerPassword",pch)) {
      // brokerPassword=strchr(pch,'=')+1;
    // }

    if (startsWith("delay",pch)) {
      sleepDelayInSeconds=strchr(pch,'=')+1;
    }
    pch = strtok (NULL, "&");
    }

    IPAddress _ip(getValue(ip, '.', 0).toInt(), getValue(ip, '.', 1).toInt(), getValue(ip, '.', 2).toInt(), getValue(ip, '.', 3).toInt());
    IPAddress _gateway(getValue(gate, '.', 0).toInt(), getValue(gate, '.', 1).toInt(), getValue(gate, '.', 2).toInt(), getValue(gate, '.', 3).toInt());
    IPAddress _mask(getValue(mask, '.', 0).toInt(), getValue(mask, '.', 1).toInt(), getValue(mask, '.', 2).toInt(), getValue(mask, '.', 3).toInt());

    // Adafruit_MQTT_Client mqtt(&client, brokerIP.c_str(), (uint16_t)brokerPort.toInt(), brokerUser.c_str(), brokerPassword.c_str());
    /****************************** Feeds ***************************************/
    // Adafruit_MQTT_Publish _temperature             = Adafruit_MQTT_Publish(&mqtt, "/holcik/Vlhkomer5/Temperature");
    // Adafruit_MQTT_Publish _humidity                = Adafruit_MQTT_Publish(&mqtt, "/holcik/Vlhkomer5/Humidity");
    // Adafruit_MQTT_Publish _versionSW               = Adafruit_MQTT_Publish(&mqtt, "/holcik/Vlhkomer5/VersionSW");
    
    WiFi.mode(WIFI_STA);
    WiFi.config(_ip, _gateway, _mask);
    
    char _ssid[ssid.length()+1];
    char _passw[passw.length()+1];
    ssid.toCharArray(_ssid, ssid.length()+1);
    passw.toCharArray(_passw, passw.length()+1); 
    
    DEBUG_PRINTLN(_ssid);
    DEBUG_PRINTLN(_passw);

    WiFi.begin(_ssid, _passw);

    byte cyklu=0;  
    while (WiFi.status() != WL_CONNECTED) {
      cyklu++;
      delay(ONECYCLE_IN_MS);
      DEBUG_PRINT(".");
      if (cyklu*ONECYCLE_IN_MS>TIMEOUTFORWIFI_IN_MS) {
        DEBUG_PRINTLN();
        if (mode==3) {
          DEBUG_PRINT("Wifi unavailable, restart.");
          ESP.restart();
        } else {
          DEBUG_PRINT("Wifi unavailable, go to deepsleep for 5 minutes");
          ESP.deepSleep(300 * MICROSECOND, WAKE_RF_DEFAULT);
        }
      }
    }
    
    if (mode==4) {
      while (WiFi.status() != WL_CONNECTED) {
        cyklu++;
        delay(ONECYCLE_IN_MS);
        DEBUG_PRINT(".");
        if (cyklu*ONECYCLE_IN_MS>TIMEOUTFORWIFI_IN_MS) {
          DEBUG_PRINTLN();
          DEBUG_PRINT("Wifi unavailable, go to deepsleep for 5 minutes");
          ESP.deepSleep(300 * MICROSECOND, WAKE_RF_DEFAULT);
        }
      }
    }

    DEBUG_PRINTLN("");
    DEBUG_PRINTLN("WiFi connected");  
    DEBUG_PRINT("IP address: ");
    DEBUG_PRINTLN(WiFi.localIP());
    DEBUG_PRINT("Gateway: ");
    DEBUG_PRINTLN(WiFi.gatewayIP());
    DEBUG_PRINT("Subnet: ");
    DEBUG_PRINTLN(WiFi.subnetMask());
    DEBUG_PRINT("MAC: ");
    DEBUG_PRINTLN(WiFi.macAddress());
    DEBUG_PRINT("Interval: ");
    DEBUG_PRINT(sleepDelayInSeconds);
    DEBUG_PRINTLN(" seconds");

    sensor.begin(SDA,SCL);
    serverA.on ( "/", handleRoot );
  	serverA.begin();
    DEBUG_PRINTLN ( "HTTP server started" );
  }
}

void loop() {
  if (mode==2) {
    WiFiClient client = server.available();
      
    if (client) {
      while (client.connected()) {
        if (client.available()) {
           char c = client.read();
           //read MaxHeaderLength number of characters in the HTTP header
           //discard the rest until \n
           if (HttpHeader.length() < MaxHeaderLength) {
             //store characters to string
             HttpHeader = HttpHeader + c;
           }
           //if HTTP request has ended
           if (c == '\n') {
             if (HttpHeaderBak.length()>0) {
               HttpHeader=HttpHeaderBak;
               HttpHeaderBak="";
             }
             
             String key;
             
             char *pch;
             byte s[HttpHeader.length()];
             HttpHeader.getBytes(s, HttpHeader.length());
             pch = strtok((char*)s,"?&=");
             while (pch != NULL) {
               DEBUG_PRINTLN(pch);
              if (startsWith("ssid",pch)) {
                ssid=strchr(pch,'=')+1;
              }
              if (startsWith("passw",pch)) {
                passw=strchr(pch,'=')+1;
              }
              if (startsWith("ip",pch)) {
                ip=strchr(pch,'=')+1;
              }
              if (startsWith("mask",pch)) {
                mask=strchr(pch,'=')+1;
              }
              if (startsWith("gateway",pch)) {
                gate=strchr(pch,'=')+1;
              }
              // if (startsWith("brokerIP",pch)) {
                // brokerIP=strchr(pch,'=')+1;
              // }
              // if (startsWith("brokerPort",pch)) {
                // brokerPort=strchr(pch,'=')+1;
              // }
              // if (startsWith("brokerUser",pch)) {
                // brokerUser=strchr(pch,'=')+1;
              // }
              // if (startsWith("brokerPassword",pch)) {
                // brokerPassword=strchr(pch,'=')+1;
              // }
              if (startsWith("delay",pch)) {
                sleepDelayInSeconds=strchr(pch,'=')+1;
              }
              pch = strtok (NULL, "&");
             }

             
             // client.println("HTTP/1.1 200 OK");
             // client.println("Content-Type: text/html");
             // client.println("<html><head><meta charset='UTF-8'></head><body>");
             // client.println();
             client.print("<html><head><meta charset='UTF-8'></head><style> body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }</style>");
             client.print("<form>");
             client.print("<span style='font-weight:bold; font-size:20pt;'>Vlhkoměr</span><span style='font-style:italic;'>v");
             client.print(versionSW);
             client.print("Topic base: ");
             client.print(MQTTBASE);             
             client.print("</span><br/><br/>");
             client.print("<table><tr><td style='text-align:right;'>AP SSID:</td><td><input type='text' name=ssid value='");
             client.print(ssid);
             client.print("'/></td></tr>");
             client.print("<tr><td style='text-align:right;'>AP password:</td><td><input type='text' name=passw value='" + passw + "'/></td></tr>");
             client.print("<tr><td style='text-align:right;'>IP:</td><td><input type='text' name='ip' maxlength='15' value='" + ip + "'/></td></tr>");
             client.print("<tr><td style='text-align:right;'>Maska:</td><td><input type='text' name='mask' maxlength='15' value='" + mask + "'/></td></tr>");
             client.print("<tr><td style='text-align:right;'>Gateway:</td><td><input type='text' name='gateway' maxlength='15' value='" + gate + "'/></td></tr>");
             // client.print("<tr><td style='text-align:right;'>MQTT Broker IP:</td><td><input type='text' name='brokerIP' maxlength='15' value='" + brokerIP + "'/></td></tr>");
             // client.print("<tr><td style='text-align:right;'>MQTT Broker port:</td><td><input type='text' name='brokerPort' maxlength='5' value='" + brokerPort + "'/></td></tr>");
             // client.print("<tr><td style='text-align:right;'>MQTT Broker user:</td><td><input type='text' name='brokerUser' maxlength='15' value='" + brokerUser + "'/></td></tr>");
             // client.print("<tr><td style='text-align:right;'>MQTT Broker password:</td><td><input type='text' name='brokerPassword' maxlength='15' value='" + brokerPassword + "'/></td></tr>");
             client.print("<tr><td style='text-align:right;'>Prodleva mezi měřeními [s]:</td><td><input type='text' name='delay' maxlength='5' value='" + sleepDelayInSeconds + "'/></td></tr>");
             client.print("<tr><td style='text-align:right'><input type=submit name=ulozit value='Uložit'></td>");
             // client.print("<td style='text-align:right'><input type=submit name=reset value='Reset'></td></tr>");
             client.print("</table></form>");
             client.print("</body></html>");

             if (HttpHeader.indexOf('?') > 0) {
              // open file for writing
              File f = SPIFFS.open("/config.txt", "w");
              if (!f) {
                  DEBUG_PRINTLN("file open failed");
              }
              DEBUG_PRINTLN("====== Writing to SPIFFS file =========");
              DEBUG_PRINT("HttpHeader:");
              DEBUG_PRINTLN(HttpHeader);
              // write 10 strings to file
              f.println(HttpHeader);
              f.close();
             }
             //clearing string for next read
             HttpHeader="";
             //stopping client
             client.stop();
          }
        }
      }
    }
  } else if (mode==3 || mode==4) {
    temperature=sensor.getCelsiusHundredths();
    humidity=sensor.getHumidityPercent();
    if (millis() - lastDisp >= dispDelay) {
      lastDisp = millis();
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
    }
    if (mode==3) {
      serverA.handleClient();
    }
    if (millis() - lastSend >= sendDelay) {
      lastSend = millis();
      //send MQTT data
      sendDataHA();
    }
    if (mode==4) { //mererni
      DEBUG_PRINT("Start duration:");
      DEBUG_PRINT(millis()-start);
      DEBUG_PRINTLN(" ms.");
      DEBUG_PRINTLN("Entering deep sleep mode.");
      DEBUG_PRINT("Wake in ");
      DEBUG_PRINT(sleepDelayInSeconds);
      DEBUG_PRINTLN(" s.");
      delay(2000);
      ESP.deepSleep(sleepDelayInSeconds.toInt() * MICROSECOND, WAKE_RF_DEFAULT);
    }
  }
}

bool startsWith(const char *pre, const char *str) {
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
}


int readConfig() {
  DEBUG_PRINTLN("Read config file.");
  SPIFFS.begin();
  // open file for reading
  File f = SPIFFS.open("/config.txt", "r");
  if (!f) {
    DEBUG_PRINTLN("file open failed");
    return 1;
  } else {
    DEBUG_PRINTLN("====== Reading from SPIFFS file =======");
    HttpHeaderBak=f.readStringUntil('\n');
    DEBUG_PRINTLN(HttpHeaderBak);
  }
  return 0;
}


String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void sendDataHA() {
  DEBUG_PRINTLN(" I am sending data from humidity and temperature sensor to HomeAssistant");
  MQTT_connect();
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
  if (! _bootTime.publish((uint8_t)(millis()-start))) {
    DEBUG_PRINTLN("BootTime failed");
  } else {
    DEBUG_PRINTLN("BootTime OK!");
  }  
  if (! _versionSW.publish(versionSW)) {
    DEBUG_PRINTLN("Version SW failed");
  } else {
    DEBUG_PRINTLN("Version SW OK!");
  }  
}
 
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