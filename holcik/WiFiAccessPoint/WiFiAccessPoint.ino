#include <ESP8266WiFi.h>
// #include <WiFiClient.h> 
// #include <ESP8266WebServer.h>
// #include <FS.h>

#define MaxHeaderLength 1600
byte mode=1;

#include "FS.h"

/* Set these to your desired credentials. */
const char *ssid = "ESPHum";
const char *password = "hum007";

String HttpHeader = String(MaxHeaderLength);
String HttpHeaderBak = String(MaxHeaderLength);

WiFiServer server(80);

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
 * connected to this access point to see it.
 */
void handleRoot() {
  // String contentType = "text/html";
  // File file = SPIFFS.open("/default.htm", "r");
  // size_t sent = server.streamFile(file, contentType);
  // file.close();
  // char temp[1500];
  // snprintf ( temp, 1500, "<html><head></head><body><form method=get><input type='radio' name=r value='1'> One<br><input type='radio' name=r value='2' checked>\
      // Two<br><input type='radio' name=r value='3'> Three<br><input type=submit value=submit></form></body></html>");
  // // snprintf ( temp, 1500, "<!DOCTYPE html><html lang='en' xmlns='http://www.w3.org/1999/xhtml'><head><meta charset='utf-8' />\
        // // // <title>Vlhkoměr</title></head><body><span style='font-weight:bold; font-size:20pt;'>Vlhkoměr</span><span style='font-style:italic;'> \
        // // // Verze FW: 1.0.1</span><br/><br/><table><tr><td style='text-align:right;'>AP SSID:</td><td><input id='SSID' type='text' /></td></tr><tr>\
        // // // <td style='text-align:right;'>AP password:</td><td><input id='passw' type='text' /></td></tr><tr><td style='text-align:right;'>IP:</td><td>\
        // // // <input id='IP' type='text' maxlength='15' /></td></tr><tr><td style='text-align:right;'>Maska:</td><td><input id='mask' type='text' \
        // // // maxlength='15' /></td></tr><tr><td style='text-align:right;'>Gateway:</td><td><input id='gateway' type='text' maxlength='15' />\
        // // // </td></tr><tr><td style='text-align:right;'>Xively API key:</td><td><input id='APIkey' type='text' /></td></tr><tr><td style=\
        // // // 'text-align:right;'>Xively feed ID:</td><td><input id='FeedID' type='text' /></td></tr><tr><td style='text-align:right;'>\
        // // // Prodleva mezi měřeními [s]:</td><td><input id='delay' type='text' maxlength='5' /></td></tr><tr><td colspan='2' \
        // // // style='text-align:center'><input id='Submit1' type='submit' value='Uložit' /></td></tr></table></body></html>");
  // server.send ( 200, "text/html", temp );
  
  
  Serial.println("HandleRoot");
}

void handleClient() {
  Serial.println("HandleClient");
}

void setup() {
  if (mode==1) {
    delay(1000);
    //SPIFFS.begin();
    Serial.begin(115200);
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
  }
}

void loop() {
  if (mode==1) {
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
  }
}

bool startsWith(const char *pre, const char *str) {
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
}
