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
