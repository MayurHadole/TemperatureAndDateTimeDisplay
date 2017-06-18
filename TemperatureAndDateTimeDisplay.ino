/*  FILE          : TemperatureAndDateTimeDisplay.ino
 *  PROGRAMMER    : Mayur Hadole (5783)
 *  FIRST VERSION : 2016-06-23
 *  DESCRIPTION   :
 *        This program reads temperature from temperature sensor
 *     and displays it on Liquid Crystal Display and Serial port
 *     in Celsius and in Fahrenheit.
 *          This program also illuminates RGB LED with specific
 *     color for specific temperature range.
 *
 *     Temperature Range     Color
 *
 *     Below -15             Green
 *     -15 to 5.0            Blue
 *     5.1 to 15.0           Yellow
 *     15.1 to 25.0          Orange
 *     Above 25.1            Red
 *
 *    Also,Time and Date is displayed on the second row of
 *    the LCD after the unix time in seconds is entered on
 *    serial port.
 *    for unix timestamp converter visit to the link
 *    http://www.unixtimestamp.com/
 *    
 *        This code is tested on Teensy 3.1 board
 */


// include the LCD library code:
#include <LiquidCrystal.h>
// include the Time Library code:
#include <Time.h>




/* LCD RS pin to digital pin 12
   LCD Enable pin to digital pin 11
   LCD D4 pin to digital pin 5
   LCD D5 pin to digital pin 4
   LCD D6 pin to digital pin 3
   LCD D7 pin to digital pin 2
   LCD R/W pin to ground
   LCD VSS pin to ground
   LCD VCC pin to 5V */
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

//Number Of Columns In LCD are 2
const uint8_t NumberOfColumnsInLCD = 2;
//Number Of Rows In LCD are 16
const uint8_t NumberOfRowsInLCD = 16;
//first row on LCD
const uint8_t firstRowOfLCD = 0;
//second row on LCD
const uint8_t secondRowOfLCD = 1;
//first Column of LCD
const uint8_t firstColumnOfLCD = 0;
//Baud rate for serial communication
const uint16_t SerialBaudRate = 9600;
//wait for 2 seconds
const uint16_t delayOf2Seconds = 2000;
//wait for 1 second
const uint16_t delayOf1Second = 1000;

//Red pin of RGB LED is connected to pin 23 of teensy
const uint8_t redLedPin = 23;
//Green pin of RGB LED is connected to pin 22 of teensy
const uint8_t greenLedPin = 22;
//Blue pin of RGB LED is connected to pin 21 of teensy
const uint8_t blueLedPin = 21;
//Temperature Sensor's Output pin is connected to analog
//input 0 i.e. A0 of Teensy.
const uint8_t temperatureSensorPin = 0;

//Analog reference voltage of Teensy's A2D converter
const float analogReferanceOfA2D = 3.3;

//output voltage of temperature sensor at O degree Celsius
const float voltageAt0Degrees = 0.5;

//Temperature Co-efficient of temperature sensor in volt per
// degree celsius
const float temperatureCoefficient = 0.01;

//Resolution of A2D converter is 10 bits which means 1024
//values.
const float resolution = 1024.0;

//counter to take average of 5000 temperature readings from
//sensor.
const uint16_t numberOfSamples = 5000;

//Following are the various brightness levels to make
//different colors on RGB LED.
const uint8_t highestBrightness = 255;
const uint8_t brightnessOf78Percent = 200;
const uint8_t brightnessOf32Percent = 80;
const uint8_t brightnessOf8Percent = 20;
const uint8_t noBrightness = 0;
const uint8_t BrightnessOf70percent = 175;

//code of degree symbol for LCD Display
const uint8_t degreeSymbolForLCD = 223;
//code of degree symbol in Windows-1252 character set for serial
//port.
const uint8_t degreeSymbolCode = 176;

//to display float variable with only one decimal point 
const uint8_t oneDecimalPoint = 1;
//to display float variable with no decimal point 
const uint8_t noDecimalPoint = 0;

//two thousand years
const uint16_t twoThousandYears = 2000;

