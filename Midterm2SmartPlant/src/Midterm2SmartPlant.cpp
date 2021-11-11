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
#include "Wire.h"
#include "Adafruit_BME280.h"
#include "math.h"
#include "Grove_Air_quality_Sensor.h"

void setup();
void loop();
String getAirQuality();
void printValues();
void getDustSensorReadings();
#line 17 "c:/Users/jeric/Desktop/IoT/Midterm-SmartPlant/Midterm2SmartPlant/src/Midterm2SmartPlant.ino"
const int DustSensor = D5;
const unsigned long SensorReadInterval = 30000;
const int OLED_RESET = D4;

const int AirQ = A3;
AirQualitySensor aqSensor(AirQ);

Adafruit_SSD1306 display(OLED_RESET);
Adafruit_BME280 bme;

String DateTime, TimeOnly;
const int soilProbe = A0;
const int hexAddress = 0x76;
int val = 0;

const int pumpPin = D11;

bool bmeStatus;
unsigned long delayTime; 
unsigned long lastInterval;
unsigned long lowpulseoccupancy = 0;
unsigned long last_lpo = 0;
unsigned long duration;

float tempf;
float tempC; 
float pressPA; 
float humidRH; 

float ratio = 0;
float Dust = 0;


void setup() { //                      BEGINNING OF VOID SETUP 
    Serial.begin(9600);
    pinMode(pumpPin, OUTPUT);
    pinMode(DustSensor, INPUT);
    lastInterval = millis();
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
    
  if(aqSensor.init()) {
    Serial.println("Air Quality Sensor ready,");
  }
  else {
    Serial.println("Air Quality Sensor ERROR!");
   }
  }

void loop() { //                          BEGINNING OF VOID LOOP 
  digitalWrite(pumpPin, HIGH);
  delay(1000);
  digitalWrite(pumpPin, LOW);

   duration = pulseIn(DustSensor, LOW);
  lowpulseoccupancy = lowpulseoccupancy + duration;
  if((millis() - lastInterval) > SensorReadInterval) {
     getDustSensorReadings();
      lowpulseoccupancy = 0;
      lastInterval = millis();
      String quality = getAirQuality();
      Serial.printlnf("Air Quality: %s", quality.c_str());
   }
  String quality = getAirQuality();
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
   display.printf("%s\n", DateTime.c_str()); //Start of OLED display details. 
   display.printf("Soil = %i", val);
   display.println();
   display.printf("Temp: %.2f\n",tempf);
   display.printf("Hum: %.2f\n",humidRH);
   display.printlnf("Dust: %f", Dust);
   display.printlnf("Air Quality: %s", quality.c_str());
   display.display();
   delay(5000);
}

String getAirQuality() {
  int quality = aqSensor.slope();
  String qual = "None";

if(quality == AirQualitySensor::FORCE_SIGNAL) {
  qual = "Danger";
}
else if(quality == AirQualitySensor::HIGH_POLLUTION) {
  qual = "High Pollution";
}
else if(quality == AirQualitySensor::LOW_POLLUTION) {
  qual = "Low Pollution";
}
else if(quality == AirQualitySensor::FRESH_AIR) {
  qual = "Fresh Air";
}
return qual;
}
  
 void printValues() {
    Serial.print("Temp:");
    Serial.print(tempf);
    Serial.println();
    Serial.print("Hum:");
    Serial.print(bme.readHumidity());
    Serial.println("%");
}

void getDustSensorReadings() {
if(lowpulseoccupancy == 0) {
  lowpulseoccupancy = last_lpo;
}
else {
  last_lpo = lowpulseoccupancy;
}
  ratio = lowpulseoccupancy / (SensorReadInterval * 10.0);
  Dust = 1.1 * pow(ratio, 3) - 3.8 * pow(ratio, 2) + 520 * ratio + 0.62;
  // Serial.printlnf("LPO: %i", lowpulseoccupancy);
  // Serial.printlnf("Ratio: %f%%", ratio);
  Serial.printlnf("Dust: %f", Dust);
}