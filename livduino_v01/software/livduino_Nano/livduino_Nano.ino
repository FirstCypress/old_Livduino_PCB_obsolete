/* 
 * LiVduino - Arduino Nano 
 *  
 * alfredc333 
 * 2017 
 * MIT license
 */ 

#include <Wire.h>
#include <RTClib.h>
#include "Adafruit_SHT31.h"
#include "TSL2561.h"
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include "livmsg.h"

/**********************************************************/
//UNCOMMENT NEXT LINE ONLY WHEN YOU DEBUG, this might interfere with messages sent to NodeMCU over Serial
//#define DBG

RTC_DS3231 rtc;
Adafruit_SHT31 sht31 = Adafruit_SHT31();
TSL2561 tsl(TSL2561_ADDR_FLOAT);
SSD1306AsciiWire oled;

//Alarm thresholds, alarm is triggered if threshold is exceeded
float T    = 100;
float H    = 100;
uint16_t L = 200;

//alarm masks for temperature, humidity and light
const byte tMask  = 0x01;
const byte hMask  = 0x02;
const byte lMask  = 0x04;
//alarm Status set to clear
byte  alarmStatus = 0x00;
//aSign is the alarm indication on OLED screen, it will show up before the measurement(s) that triggered the alarm
String aSign = "!";

//Buzzer
//buzzer sounds when alarm is triggered by measurements exceeding thresholds
const byte buzzerPin       = 12;
const byte buzzerButtonPin = 2;
int buzzerState            = LOW;

//Sonoff
const byte sonoffButtonPin = 5;
int sonoffState            = LOW;
//this is the mask for measurement type for which Sonoff will turn on when value is exceeded
//e.g. if you want Sonoff to switch on when temp threshold is exceeded, you should put tMask here
//default value here is light mask because it is very easy to do initial testing covering the light sensor 
const byte sonoffAlarmMask = lMask;

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

#ifdef DBG
  Serial.println("DS3231 init");
#endif
  if (! rtc.begin()) {
#ifdef DBG
    Serial.println("Couldn't find RTC");
#endif
    while (1);
  }

#ifdef DBG
  Serial.println("SHT31 init");
#endif
  if (! sht31.begin(0x44)) {
#ifdef DBG
    Serial.println("Couldn't find SHT31");
#endif
    while (1) delay(1);
  }

#ifdef DBG
  Serial.println("TL2561 init");
#endif
  if (tsl.begin()) {
#ifdef DBG
    Serial.println("Found TL2561 sensor");
#endif
  } else {
#ifdef DBG
    Serial.println("No TL2561 sensor found");
#endif
    while (1);
  }

  // TAKEN VERBATIM FROM TSL library example
  //You can change the gain on the fly, to adapt to brighter/dimmer light situations
  //tsl.setGain(TSL2561_GAIN_0X);         // set no gain (for bright situtations)
  tsl.setGain(TSL2561_GAIN_16X);      // set 16x gain (for dim situations)
  // Changing the integration time gives you a longer time over which to sense light
  // longer timelines are slower, but are good in very low light situtations!
  tsl.setTiming(TSL2561_INTEGRATIONTIME_13MS);  // shortest integration time (bright light)
  //tsl.setTiming(TSL2561_INTEGRATIONTIME_101MS);  // medium integration time (medium light)
  //tsl.setTiming(TSL2561_INTEGRATIONTIME_402MS);  // longest integration time (dim light)

#ifdef DBG
  Serial.println("SSD1306 init");
