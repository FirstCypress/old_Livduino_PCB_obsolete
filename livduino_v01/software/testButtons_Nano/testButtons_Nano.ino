/*
   LiVduino - Arduino Nano

   alfredc333
   2017
   MIT license

*/

//test program
//test LiVduino buttons

#include <Wire.h>
#include <RTClib.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

SSD1306AsciiWire oled;

//Buzzer
const byte buzzerPin       = 12;
const byte buzzerButtonPin = 2;
int buzzerState            = LOW;

//Sonoff
const byte sonoffButtonPin = 5;
int sonoffState            = LOW;

//ThingSpeak
const byte thingSpeakButtonPin = 4;
int thingSpeakState            = LOW;

//Extra button unused
const byte extraButtonPin = 3;
int extraState            = LOW;

/*********************************************************/
void setup()
{
  //set serial port for communication with NodeMCU
  Serial.begin(9600);
  while (!Serial) {
    delay(10);
  }
  
  Serial.println("SSD1306 init");
  oled.begin(&Adafruit128x64, 0x3C);
  oled.setFont(System5x7);
  oled.set2X();

  pinMode(buzzerPin, OUTPUT);
  pinMode(buzzerButtonPin, INPUT_PULLUP);
  pinMode(sonoffButtonPin, INPUT_PULLUP);
  pinMode(thingSpeakButtonPin, INPUT_PULLUP);
}

/*********************************************************/
void loop()
{

  readButtons(buzzerState, sonoffState, thingSpeakState, extraState);
  displayPageThree(buzzerState, sonoffState, thingSpeakState, extraState);

  //sound buzzer if 
  if (buzzerState == 1) {
    digitalWrite(buzzerPin, HIGH);
  } else {
    digitalWrite(buzzerPin, LOW);
  }
  delay(2000);

}


//readButtons
void readButtons(int &buzzerState, int &sonoffState, int &thingSpeakState, int &extraState) {
  buzzerState     = digitalRead(buzzerButtonPin);
  sonoffState     = digitalRead(sonoffButtonPin);
  thingSpeakState = digitalRead(thingSpeakButtonPin);
  extraState      = digitalRead(extraButtonPin);
}



//this page shows states for Buzzer, Sonoff and ThingSpeak
//if a button is pushed down, the state is ON
//when Buzzer state is ON, you will hear the buzzer sound
void displayPageThree(int bState, int sState, int tsState, int eState) {
  String buzzerState     = "NOT_INIT";
  String sonoffState     = "NOT_INIT";
  String thingSpeakState = "NOT_INIT";
  String extraState = "NOT_INIT";

  if (bState == 1) {
    buzzerState = "ON";
  }
  else {
    buzzerState = "OFF";
  }

  if (sState == 1) {
    sonoffState = "ON";
  }
  else {
    sonoffState = "OFF";
  }

  if (tsState == 1) {
    thingSpeakState = "ON";
  }
  else {
    thingSpeakState = "OFF";
  }

  if (eState == 1) {
    extraState = "ON";
  }
  else {
    extraState = "OFF";
  }
  oled.clear();
  oled.println("Buzzer: " + buzzerState);
  oled.println("Sonoff: " + sonoffState);
  oled.println("ThingS: " + thingSpeakState);
  oled.println("Extra:  " + extraState);
}

