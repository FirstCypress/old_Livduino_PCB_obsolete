/* LiVduino
 * alfredc333
 * May, 2017 
 * MIT License
 */

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>

/**********************************************************/

int displayTime = 4000; 

String currentDate = "NOT_INIT";
String currentTime = "NOT_INIT";


String ppmString =" ppm";
int co2Level = 0;
int rc = 1;
String co2Value = "NOT_INIT";


LiquidCrystal_I2C lcd(0x27,16,2); // set the LCD address to 0x27 for a 16 chars and 2 line display
RTC_DS3231 rtc;

/*********************************************************/
void setup()
{
  
  Serial.begin(9600);

  lcd.init(); 
  lcd.backlight(); //turn on backlight 
}

/*********************************************************/
void loop() 
{

  char status;
  //get time stamp
    if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
     rtc.adjust(DateTime(2017, 6, 25, 3, 20, 30));
  }
  
  DateTime now = rtc.now();
  currentDate = String("Date: ") + now.year() + String('/') + now.month() + String('/') + now.day();
  currentTime = String("Time: ") + now.hour() + String(':') + now.minute() + String(':') + now.second();
  
  //display date and time
  lcd.clear();
  lcd.setCursor(0,0); // set the cursor to column 0, line 0
  lcd.print(currentDate);
  lcd.setCursor(0,1); // set the cursor to column 0, line 1
  lcd.print(currentTime);
  delay(displayTime);
 
}

