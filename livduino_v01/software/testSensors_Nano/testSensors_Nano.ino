/*
   LiVduino - Arduino Nano

   alfredc333
   2017
   MIT license
*/

//test program
//test LiVduino sensor

#include <Wire.h>
#include <RTClib.h>
#include "Adafruit_SHT31.h"
#include "TSL2561.h"
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"


RTC_DS3231 rtc;
Adafruit_SHT31 sht31 = Adafruit_SHT31();
TSL2561 tsl(TSL2561_ADDR_FLOAT);
SSD1306AsciiWire oled;

/*********************************************************/
void setup()
{
  //set serial port for communication with NodeMCU
  Serial.begin(9600);
  while (!Serial) {
    delay(10);
  }

  Serial.println("DS3231 init");
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  Serial.println("SHT31 init");
  if (! sht31.begin(0x44)) {
    Serial.println("Couldn't find SHT31");
    while (1) delay(1);
  }

  Serial.println("TL2561 init");
  if (tsl.begin()) {
    Serial.println("Found TL2561 sensor");
  } else {
    Serial.println("No TL2561 sensor found");
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

  Serial.println("SSD1306 init");
  oled.begin(&Adafruit128x64, 0x3C);
  oled.setFont(System5x7);
  oled.set2X();

}

/*********************************************************/
void loop()
{
  //set time
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
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
    Serial.println("------------");
    if (! isnan(t)) {  // check if 'is not a number'
      Serial.print("Temp *C = "); Serial.println(t);
      temperature = String(t) + " C";
    } else {
      Serial.println("Failed to read temperature");
      temperature = "ERROR";
    }
    if (! isnan(h)) {  // check if 'is not a number'
      Serial.print("Hum. % = "); Serial.println(h);
      humidity = String(h) + " %";
    } else {
      Serial.println("Failed to read humidity");
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

    Serial.println(currentDate);
    Serial.println(currentTime);
    Serial.println(temperature);
    Serial.println(humidity);
    Serial.println(light);
    displayPageOne(currentDate, currentTime);
    //page one displaytime
    delay(5000);


    displayPageTwo(temperature, humidity, light);
    //page two displaytime
    delay(5000);
  }

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
void displayPageTwo(String temperature, String humidity, String light) {
  String ts = "";
  String hs = "";
  String ls = "";

  oled.clear();
  oled.println(ts + String("T ") + temperature);
  oled.println(hs + String("H ") + humidity);
  oled.println(ls + String("L ") + light);
}