//ten seconds or minutes
const uint8_t tenSecondsOrMinutes = 10;

// Length of unix time in seconds is 10;
const uint8_t unixTimeLength = 10;

//Boolean variable to know if time is synced
//or not synced by using user entered unix time.
//It is set to "false" because at the start of the
//execution, time will not be set.
//its defined globally because it needs to be
//initialized only once.
boolean timeIsSet = false;

//Prototypes of the functions used in this program
float getTemperature();
float celsiusToFahrenheitConversion( float temperatureInCelsius);
void RGBLedIndicator(float temperatureInCelsius);
void printTemperature0nLcd(float temperatureInCelsius);
void printTemperatureOnSerialPort(float temperatureInCelsius );
void makeColor (uint8_t red , uint8_t green , uint8_t blue);
void turnLedOnWithRedColor();
void turnLedOnWithGreenColor();
void turnLedOnWithBlueColor();
void turnLedOnWithOrangeColor();
void turnLedOnWithYellowColor();
void digitalClock();
void printDigits(uint8_t digits);



// FUNCTION     : getTemperature
// DESCRIPTION  :
//      This fuctions reads analog signal from temperature sensor
//   pin and converts digitally converted signal into Temperature
// PARAMETERS   :  void
// RETURNS      :  float: TemperatureinCelsius
//          returns temperature in celsius
float getTemperature()
{
  //To store the output of A2D converter.
  float outputOfSensor;
  //To Store Temperature in Celsius.
  float temperatureInCelsius = 0;
  //This For loop is used to improve accuracy of temperature
  //reading by averaging 5000 sample readings of temperature
  //sensor.
  for ( uint16_t sample = 0; sample <= numberOfSamples ; sample++)
  {
    //Stores the output of A2D converter into the variable.It is
    //in the range of 0 to 1023.
    outputOfSensor = analogRead(temperatureSensorPin);
    //this converts the value into voltage by mutiplying value
    //returned by analogRead to the voltage equaling to one step
    //of A2D converter. which is, 3.3/1024.0
    outputOfSensor =
      (outputOfSensor * ( analogReferanceOfA2D / resolution));
    //Calibrating the sensor by getting difference between output
    //voltage of sensor and voltage at 0 degree(0.5 volts).
    //The result is linearly dependent on temperature.
    outputOfSensor = (outputOfSensor - voltageAt0Degrees);
    //converts the voltage into temperature in celsius by dividing
    //voltage by temperature co-efficient which is 0.01 volts per
    //degree celsius.
    outputOfSensor = (outputOfSensor / temperatureCoefficient);
    //Adding all temperature readings to take average
    temperatureInCelsius = outputOfSensor + temperatureInCelsius;
  }
  //taking average
  temperatureInCelsius = temperatureInCelsius / numberOfSamples;
  //wait for 1 seconds.
  delay(delayOf1Second);
  //returning the averaged temperature in celsius
  return temperatureInCelsius;
}

// FUNCTION     : RGBLedIndicator
// DESCRIPTION  :
//   This function illuminates RGB LED with specific
//    color for specific temperature range.
//
//    Temperature Range     Color
//
//    Below -15             Green
//    -15 to 5.0            Blue
//    5.1 to 15.0           Yellow
//    15.1 to 25.0          Orange
//    Above 25.1            Red
//  
// PARAMETERS   :  float: TemperatureinCelsius
//       Temperature in celsius
// RETURNS      : void 

