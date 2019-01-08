/* LiVduino
*
*   alfredc333
*   May, 2017
*   MIT License
*/

#include <Wire.h>
#include <RTClib.h>
#include "Adafruit_SHT31.h"
#include "TSL2561.h"
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

/**********************************************************/

RTC_DS3231 rtc;
Adafruit_SHT31 sht31 = Adafruit_SHT31();
TSL2561 tsl(TSL2561_ADDR_FLOAT);
SSD1306AsciiWire oled;

const byte buzzerPin = 12;
const byte buttonPin = 2;
int bState = LOW;

float T = 100;
float H = 100;
uint16_t X = 10000;

const byte tMask  = 0x01;
const byte hMask  = 0x02;
const byte lMask  = 0x04;
byte  alarmStatus = 0x00;
String aSign = ">";



/*********************************************************/
void setup()
{
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
    Serial.println("Found sensor");
  } else {
    Serial.println("No sensor?");
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

  pinMode(buzzerPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
}

/*********************************************************/
void loop()
{
  //set time
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    //rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

  }

  while (1) {

    String currentDate = "NOT_INIT";
    String currentTime = "NOT_INIT";

    String temperature = "NOT_INIT";
    String humidity    = "NOT_INIT";
    String light       = "NOT_INIT";

    //get time
    DateTime now = rtc.now();
    currentDate = now.year() + String('/') + now.month() + String('/') + now.day();
    currentTime = now.hour() + String(':') + now.minute() + String(':') + now.second();

    //get t&h
    float t = sht31.readTemperature();
    float h = sht31.readHumidity();

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
    // Simple data read example. Just read the infrared, fullspecrtrum diode
    // or 'visible' (difference between the two) channels.
    // This can take 13-402 milliseconds! Uncomment whichever of the following you want to read
    uint16_t x = tsl.getLuminosity(TSL2561_VISIBLE);
    //uint16_t x = tsl.getLuminosity(TSL2561_FULLSPECTRUM);
    //uint16_t x = tsl.getLuminosity(TSL2561_INFRARED);
    Serial.println(x, DEC);

    light =  String(x, DEC) + " lux";

    byte aStatus = checkAlarms(t, h, x);

    Serial.println(currentDate);
    Serial.println(currentTime);
    Serial.println(temperature);
    Serial.println(humidity);
    Serial.println(light);


    displayPageOne(temperature, humidity, light, aStatus);
    delay(5000);
    bState = readButtons();
    displayPageTwo(currentDate, currentTime, bState);
    delay(5000);
  }
}

int readButtons() {
  int state = digitalRead(buttonPin);
  return state;
}

//page one shows current measurements
void displayPageOne(String temperature, String humidity, String light, byte aStatus) {
  oled.clear();
  String ts = "";
  String hs = "";
  String ls = "";
  if (aStatus & tMask)
    ts = aSign;
  if (aStatus & hMask)
    hs = aSign;
  if (aStatus & lMask)
    ls = aSign;

  oled.println(ts + String("T ") + temperature);
  oled.println(hs + String("H ") + humidity);
  oled.println(ls + String("L ") + light);
}

//page two shows current time and alarm status for
//buzzer and sonoff
void displayPageTwo(String currentDate, String currentTime, int bState) {
  String buzzerState = "NOT_INIT";
  if (bState == 1) {
    buzzerState = "ON";
  }
  else {
    buzzerState = "OFF";
  }

  oled.clear();
  oled.println(currentDate);
  oled.println(currentTime);
  oled.println(String("Buzzer: ") + buzzerState);
  oled.println("Sonoff: on");
}

byte checkAlarms(float t, float h, uint16_t x) {
  if ( (t > T) || (h > H) || (x > X) ) {
    digitalWrite(buzzerPin, HIGH);
  }
  else {
    digitalWrite(buzzerPin, LOW);
  }
  byte status = 0x00;
  if (t > T)
    status |= tMask;
  if (h > H)
    status |= hMask;
  if (x > X)
    status |= lMask;
  return status;
}

