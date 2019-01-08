/*
   LiVduino - nodeMCU

   alfredc333
   2017
   MIT License
*/

//test program
//writes hardcoded temp, hum, light data to ThingSpeak every 30 seconds

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include "ThingSpeak.h"

//NodeMCU WiFi
ESP8266WiFiMulti WiFiMulti;
const char accessPointName[]     = "YOUR_SSID_NAME";
const char accessPointPassword[] = "YOUR_PASSWD";


//Thingspeak
int status  = WL_IDLE_STATUS;
WiFiClient  client;
unsigned long myChannelNumber = your_channel_no;
const char * myWriteAPIKey = "YOUR_API_KEY";


void setup() {
  //Set Serial port
  Serial.begin(9600);

  //ThingSpeak
  WiFi.begin(accessPointName, accessPointPassword);
  ThingSpeak.begin(client);
}

void loop() {
  float t    = 100;
  float h    = 200;
  float l    = 300;
  Serial.println("Sending data to ThingSpeak");
  updateThingSpeak(t, h, l);
  delay(30000);
}

void updateThingSpeak(float temp, float humid, float light) {
  Serial.println("Write data to ThingSpeak");
  ThingSpeak.setField(1, temp);
  ThingSpeak.setField(2, humid);
  ThingSpeak.setField(3, light);
  // Write the fields that you've set all at once.
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
}


