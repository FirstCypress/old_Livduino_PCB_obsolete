/*
   LiVduino - nodeMCU

   alfredc333
   2017
   MIT License
*/

//test program
//switches Sonoff on and off every 10 seconds

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include "livmsg.h"

//NodeMCU WiFi
ESP8266WiFiMulti WiFiMulti;
const char accessPointName[]     = "YOUR_SSID";
const char accessPointPassword[] = "YOUR_PASSWD";

//Sonoff
String sonoffIPAddress = "YOUR_SONOFF_IP_ADDRESS";
String sonoffOn        = String("cmd=event,T1");
String sonoffOff       = String("cmd=event,T0");


void setup() {
  //Set Serial port to receive liv messages from Nano
  Serial.begin(9600);

  //Sonoff
  WiFiMulti.addAP(accessPointName, accessPointPassword);

}

String s_on       = "http://" + sonoffIPAddress + "/control?" + sonoffOn;
String s_off       = "http://" + sonoffIPAddress + "/control?" + sonoffOff;

void loop() {
  int counter = 0;
  while (1) {
    if (counter % 2 == 0) {
      Serial.println("Switch Sonoff on");
      controlSonoff(s_on);
    } else {
      Serial.println("Switch Sonoff off");
      controlSonoff(s_off);
    }
    delay(10000);
    counter += 1;
  }
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
      // HTTP header has been sent and Server response header has been handled
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