void RGBLedIndicator(float temperatureInCelsius)
{
  //if temperature in celsius is between -15 degrees and 5 degree
  //celsius then
  //Temperature ranges are not defined as constants purposely
  //because its easy to change values here rather than in constants
  if (temperatureInCelsius >= -15.0 && temperatureInCelsius <= 5.0)
  {
    //turns RGB LED ON with blue color
    turnLedOnWithBlueColor();
  }
  //if temperature in celsius is between 5.1 degrees and 15 degrees
  // celsius then
  else if (temperatureInCelsius >= 5.1 && temperatureInCelsius <= 15.0)
  {
    //turns RGB LED ON with Yellow color
    turnLedOnWithYellowColor();
  }
  //if temperature in celsius is between 15.1 degrees and 25 degrees
  // celsius then
  else if (temperatureInCelsius >= 15.1 && temperatureInCelsius <= 25.0)
  {
    //turns RGB LED ON with Orange color
    turnLedOnWithOrangeColor();
  }
  //if temperature in celsius is above 25 degrees then
  else if (temperatureInCelsius >= 25.0 )
  {
    //turns RGB LED ON with Red color
    turnLedOnWithRedColor();
  }
  //if temperature in celsius is below -15 degrees then
  else if (temperatureInCelsius < -15.0)
  {
    //turns RGB LED ON with Green color
    turnLedOnWithGreenColor();
  }
}

// FUNCTION     : celsiusToFahrenheitConversion
// DESCRIPTION  :
//      This fuctions converts temperature in celsius into
//  temperature in fahrenheit.
// PARAMETERS   :
//     float temperatureInCelsius : gets temperature in celsius
// RETURNS      :
//     float temperatureInFahrenheit : returns temperature
//                                     in fahrenheit

float celsiusToFahrenheitConversion( float temperatureInCelsius)
{
  //variable to store temperature in fahrenheit
  float temperatureInFahrenheit;
  //formula to convert celsius into fahrenheit
  // "* 1.8 + 32 " is not defined as constant because
  //1.8 needs to be multipled first and then 32 is added.
  //Also formula is never going to change.
  temperatureInFahrenheit = temperatureInCelsius * 1.8 + 32;
  return temperatureInFahrenheit;
}

// FUNCTION     : printTemperature0nLcd
// DESCRIPTION  :
//      This fuctions prints temperature in celsius and in
//   fahrenheit on Liquid Crystal Display.
// PARAMETERS   :
//     float temperatureInCelsius : get temperature in celsius
// RETURNS      : void

void printTemperature0nLcd(float temperatureInCelsius)
{
  //variable to store temperature in fahrenheit
  float temperatureInFahrenheit;
  //converts temperature in celsius into temperature in fahrenheit.
  //and stores in variable.
  temperatureInFahrenheit
    = celsiusToFahrenheitConversion(temperatureInCelsius);
  // set cursor position to start of first line on the LCD
  lcd.setCursor( firstColumnOfLCD , firstRowOfLCD );
  // prints temperature in celsius on LCD with only one decimal
  //point.
  lcd.print(temperatureInCelsius, oneDecimalPoint);
  //prints the degree symbol on LCD
  lcd.print((char)degreeSymbolForLCD);
  //prints "C" and White space on LCD
  lcd.print("C      ");
  //prints temperature in fahrenheit on LCD without decimal point
  lcd.print(temperatureInFahrenheit, noDecimalPoint);
  //prints the degree symbol on LCD
  lcd.print((char)degreeSymbolForLCD);
  //prints "F" on LCD
  lcd.print("F");
}

// FUNCTION     : printTemperature0nSerialPort
// DESCRIPTION  :
//      This fuctions prints temperature in celsius and in
//   fahrenheit on Serial Moniter.
// PARAMETERS   :
//     float temperatureInCelsius : get temperature in celsius
// RETURNS      : void

void printTemperatureOnSerialPort(float temperatureInCelsius )
{
  //variable to store temperature in fahrenheit
  float temperatureInFahrenheit;
  //converts temperature in celsius into temperature in fahrenheit.
  //and stores in variable.
  temperatureInFahrenheit
    = celsiusToFahrenheitConversion(temperatureInCelsius);
  //prints "temperature =" on serial moniter
  Serial.print("Temperature = ");
  // prints temperature in celsius on serial moniter with only
  //one decimal point.
  Serial.print(temperatureInCelsius, oneDecimalPoint);
  //prints the degree symbol on serial moniter
  Serial.write(degreeSymbolCode);
  //prints "C" and white space on serial moniter
  Serial.print("C    ");
  //prints temperature in fahrenheit on serial moniter without
  //decimal point.
  Serial.print(temperatureInFahrenheit, noDecimalPoint);
  //prints the degree symbol on serial moniter
  Serial.write(degreeSymbolCode);
  //prints "F" on serial moniter and then a new line
  Serial.println("F ");
}

