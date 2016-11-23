#include <ESP8266WiFi.h>
#include <SI7021.h>

SI7021 sensor;

#define TEMPERATURE_DIVIDOR 100.0
#define MILIVOLT_TO_VOLT 1000.0

#define SDA 12 // D6 - GPI12 on ESP-201 module
#define SCL 14 // D5 - GPI14 on ESP-201 module

#include <ESP8266WebServer.h>
ESP8266WebServer serverA(80);

// #include <WiFiClient.h> 
// #include <FS.h>

#define MaxHeaderLength 1600
//byte mode=1; //flash
//byte mode=2; //setup
byte mode=3; //test
//byte mode=4; //mereni

#include "FS.h"

/* Set these to your desired credentials. */
const char *ssid;
const char *password;

String HttpHeader = String(MaxHeaderLength);
String HttpHeaderBak = String(MaxHeaderLength);

volatile float temperature;
volatile float humidity;


WiFiServer server(80);

void handleRoot() {
	char temp[400];

	snprintf ( temp, 400,

"<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>Vlhkomer</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Vlhkomer</h1>\
    <p>Teplota: %05d Vlhkost: %05d</p>\
  </body>\
</html>",

		(int)(temperature/TEMPERATURE_DIVIDOR), (int)humidity
	);
	serverA.send ( 200, "text/html", temp );
}

void setup() {
  Serial.begin(115200);
  if (mode==2) {
    delay(1000);
    ssid = "ESPHum";
    password = "hum007";

    //SPIFFS.begin();
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
    //server.on("/", handleRoot);
    server.begin();
    Serial.println("HTTP server started");
    HttpHeader = "";
    SPIFFS.begin();
    // open file for reading
    File f = SPIFFS.open("/config.txt", "r");
    if (!f) {
      Serial.println("file open failed");
    } else {
      Serial.println("====== Reading from SPIFFS file =======");
      HttpHeaderBak=f.readStringUntil('\n');
      Serial.println(HttpHeaderBak);
    }
  } else if (mode==3) {
    ssid = "Datlovo";
    password = "Nu6kMABmseYwbCoJ7LyG";
 
    IPAddress ip(192,168,1,166);  //Node static IP
    IPAddress gateway(192,168,1,2);
    IPAddress subnet(255,255,255,0);
      
    WiFi.mode(WIFI_STA);
    WiFi.config(ip, gateway, subnet);
    WiFi.begin(ssid, password);
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
    sensor.begin(SDA,SCL);
    serverA.on ( "/", handleRoot );
    // server.on ( "/inline", []() {
		// server.send ( 200, "text/plain", "this works as well" );
      // } );
    // server.onNotFound ( handleNotFound );
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
             // show the string on the monitor
             Serial.print("HttpHeader:");
             Serial.println(HttpHeader);
             
            // open file for writing
            File f = SPIFFS.open("/config.txt", "w");
            if (!f) {
                Serial.println("file open failed");
            }
            Serial.println("====== Writing to SPIFFS file =========");
            // write 10 strings to file
            f.println(HttpHeader);
            f.close();

             String ssid;
             String passw;
             String ip;
             String mask;
             String gate;
             String apiKey;
             String feedId;
             String delay;
             
             char *pch;
             byte typ=0;
             byte s[HttpHeader.length()];
             HttpHeader.getBytes(s, HttpHeader.length());
             pch = strtok((char*)s,"?&=");
             while (pch != NULL) {
               Serial.println(pch);
               if (startsWith("ssid",pch)) {
                 typ=1;
               } else if (startsWith("passw",pch)) {
                 typ=2;
               } else if (startsWith("ip",pch)) {
                 typ=3;
               } else if (startsWith("mask",pch)) {
                 typ=4;
               } else if (startsWith("gate",pch)) {
                 typ=5;
               } else if (startsWith("APIiKey",pch)) {
                 typ=6;
               } else if (startsWith("FeedId",pch)) {
                 typ=7;
               } else if (startsWith("delay",pch)) {
                 typ=8;
               } else if (startsWith("ulozit",pch)) {
                 typ=9;
               } else if (startsWith("reset",pch)) {
                 typ=10;
               }

               pch = strtok (NULL, "?&=");
               if (typ==1) {
                 ssid=pch;
               } else if (typ==2) {
                 passw=pch;
               } else if (typ==3) {
                 ip=pch;
               } else if (typ==4) {
                 mask=pch;
               } else if (typ==5) {
                 gate=pch;
               } else if (typ==6) {
                 apiKey=pch;
               } else if (typ==7) {
                 feedId=pch;
               } else if (typ==8) {
                 delay=pch;
               } else if (typ==9) {
                 
               } else if (typ==10) {
                 // //reset 
                 // Serial.println("ESP RESTART NOW");
                 // ESP.restart();
               }
             }

             
             client.println("HTTP/1.1 200 OK");
             client.println("Content-Type: text/html");
             client.println("<html><head><meta charset='Windows-1250' /></head><body>");
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
             client.print("<tr><td style='text-align:right;'>Xively API key:</td><td><input type='text' name='APIkey' value='" + apiKey + "'/></td></tr>");
             client.print("<tr><td style='text-align:right;'>Feed ID:</td><td><input type='text' name='FeedID' value='" + feedId + "'/></td></tr>");
             client.print("<tr><td style='text-align:right;'>Prodleva mezi měřeními [s]:</td><td><input type='text' name='delay' maxlength='5' value='" + delay + "'/></td></tr>");
             client.print("<tr><td style='text-align:right'><input type=submit name=ulozit value='Uložit'></td>");
             // client.print("<td style='text-align:right'><input type=submit name=reset value='Reset'></td></tr>");
             client.print("</table></form>");
             client.print("</body></html>");

             //clearing string for next read
             HttpHeader="";
             //stopping client
             client.stop();
          }
        }
      }
    }
  } else if (mode==3) {
    temperature=sensor.getCelsiusHundredths();
    humidity=sensor.getHumidityPercent();
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
    serverA.handleClient();
    delay(1000);
  }
}

bool startsWith(const char *pre, const char *str) {
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
}
