/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#line 1 "c:/Users/jeric/Desktop/IoT/Midterm-SmartPlant/Midterm2SmartPlant/src/Midterm2SmartPlant.ino"
/*
 * Project Midterm2SmartPlant
 * Description:Smart Plant Watering System
 * Author: Jericho Ortiz
 * Date:11/9/2021
 */

#include "Adafruit_GFX.h"
#include "AdaFruit_SSD1306.h"
#include "Particle.h"

void setup();
void loop();
#line 12 "c:/Users/jeric/Desktop/IoT/Midterm-SmartPlant/Midterm2SmartPlant/src/Midterm2SmartPlant.ino"
#define OLED_RESET D4
Adafruit_SSD1306 display(OLED_RESET);

String DateTime, TimeOnly;
const int soilProbe = A0;
int val = 0;

void setup() {
Serial.begin(9600);
    pinMode(soilProbe,INPUT);
    analogRead(soilProbe);
    Time.zone(-7); //MST = -7, MDT = -6
    Particle.syncTime(); //Sync time w/ Particle Cloud
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.display();
    delay(2000);
    display.clearDisplay();
    display.drawPixel(10, 10, WHITE);
    display.display();
    delay(2000);
    display.clearDisplay();

}

void loop() {
DateTime = Time.timeStr(); // Current Date and Time from Particle Time class
  TimeOnly = DateTime.substring(11,19); // Current Date and Time from Particle Time class
  val = analogRead(soilProbe);
  Serial.println(val);
  Serial.printf("Date and time is %s\n", DateTime.c_str()); //%s prints an array of char
  Serial.printf("Time is %s\n",TimeOnly.c_str()); // the . c_str () method converts a String to an array of char
  display.clearDisplay();
  delay(2000);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Jericho");
  display.setTextColor(WHITE); 
  display.printf("%s\n",TimeOnly.c_str());
  display.println(val);
  display.display();
  delay(2000);
}