// FUNCTION     : makeColor
// DESCRIPTION  :
//      This fuctions is used to illuminate RGB LED with
//   different colours by selecting brightness of red, green and
//   blue.
// PARAMETERS   :
//       uint8_t red  : to set intensity of red in RGB LED
//                      from 0 to 255.
//       uint8_t gren : to set intensity of green in RGB LED
//                      from 0 to 255.
//       uint8_t blue : to set intensity of blue in RGB LED
//                      from 0 to 255.
//
// RETURNS      : void

void makeColor (uint8_t red , uint8_t green , uint8_t blue)
{
  //sends the PWM wave on red led pin whose ON time Width
  //depends on intensity passed in "red" parameter
  analogWrite( redLedPin , red );
  //sends the PWM wave on green led pin whose ON time Width
  //depends on intensity passed in "green" parameter
  analogWrite( greenLedPin , green );
  //sends the PWM wave on blue led pin whose ON time Width
  //depends on intensity passed in "blue" parameter
  analogWrite( blueLedPin , blue);
}

// FUNCTION     : turnLedOnWithRedColor
// DESCRIPTION  :
//      This fuction calls makeColor() function with intensities
//   of red,green and blue as parameters to illuminate RGB
//   LED with RED color
// PARAMETERS   : void
// RETURNS      : void

void turnLedOnWithRedColor()
{
  //sets intensity of red at 100% and others to zero to make
  //bright red color on RGB LED
  makeColor( highestBrightness , noBrightness , noBrightness);
}

// FUNCTION     : turnLedOnWithGreenColor
// DESCRIPTION  :
//      This fuction calls makeColor() function with intensities
//   of red,green and blue as parameters to illuminate RGB
//   LED with green color
// PARAMETERS   : void
// RETURNS      : void

void turnLedOnWithGreenColor()
{
  //sets intensity of green at 100% and others to zero to make
  //bright green color on RGB LED
  makeColor( noBrightness , highestBrightness , noBrightness);
}

// FUNCTION     : turnLedOnWithBlueColor
// DESCRIPTION  :
//      This fuction calls makeColor() function with intensities
//   of red,green and blue as parameters to illuminate RGB
//   LED with Blue color
// PARAMETERS   : void
// RETURNS      : void

void turnLedOnWithBlueColor()
{
  //sets intensity of blue at 100% and others to zero to make
  //bright blue color on RGB LED
  makeColor( noBrightness , noBrightness , highestBrightness);
}

// FUNCTION     : turnLedOnWithOrangeColor
// DESCRIPTION  :
//      This fuction calls makeColor() function with intensities
//   of red,green and blue as parameters to illuminate RGB
//   LED with Orange color
// PARAMETERS   : void
// RETURNS      : void

void turnLedOnWithOrangeColor()
{
  //sets the intensity of red at 78%,green at 8% and blue at 0%
  //to make Orange color on RGB LED.
  makeColor( brightnessOf78Percent , brightnessOf8Percent , noBrightness);
}

// FUNCTION     : turnLedOnWithYellowColor
// DESCRIPTION  :
//      This fuction calls makeColor() function with intensities
//   of red,green and blue as parameters to illuminate RGB
//   LED with Yellow color
// PARAMETERS   : void
// RETURNS      : void

void turnLedOnWithYellowColor()
{
  //sets the intensity of red at 100%,green at 32% and blue at 0%
  //to make Yellow color on RGB LED.
  makeColor( highestBrightness , brightnessOf32Percent , noBrightness);

}

