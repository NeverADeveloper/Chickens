/* Light Level Monitor
 *  by Andrew Westfold. (c) April 2016
 *  Uses an i2c RTC, i2c LCD display, Light-dependant Resistor (LDR) and an SD card 
 *  Takes a light sample every n minutes for m hours.
 * Note: this runs entirely in void setup() to prevent in running continuously!
 *  Saves the data to a .csv file on SD card
 *  
 * Connections:
 *  SD Card
 *				Nano		SD Card
 *    D10 - CS
 *    D11 - MOSI
 *    D12 - MISO
 *    D13 - SCK
 *    VCC - VCC
 *    GND - GND
 *  i2c RTC
 *    Nano		RTC
 *    GND - GND
 *    VCC - VCC
 *    A4 -  SDA
 *    A5 -  SCL
 *  LDR 
 *    5v ---0--- A0 ---10k--- GND
 * 
 * Set runTime to adjust running time (Time to run = runTime * sampleRate)
 * Set sampleRate to adjust sample time
 *
 * NOTE: Runs fine when Arduino Nano connected to Laptop via USB, but SD card doesnt work from batteries.
 *       Potential 3.3v/5v supply issue?
 *
 * Could comment out all Serial. statements when running 'off-laptop'
*/

#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#define rtc_I2C_Addr 0x68 //check with i2c_Scanner.ino
int ldrPin = A0;
int ldrValue = 0;
int ldrMaxValue = 700;
int ldrMinValue = 700;
int sampleRate = 600000; // 300000 = sample once every 5 mins 900000 = 15 mins
int runTime = 144; //Calculated by Time to run divided by sample rate
File myfile;

byte bcdToDec(byte val)// Convert binary coded decimal to decimal
{
     return((val/16*10) + (val%16));
}

void readRTCtime
(byte *second,byte *minute,byte *hour,byte *dayOfWeek,byte *dayOfMonth,byte *month,byte *year)
{
     Wire.beginTransmission(rtc_I2C_Addr);
     Wire.write(0); //set DS3231 register pointer to 00h
     Wire.endTransmission();
     Wire.requestFrom(rtc_I2C_Addr, 7);//request 7 bytes of RTC data starting loc 00h
          *second = bcdToDec(Wire.read() & 0x7f);
          *minute = bcdToDec(Wire.read());
          *hour = bcdToDec(Wire.read() & 0x3f);
          *dayOfWeek = bcdToDec(Wire.read());
          *dayOfMonth = bcdToDec(Wire.read());
          *month = bcdToDec(Wire.read());
          *year = bcdToDec(Wire.read());
}

void setup()
{
     Wire.begin();
     Serial.begin(9600);
     pinMode(ldrPin, INPUT);
     pinMode(10, OUTPUT);
     if (!SD.begin(10)) 
     {
          Serial.println("Initialization of the SD card failed!");
          return;
     }
     myfile = SD.open("light.csv", FILE_WRITE);
     if (myfile)
     Serial.println("Writing Header ");
     myfile.print("Time:");
     myfile.print(",");
     myfile.println("Current value:");
     Serial.println("   Header written!");     
     myfile.close();
     for (int t = 0; t <= runTime; t = t + 1)
     {
          byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
          readRTCtime(&second,&minute,&hour,&dayOfWeek,&dayOfMonth,&month,&year); //Read time
          ldrValue = analogRead(A0); //Read light level
          if (ldrValue >= ldrMaxValue)//Rewrite the Max light level if higher
          {
               ldrMaxValue = ldrValue;
          }  
          else if (ldrValue <= ldrMinValue)//Rewrite the Min light level if lower
          {
               ldrMinValue = ldrValue;
          }
          myfile = SD.open("light.csv", FILE_WRITE);//Open file and write time and light level
          myfile.print(hour, DEC);
          myfile.print(":");
          if (minute<10)
          {
               myfile.print("0");
          }
          myfile.print(minute, DEC);
          myfile.print(",");
          myfile.println(ldrValue);
          Serial.print("   Writing Iteration ");
          Serial.println(t);
          myfile.close();
          delay(sampleRate); // delay(900000); = samples once every 5 mins
     }
     myfile = SD.open("light.csv", FILE_WRITE); //After measuring, write Max and Min values to file
     Serial.println("   Writing Min and Max values");
     myfile.print("Minimum = ");
     myfile.print(",");
     myfile.print(ldrMinValue);
     myfile.print(",");
     myfile.print("Maximum = ");
     myfile.print(",");
     myfile.println(ldrMaxValue);
     myfile.close();
     Serial.println("All Data written to card!");   //Indicate that all is complete
}

void loop()
{
}
