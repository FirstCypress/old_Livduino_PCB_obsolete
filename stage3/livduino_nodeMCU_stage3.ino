/*
*   LiVduino
*   alfredc333
*   July, 2017
*   MIT License
*
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>


//Sonoff
//add here your own settings
ESP8266WiFiMulti WiFiMulti;
const char accessPointName[]      = "WIFI_SSID";
const char accessPointPassword[]  = "WIFI_PASSWORD";
String sonoffIPAddress     = "192.168.1.42";
String sonoffOn            = String("cmd=event,T1");
String sonoffOff           = String("cmd=event,T0");


int loopCounter = 1;

String cs = "NOT_INIT";

void setup() {

  Serial.begin(9600);

  //Sonoff
  WiFiMulti.addAP(accessPointName, accessPointPassword);

}

void loop() {



  if (loopCounter % 2 == 0) {
    cs       = "http://" + sonoffIPAddress + "/control?" + sonoffOn;
  }
  else {
    cs       = "http://" + sonoffIPAddress + "/control?" + sonoffOff;
  }


  controlSonoff(cs);


  ++loopCounter;
  delay(5000);
}


void controlSonoff(String cs) {
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    HTTPClient http;
    Serial.print(cs);
    http.begin(cs); //HTTP
    Serial.print("[HTTP] Begin connection ...\n");
    // start connection and send HTTP header
    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET code: %d\n", httpCode);
      // command ok
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.print("OK code:    ");
        Serial.println(payload);
      }
    } else {
      Serial.printf("[HTTP] GET failed with error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  }
}

