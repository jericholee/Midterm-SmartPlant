/*
 * Project Midterm2SmartPlant
 * Description:Smart Plant Watering System
 * Author: Jericho Ortiz
 * Date:11/9/2021
 */

#include "Adafruit_GFX.h"
#include "AdaFruit_SSD1306.h"
#include "Particle.h"
#include "Wire.h"
#include "Adafruit_BME280.h"
#define SEALEVELPRESSURE_HPA (1013.25)

#define OLED_RESET D4
Adafruit_SSD1306 display(OLED_RESET);

Adafruit_BME280 bme;

String DateTime, TimeOnly;
const int soilProbe = A0;
const int hexAddress = 0x76;
int val = 0;

bool bmeStatus;
unsigned long delayTime; 

float tempf;
float tempC; 
float pressPA; 
float humidRH; 


void setup() {
  Serial.begin(9600);
    pinMode(soilProbe,INPUT);
    analogRead(soilProbe);
    while (!Serial);// time to get serial running
    Serial.println(F("BME280 test")); 
    unsigned status;
    status = bme.begin();
      if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        Serial.print("SensorID was: 0x");
        Serial.println(bme.sensorID(), 16);
        Serial.print("ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        Serial.print("ID of 0x56-0x58 represents a BMP 280,\n");
        Serial.print("ID of 0x60 represents a BME 280.\n");
        Serial.print("ID of 0x61 represents a BME 680.\n");
      while (1);
}
  Serial.println("-- Default Test --");
  Serial.println();

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
   val = analogRead(soilProbe);
   tempC = bme.readTemperature();
   tempf = (tempC * 9.0/5.0) + 32.0;
   humidRH = bme.readHumidity();
   Serial.printf("%s\n", DateTime.c_str()); //%s prints an array of char
   printValues();
   delay(delayTime);
   display.clearDisplay();
   display.setTextSize(1);
   display.setTextColor(WHITE);
   display.setCursor(0,0);
   display.printf("%s\n", DateTime.c_str());
   display.printf("Soil = %i", val);
   display.println();
   display.printf("Temp: %.2f\n",tempf);
   display.printf("Hum: %.2f\n",humidRH);
   display.display();
   delay(5000);
}
  
 void printValues() {
    Serial.print("Temp:");
    Serial.print(tempf);
    Serial.println();
    Serial.print("Hum:");
    Serial.print(bme.readHumidity());
    Serial.println("%");
}

// void showTemp(void) {
//  display.clearDisplay();
//  display.setTextSize(1);
//  display.setTextColor(WHITE);
//  display.setCursor(0,0);
//  display.setRotation(0);
//  display.println(tempf, humidRH);
//  display.printf("tempature = %.2f\n, humidity = %.2f\n",tempf, humidRH);
//  display.display();
// delay(1000); 
// }