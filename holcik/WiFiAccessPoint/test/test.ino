String HttpHeader = String(1600);

void setup() {
  Serial.begin(115200);
  delay(2000);
}

void loop() {
  HttpHeader = "GET /favicon.ico HTTP/1.1 HttpHeader:GET /?ssid=sdfsd&passw=&ip=192.168.1.166&mask=&gateway=&APIkey=&FeedID=&delay=3000&ulozit=Ulo%C5%BEit HTTP/1.1";
  
  String ssid;
  String passw;
  String ip;
  String mask;
  String gate;
  String apiKey;
  String feedId;
  String del;

  char *pch;
  char *poz;
  byte typ=0;
  byte s[HttpHeader.length()];
  HttpHeader.getBytes(s, HttpHeader.length());
  pch = strtok((char*)s,"?");
  while (pch != NULL) {
    //Serial.println(pch);
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
      apiKey=strchr(pch,'=')+1;
    }
    if (startsWith("FeedID",pch)) {
      feedId=strchr(pch,'=')+1;
    }
    if (startsWith("delay",pch)) {
      del=strchr(pch,'=')+1;
    }
    pch = strtok (NULL, "&");
  }
  Serial.println(ssid);
  Serial.println(passw);
  Serial.println(ip);
  Serial.println(mask);
  Serial.println(gate);
  Serial.println(apiKey);
  Serial.println(feedId);
  Serial.println(del);

  delay(10000);
}


bool startsWith(const char *pre, const char *str) {
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
}