#endif
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
  //set time
  if (rtc.lostPower()) {
#ifdef DBG
    Serial.println("RTC lost power, let's set the time!");
#endif
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    //rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  while (1) {
    //get time
    String currentDate = "NOT_INIT";
    String currentTime = "NOT_INIT";
    //get time
    DateTime now = rtc.now();
    currentDate = now.year() + String('/') + now.month() + String('/') + now.day();
    currentTime = now.hour() + String(':') + now.minute() + String(':') + now.second();

    //get temp & humidity
    String temperature = "NOT_INIT";
    String humidity    = "NOT_INIT";
    //get t&h
    float t = sht31.readTemperature();
    float h = sht31.readHumidity();
#ifdef DBG
    Serial.println("------------");
#endif
    if (! isnan(t)) {  // check if 'is not a number'
#ifdef DBG
      Serial.print("Temp *C = "); Serial.println(t);
#endif
      temperature = String(t) + " C";
    } else {
#ifdef DBG
      Serial.println("Failed to read temperature");
#endif
      temperature = "ERROR";
    }
    if (! isnan(h)) {  // check if 'is not a number'
#ifdef DBG
      Serial.print("Hum. % = "); Serial.println(h);
#endif
      humidity = String(h) + " %";
    } else {
#ifdef DBG
      Serial.println("Failed to read humidity");
#endif
      humidity = "ERROR";
    }

    //get luminosity
    String light       = "NOT_INIT";
    uint16_t l = tsl.getLuminosity(TSL2561_VISIBLE);
    // Simple data read example. Just read the infrared, fullspecrtrum diode
    // or 'visible' (difference between the two) channels.
    // This can take 13-402 milliseconds! Uncomment whichever of the following you want to read
    //uint16_t l = tsl.getLuminosity(TSL2561_VISIBLE);
    //uint16_t x = tsl.getLuminosity(TSL2561_FULLSPECTRUM);
    //uint16_t x = tsl.getLuminosity(TSL2561_INFRARED);
    //Serial.println(x, DEC);
    light =  String(l, DEC) + " lux";

#ifdef DBG
    Serial.println(currentDate);
    Serial.println(currentTime);
    Serial.println(temperature);
    Serial.println(humidity);
    Serial.println(light);
#endif

    //display loop, you can use counter value to control how long is the report time for thingSpeak and Sonoff
    byte counter = 1;
    while (counter) {
      displayPageOne(currentDate, currentTime);
      //page one displaytime
      delay(5000);

      checkAlarms(t, h, l, alarmStatus);
      displayPageTwo(temperature, humidity, light, alarmStatus);
      //page two displaytime
      delay(5000);
      
      readButtons(buzzerState, sonoffState, thingSpeakState);
      displayPageThree(buzzerState, sonoffState, thingSpeakState);
      //page three displaytime
      delay(2000);
      
      counter -=1;
    }
    
    //flush Serial before sending messages to NodeMCU
    //DO NOT PRINT ANY DEBUG MESSAGES until communication with NodeMCU ends
    Serial.flush();
    delay(2000);

    //send messages to NodeMCU wrt. ThingSpeak
    if (thingSpeakState == 1 ) {
      livmsg lm;
      lm = {TS_REPORT, t, h, float(l)};
      Serial.write((byte*)&lm, sizeof(livmsg));
      Serial.flush();
      delay(2000);
    }

    //send messages to NodeMCU wrt. Sonoff
    if (sonoffState == 1 ) {
      if (alarmStatus & sonoffAlarmMask) {
        livmsg lm;
        lm = {SONOFF_ON, t, h, float(l)};
        Serial.write((byte*)&lm, sizeof(livmsg));
        Serial.flush();
        delay(2000);
      }
      else {
        livmsg lm;
        lm = {SONOFF_OFF, t, h, float(l)};
        Serial.write((byte*)&lm, sizeof(livmsg));
        Serial.flush();
        delay(2000);
      }
    }
  }
}


//checkAlarms
//alarms are triggered when value exceed threshold
//alarms are cleared when value is less than 90% of the threshold value
//e.g. a temp threshold of 30C triggers alarm when temp is higher than 30C
//alarm is cleared when temp is less than 27C
void checkAlarms(float t, float h, uint16_t l,byte &alarmStatus) {
  //set alarms  
  if (t > T)
    alarmStatus |= tMask;
  if (h > H)
    alarmStatus |= hMask;
  if (l > L)
    alarmStatus |= lMask;
  
  //clear alarms
  if (t < 0.9*T)
    alarmStatus &= ~tMask;
  if (h < 0.9*H)
    alarmStatus &= ~hMask;
  if (l < 0.9*L)
    alarmStatus &= ~lMask;

  //sound buzzer if there is an alarm
  if (alarmStatus != 0x00){
    digitalWrite(buzzerPin, HIGH);
  }else{
    digitalWrite(buzzerPin, LOW);
  }
}


//readButtons
void readButtons(int &buzzerState, int &sonoffState, int &thingSpeakState) {
  buzzerState = digitalRead(buzzerButtonPin);
  sonoffState = digitalRead(sonoffButtonPin);
  thingSpeakState = digitalRead(thingSpeakButtonPin);
}


//this page display logo and current time
void displayPageOne(String currentDate, String currentTime) {
  oled.clear();
  oled.println("LiVduino");
  oled.println("");
  oled.println(currentDate);
  oled.println(currentTime);
}


//this page shows current measurements
void displayPageTwo(String temperature, String humidity, String light, byte aStatus) {
  String ts = "";
  String hs = "";
  String ls = "";
  if (aStatus & tMask)
    ts = aSign;
  if (aStatus & hMask)
    hs = aSign;
  if (aStatus & lMask)
    ls = aSign;

  oled.clear();
  oled.println(ts + String("T ") + temperature);
  oled.println(hs + String("H ") + humidity);
  oled.println(ls + String("L ") + light);
}


//this page shows states for Buzzer, Sonoff and ThingSpeak
//if a button is pushed down, the state is ON
//when Buzzer state is OFF, you can not hear the buzzer sound. The alarm still shows on OLED.
//when Sonof state is ON, commands to turn on/off are sent to Sonoff
//when ThingSpeak state is ON, measurements are written to ThingSpeak 
void displayPageThree(int bState, int sState, int tsState) {
  String buzzerState     = "NOT_INIT";
  String sonoffState     = "NOT_INIT";
  String thingSpeakState = "NOT_INIT";

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

  oled.clear();
  oled.println("Buzzer: " + buzzerState);
  oled.println("Sonoff: " + sonoffState);
  oled.println("ThingS: " + thingSpeakState);
}