// FUNCTION     : digitalCloack
// DESCRIPTION  :
//      This fuction prints time and date on the second
//   row of the lcd. Date is shown in DD MM YY format.
//   time is shows in 24 hour formate (HH MM SS).
// PARAMETERS   : void
// RETURNS      : void
void digitalClock()
{
  //set cursor to the second row of the LCD.
  lcd.setCursor( firstColumnOfLCD , secondRowOfLCD);
  //prints the hours on the LCD.
  //hour() is the time libtary function which returns
  //hours in integer value.
  lcd.print(hour());
  //prints the minutes on the LCD.
  //minute() is the time libtary function which returns
  //minutes in integer value.
  printDigits(minute());
  //prints the seconds on the LCD.
  //second() is the time libtary function which returns
  //seconds in integer value.
  printDigits(second());
  // prints a space between time and date.
  lcd.print(" ");
  //prints the day on the LCD.
  //day() is the time libtary function which returns
  //day in integer value.
  lcd.print(day());
  //prints space on the LCD
  lcd.print(" ");
  //prints the month on the LCD.
  //month() is the time libtary function which returns
  //month in integer value.
  lcd.print(month());
  //prints space on the LCD
  lcd.print(" ");
  //prints the month on the LCD.
  //year() is the time libtary function which returns
  //year in integer value.
  //2000 is substracted to make the year integer value
  //in two digits.
  lcd.print(year() - twoThousandYears);
}

// FUNCTION     : printDigits
// DESCRIPTION  :
//      This fuction prints minutes or seconds on lcd
//    depending upon the parameter passed.This function
//    also includes the "0" in from of minutes or
//    seconds if they are less than 10.
// PARAMETERS   :
//       uint8_t digits: passes the minutes or seconds in
//                       integer values.
// RETURNS      : void
void printDigits(uint8_t digits)
{
  //prints colon on the LCD
  lcd.print(":");
  //if the passed value(minutes or seconds) is less than
  //10 then
  if (digits < tenSecondsOrMinutes)
  {
    //print "o" first then
    lcd.print('0');
  }
  //print the value(minutes or seconds).
  lcd.print(digits);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(SerialBaudRate);
  //wait for Serial port to initialize
  delay(delayOf2Seconds);
  //sets the red led pin as an output
  pinMode(redLedPin, OUTPUT);
  //sets the green led pin as an output
  pinMode(greenLedPin, OUTPUT);
  //sets the blue led pin as an output
  pinMode(blueLedPin, OUTPUT);

  // set up the LCD's number of columns and rows:
  lcd.begin(NumberOfRowsInLCD, NumberOfColumnsInLCD);
  //time library of arduino demands the unix time
  //in seconds. So we will get than from user using
  //serial port.
  Serial.print("Enter Unix Time in seconds");


}


void loop()
{
  // put your main code here, to run repeatedly:
  //if time is not set.
  if (!timeIsSet)
  {
    //if data available on serial port is equal
    // to the length of unix time in seconds
    //i.e. 10.
    if (Serial.available() == unixTimeLength)
    {
      //variable to save user entered unix time.
      unsigned long unixTime;
      //get the unix time from serial port
      //to the variable
      unixTime = Serial.parseInt();
      // this is the time library function
      //which sets the time for the functions
      //day(),month(),year(),hour(),minute()
      //and second() using the passed unix
      //time in seconds.
      setTime(unixTime);
      //prints the space and newline on serial port
      Serial.println(" ");
      //shows the notification on the serial port
      //as "time is synced"
      Serial.println("Time is Synced");
      //time is set so, the boolean variable is made true.
      timeIsSet = true;
    }
  }
  //if time is set then
  if (timeIsSet)
  {
    //variable to store temperature in celsius
    float temperatureInCelsius;
    //Get the averaged temperature from 5000 sample readings
    //to variable
    temperatureInCelsius = getTemperature();
    //print temperature in celsius and fahrenheit on Serial moniter
    printTemperatureOnSerialPort(temperatureInCelsius);
    //print temperature in celsius and fahrenheit on LCD.
    printTemperature0nLcd( temperatureInCelsius);
    //turns the RGB led ON with different colours depending on 
    //temperature ranges.
    RGBLedIndicator(temperatureInCelsius);
    //prints the date and time on the second row of
    //the LCD.
    digitalClock();
  }
}
