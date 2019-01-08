/* 
 * LiVduino - Arduino Nano 
 *  
 * alfredc333 
 * 2017 
 * MIT license
 */ 

//test program
//sends messages from Nano to NodeMCU over serial
 
#include "livmsg.h"

void setup() {
  Serial.begin(9600); // opens serial port
}


//sends message containing temp, humidity and light hardcoded measurement every 10 seconds  
void loop() {
  livmsg lm = {SONOFF_ON, 111, 222, 333};
  Serial.write((byte*)&lm, sizeof(livmsg));
  delay(10000);
  
}
