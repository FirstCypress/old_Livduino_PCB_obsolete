/*
   LiVduino - Arduino Nano

   alfredc333
   2017
   MIT license
*/

//test program
//receives messages from Nano over serial and prints their content

#include "livmsg.h"

int incomingByte = 0;  // for incoming serial data

void setup() {
  Serial.begin(9600); // opens serial port
}


byte buffer[sizeof(livmsg)];
int incomingBytes = 0;

void loop() {
  // wait for data
  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingBytes = Serial.readBytes(buffer, sizeof(livmsg));

    // say what you got:
    Serial.println("----------------");
    Serial.println("I received: ");
    Serial.println(incomingBytes);
    livmsg* p = (livmsg*)buffer;
    float t = p->t;
    float h = p->h;
    float l = p->l;
    byte o = p->op;
    Serial.println(o);
    Serial.println(t);
    Serial.println(h);
    Serial.println(l);
    Serial.println("\n");
  }
}




