unsigned long start=millis();

#include <ESP8266WiFi.h>
#include <SI7021.h>
#include <Xively.h>
#include <WiFiClient.h> 
#include <FS.h>

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
String feedId;


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
unsigned long xivelyFeedHumidity = 0;
XivelyFeed feedHumidity(xivelyFeedHumidity,         datastreamsHumidity,       4);
WiFiClient clientXively;
XivelyClient xivelyClientHumidity(clientXively);
char xivelyKey[50];


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
          <h1>Vlhkoměr</h1>\
          <p>FeedID: %20d<p>\
          <p>Teplota: %3d,%02d °C Vlhkost: %3d \%Rh Napětí: %2d,%02d V</p>\
        </body>\
      </html>",
      feedHumidity._id, 
		 (int)(temperature/TEMPERATURE_DIVIDOR), ((int)temperature)%TEMPERATURE_DIVIDOR, (int)humidity, (int)voltage, ((int)(voltage*100.0))%100
	);
	serverA.send ( 200, "text/html", temp );
}

int readConfig(void);
bool startsWith(const char *, const char *);
String getValue(String, char, int);

void setup() {
  Serial.begin(PORTSPEED);
  Serial.println();
  Serial.println("Vlhkoměr");
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
  Serial.print("Mode:");
  Serial.println(mode);

  if (mode==2) { //setup
    delay(1000);
    /* Set these to your desired credentials. */
    const char *ssid;
    const char *password;

    ssid = "ESPHum";
    password = "hum007";

    Serial.println();
    Serial.print("Configuring access point...");
    /* You can remove the password parameter if you want the AP to be open. */
    WiFi.softAP(ssid, password);

    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    Serial.print("SSID: ");
    Serial.println(ssid);
    Serial.print("Password: ");
    Serial.println(password);
    server.begin();
    Serial.println("HTTP server started");
    HttpHeader = "";
    
    readConfig();
    
  } else if (mode==3 || mode==4) { //test a mereni
    if (readConfig()>0) {
      Serial.println("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      Serial.println("!!!! Zařízení není nakonfigurované, končím !!!!!");
      Serial.println("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      return;
    }
  
    char *pch;
    byte s[HttpHeaderBak.length()];
    HttpHeaderBak.getBytes(s, HttpHeaderBak.length());
    Serial.println(HttpHeaderBak);
    pch = strtok((char*)s,"?&=");
    while (pch != NULL) {
     Serial.println(pch);
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
    if (startsWith("APIkey",pch)) {
      memcpy(xivelyKey, strchr(pch,'=')+1, strlen(strchr(pch,'=')+1)+1);
    }
    if (startsWith("FeedID",pch)) {
      feedId=strchr(pch,'=')+1;
    }
    if (startsWith("delay",pch)) {
      sleepDelayInSeconds=strchr(pch,'=')+1;
    }
    pch = strtok (NULL, "&");
    }

    IPAddress _ip(getValue(ip, '.', 0).toInt(), getValue(ip, '.', 1).toInt(), getValue(ip, '.', 2).toInt(), getValue(ip, '.', 3).toInt());
    IPAddress _gateway(getValue(gate, '.', 0).toInt(), getValue(gate, '.', 1).toInt(), getValue(gate, '.', 2).toInt(), getValue(gate, '.', 3).toInt());
    IPAddress _mask(getValue(mask, '.', 0).toInt(), getValue(mask, '.', 1).toInt(), getValue(mask, '.', 2).toInt(), getValue(mask, '.', 3).toInt());
     
    WiFi.mode(WIFI_STA);
    WiFi.config(_ip, _gateway, _mask);
    
    char _ssid[ssid.length()+1];
    char _passw[passw.length()+1];
    ssid.toCharArray(_ssid, ssid.length()+1);
    passw.toCharArray(_passw, passw.length()+1); 
    
    Serial.println(_ssid);
    Serial.println(_passw);

    WiFi.begin(_ssid, _passw);

    byte cyklu=0;  
    while (WiFi.status() != WL_CONNECTED) {
      cyklu++;
      delay(ONECYCLE_IN_MS);
      Serial.print(".");
      if (cyklu*ONECYCLE_IN_MS>TIMEOUTFORWIFI_IN_MS) {
        Serial.println();
        if (mode==3) {
          Serial.print("Wifi unavailable, restart.");
          ESP.restart();
        } else {
          Serial.print("Wifi unavailable, go to deepsleep for 5 minutes");
          ESP.deepSleep(300 * MICROSECOND, WAKE_RF_DEFAULT);
        }
      }
    }
    
    if (mode==4) {
      while (WiFi.status() != WL_CONNECTED) {
        cyklu++;
        delay(ONECYCLE_IN_MS);
        Serial.print(".");
        if (cyklu*ONECYCLE_IN_MS>TIMEOUTFORWIFI_IN_MS) {
          Serial.println();
          Serial.print("Wifi unavailable, go to deepsleep for 5 minutes");
          ESP.deepSleep(300 * MICROSECOND, WAKE_RF_DEFAULT);
        }
      }
    }

    feedHumidity._id = feedId.toInt();
    
    Serial.println("");
    Serial.println("WiFi connected");  
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Gateway: ");
    Serial.println(WiFi.gatewayIP());
    Serial.print("Subnet: ");
    Serial.println(WiFi.subnetMask());
    Serial.print("MAC: ");
    Serial.println(WiFi.macAddress());
    Serial.print("Interval: ");
    Serial.print(sleepDelayInSeconds);
    Serial.println(" seconds");
    Serial.print("Feed ID:");
    Serial.println(feedId.toInt());
    Serial.print("APIkey:");
    Serial.println(xivelyKey);


    sensor.begin(SDA,SCL);
    serverA.on ( "/", handleRoot );
  	serverA.begin();
    Serial.println ( "HTTP server started" );
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
               Serial.println(pch);
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
              if (startsWith("APIkey",pch)) {
                key=strchr(pch,'=')+1;
              }
              if (startsWith("FeedID",pch)) {
                feedId=strchr(pch,'=')+1;
              }
              if (startsWith("delay",pch)) {
                sleepDelayInSeconds=strchr(pch,'=')+1;
              }
              pch = strtok (NULL, "&");
             }

             
             client.println("HTTP/1.1 200 OK");
             client.println("Content-Type: text/html");
             client.println("<html><head><meta charset='UTF-8' /></head><body>");
             client.println();
             client.print("<form method=get>");
             client.print("<span style='font-weight:bold; font-size:20pt;'>Vlhkoměr</span><span style='font-style:italic;'>Verze FW: 1.0.1</span><br/><br/>");
             client.print("<table><tr><td style='text-align:right;'>AP SSID:</td><td><input type='text' name=ssid value='");
             client.print(ssid);
             client.print("'/></td></tr>");
             client.print("<tr><td style='text-align:right;'>AP password:</td><td><input type='text' name=passw value='" + passw + "'/></td></tr>");
             client.print("<tr><td style='text-align:right;'>IP:</td><td><input type='text' name='ip' maxlength='15' value='" + ip + "'/></td></tr>");
             client.print("<tr><td style='text-align:right;'>Maska:</td><td><input type='text' name='mask' maxlength='15' value='" + mask + "'/></td></tr>");
             client.print("<tr><td style='text-align:right;'>Gateway:</td><td><input type='text' name='gateway' maxlength='15' value='" + gate + "'/></td></tr>");
             client.print("<tr><td style='text-align:right;'>Xively API key:</td><td><input type='text' name='APIkey' value='" + key + "'/></td></tr>");
             client.print("<tr><td style='text-align:right;'>Feed ID:</td><td><input type='text' name='FeedID' value='" + feedId + "'/></td></tr>");
             client.print("<tr><td style='text-align:right;'>Prodleva mezi měřeními [s]:</td><td><input type='text' name='delay' maxlength='5' value='" + sleepDelayInSeconds + "'/></td></tr>");
             client.print("<tr><td style='text-align:right'><input type=submit name=ulozit value='Uložit'></td>");
             // client.print("<td style='text-align:right'><input type=submit name=reset value='Reset'></td></tr>");
             client.print("</table></form>");
             client.print("</body></html>");

             if (HttpHeader.indexOf('?') > 0) {
              // open file for writing
              File f = SPIFFS.open("/config.txt", "w");
              if (!f) {
                  Serial.println("file open failed");
              }
              Serial.println("====== Writing to SPIFFS file =========");
              Serial.print("HttpHeader:");
              Serial.println(HttpHeader);
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
      Serial.print("Temperature:");
      Serial.print(temperature/TEMPERATURE_DIVIDOR);
      Serial.println("C");
      Serial.print("Humidity:");
      Serial.print(humidity);
      Serial.println("%");
      voltage = (float)ESP.getVcc()/MILIVOLT_TO_VOLT;
      Serial.print("Voltage:");
      Serial.print(voltage);
      Serial.println("V");
      Serial.println();
    }
    if (mode==3) {
      serverA.handleClient();
    }
    if (millis() - lastSend >= sendDelay) {
      lastSend = millis();
      if (!clientXively.connected()) {
        datastreamsHumidity[0].setFloat(humidity);
        datastreamsHumidity[1].setFloat(temperature/TEMPERATURE_DIVIDOR);
        datastreamsHumidity[2].setFloat(voltage);
        //datastreamsHumidity[3].setFloat((float)(millis() - boottime)/millis_to_sec);
        Serial.print("uploading data to xively, Feed:");
        Serial.print(feedHumidity._id);
        Serial.print(" xivelyKey:");
        Serial.println(xivelyKey);
        int ret = xivelyClientHumidity.put(feedHumidity, xivelyKey);
        
        Serial.print(F("xivelyClientHumidity.put returned "));
        Serial.println(ret);
      }
    }
    if (mode==4) { //mererni
      Serial.print("Start duration:");
      Serial.print(millis()-start);
      Serial.println(" ms.");
      Serial.println("Entering deep sleep mode.");
      Serial.print("Wake in ");
      Serial.print(sleepDelayInSeconds);
      Serial.println(" s.");
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
  SPIFFS.begin();
  // open file for reading
  File f = SPIFFS.open("/config.txt", "r");
  if (!f) {
    Serial.println("file open failed");
    return 1;
  } else {
    Serial.println("====== Reading from SPIFFS file =======");
    HttpHeaderBak=f.readStringUntil('\n');
    Serial.println(HttpHeaderBak);
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