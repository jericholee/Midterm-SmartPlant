/*
 * Project Midterm2SmartPlant
 * Description:Smart Plant Watering System
 * Author: Jericho Ortiz
 * Date:11/9/2021
 */

#include "credentials.h"
#include "Adafruit_GFX.h"
#include "AdaFruit_SSD1306.h"
#include "Particle.h"
#include "Wire.h"
#include "Adafruit_BME280.h"
#include "math.h"
#include "Grove_Air_quality_Sensor.h"
#include <Adafruit_MQTT.h>
#include "Adafruit_MQTT/Adafruit_MQTT.h"
#include "Adafruit_MQTT/Adafruit_MQTT_SPARK.h"

TCPClient TheClient;

// >>>>>Declarations, Constants<<<<<<<//
const int DustSensor = D5;
const unsigned long SensorReadInterval = 30000;
const int OLED_RESET = D4;
const int AirQ = A3;
const int pumpPin = D11;
const int soilProbe = A0;
const int hexAddress = 0x76;
const int threshold = 3000; 
int moist = 0;
bool bmeStatus;

//MQTT//
unsigned long last, lastTime;
float Humidity;
float Tempature;
float SoilMoisture;
float DustRead;
int AirQuality;
int buttonVal;


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

AirQualitySensor aqSensor(AirQ);
Adafruit_SSD1306 display(OLED_RESET);
Adafruit_BME280 bme;
String DateTime, TimeOnly;
Adafruit_MQTT_SPARK mqtt(&TheClient,AIO_SERVER ,AIO_SERVERPORT, AIO_USERNAME,AIO_KEY);

Adafruit_MQTT_Publish humidObj = Adafruit_MQTT_Publish(&mqtt,AIO_USERNAME"/feeds/Humidity");
Adafruit_MQTT_Publish tempObj = Adafruit_MQTT_Publish(&mqtt,AIO_USERNAME"/feeds/Tempature");
Adafruit_MQTT_Publish SoilObj = Adafruit_MQTT_Publish(&mqtt,AIO_USERNAME"/feeds/Soil");
Adafruit_MQTT_Publish airQualObj = Adafruit_MQTT_Publish(&mqtt,AIO_USERNAME"/feeds/Air Quality");
Adafruit_MQTT_Publish dustObj = Adafruit_MQTT_Publish(&mqtt,AIO_USERNAME"/feeds/Dust Reading");
Adafruit_MQTT_Subscribe buttonObjs = Adafruit_MQTT_Subscribe (&mqtt,AIO_USERNAME"/feeds/buttonClicker");



void setup() { //                      BEGINNING OF VOID SETUP 
    Serial.begin(9600);
    pinMode(pumpPin, OUTPUT);
    pinMode(DustSensor, INPUT);
    lastInterval = millis();
    pinMode(soilProbe,INPUT);
    analogRead(soilProbe);
    waitFor(Serial.isConnected, 15000);
    WiFi.connect();
    while(WiFi.connecting()) {
     Serial.printf(".");  
  }  
    mqtt.subscribe(&buttonObjs);
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
  if(moist) {
   duration = pulseIn(DustSensor, LOW);
  }
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
   moist = analogRead(soilProbe);
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
   display.printf("Soil = %i", moist);
   display.println();
   display.printf("Temp: %.2f\n",tempf);
   display.printf("Hum: %.2f\n",humidRH);
   display.printlnf("Dust: %f", Dust);
   display.printlnf("Air Quality: %s", quality.c_str());
   display.display();
   delay(5000);

  if ((millis()-last)>120000) {
  Serial.printf("Pinging MQTT \n");
  if(! mqtt.ping()) {
  Serial.printf("Disconnecting \n");
  mqtt.disconnect();
  }
  last = millis();
  }
Humidity = humidRH;
Tempature = tempf;
SoilMoisture = moist;
AirQuality = aqSensor.getValue();
DustRead = Dust;

if((millis()-lastTime > 10000)) {
  if(mqtt.Update()) {
    humidObj.publish(Humidity);
    Serial.printf("Publishing %0.2f \n",Humidity); 
    tempObj.publish(Tempature);
    Serial.printf("Publishing %0.2f \n",Tempature);
    SoilObj.publish(Tempature);
    Serial.printf("Publishing %0.2i \n",moist); 
    airQualObj.publish(AirQuality);
    Serial.printf("Publishing %0.2i \n",AirQuality);
    dustObj.publish(DustRead);
    Serial.printf("Publishing %0.2f \n",Dust);
 }
   lastTime = millis(); 
}


Adafruit_MQTT_Subscribe *subscription;
    while((subscription = mqtt.readSubscription(1000))) {
        if(subscription == &buttonObjs) {
            buttonVal = atoi((char *)buttonObjs.lastread);
            Serial.printf("Received %i from Adafruit.io feed Subscribe \n",buttonVal);
        }
    }
}
    // Function to connect and reconnect as necessary to the MQTT server.
void MQTT_connect(); {
  int8_t ret;
      // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }
  Serial.print("Connecting to MQTT... ");

  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.printf("%s\n",(char *)mqtt.connectErrorString(ret));
       Serial.printf("Retrying MQTT connection in 5 seconds..\n");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
  }
  Serial.printf("MQTT Connected!\n");
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
  Serial.printlnf("Dust: %f", Dust);
}
