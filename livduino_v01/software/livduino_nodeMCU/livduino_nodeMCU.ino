/*
   LiVduino - nodeMCU

   alfredc333
   2017
   MIT License
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include "ThingSpeak.h"
#include "livmsg.h"

//NodeMCU WiFi
ESP8266WiFiMulti WiFiMulti;
const char accessPointName[]     = "YOUR_SSID";
const char accessPointPassword[] = "YOUR_PASSWD";

//Sonoff
String sonoffIPAddress = "YOUR_SONOFF_IP_ADDRESS";
String sonoffOn        = String("cmd=event,T1");
String sonoffOff       = String("cmd=event,T0");

//Thingspeak
int status  = WL_IDLE_STATUS;
WiFiClient  client;
unsigned long myChannelNumber = your_channel_number;
const char * myWriteAPIKey = "YOUR_API_KEY";

int loopCounter = 1;

void setup() {
  //Set Serial port to receive liv messages from Nano
  Serial.begin(9600);

  //Sonoff
  WiFiMulti.addAP(accessPointName, accessPointPassword);

  //ThingSpeak
  WiFi.begin(accessPointName, accessPointPassword);
  ThingSpeak.begin(client);
}

void loop() {
  //wait for messages from Arduino Nano
  byte buffer[100];
  int incomingBytes = 0;
  float t    = 0;
  float h    = 0;
  float l    = 0;
  byte o     = 99;
  String cs  = String("NOT_INIT");
  Serial.println("-----------------");
  Serial.print("loop    ");
  Serial.println(loopCounter);

  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingBytes = Serial.readBytes(buffer, 99);

    //print what you got
    Serial.print("I received:   ");
    Serial.println(incomingBytes);
    livmsg* p = (livmsg*)buffer;
    t = p->t;
    h = p->h;
    l = p->l;
    o = p->op;
    Serial.print("opcode:    ");
    Serial.println(o);
    Serial.print("t     :    ");
    Serial.println(t);
    Serial.print("h     :    ");
    Serial.println(h);
    Serial.print("l     :    ");
    Serial.println(l);
    Serial.println("\n");

    boolean SonoffOp     = false;
    boolean ThingspeakOp = false;

    if (o == SONOFF_ON) {
      cs       = "http://" + sonoffIPAddress + "/control?" + sonoffOn;
      SonoffOp = true;
    }
    else if (o == SONOFF_OFF) {
      cs       = "http://" + sonoffIPAddress + "/control?" + sonoffOff;
      SonoffOp = true;
    }
    else if (o == TS_REPORT) {
      ThingspeakOp = true;
    }
    else {
      //ignore invalid operation, this may be some debug messages on Serial from Nano
      Serial.println("invalid operation");
    }

    if (SonoffOp == true) {
      controlSonoff(cs);
    }

    if (ThingspeakOp == true) {
      updateThingSpeak(t, h, l);
    }
  }
  else {
    Serial.println("No message received");
  }

  ++loopCounter;
  delay(500);
}


void updateThingSpeak(float temp, float humid, float light) {
  Serial.println("Write data to ThingSpeak");
  ThingSpeak.setField(1, temp);
  ThingSpeak.setField(2, humid);
  ThingSpeak.setField(3, light);
  // Write the fields that you've set all at once.
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
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

