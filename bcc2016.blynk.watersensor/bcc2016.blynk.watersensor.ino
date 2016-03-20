/**************************************************************
 * Blynk is a platform with iOS and Android apps to control
 * Arduino, Raspberry Pi and the likes over the Internet.
 * You can easily build graphic interfaces for all your
 * projects by simply dragging and dropping widgets.
 *
 *   Downloads, docs, tutorials: http://www.blynk.cc
 *   Blynk community:            http://community.blynk.cc
 *   Social networks:            http://www.fb.com/blynkapp
 *                               http://twitter.com/blynk_app
 *
 * Blynk library is licensed under MIT license
 * This example code is in public domain.
 *
 **************************************************************
 * This example runs directly on ESP8266 chip.
 *
 * You need to install this for ESP8266 development:
 *   https://github.com/esp8266/Arduino
 *
 * Please be sure to select the right ESP8266 module
 * in the Tools -> Board menu!
 *
 * Change WiFi ssid, pass, and Blynk auth token to run :)
 *
 **************************************************************/


#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Ultrasonic.h>
#include <SimpleTimer.h>

#define WATER_SENSOR_PIN 16

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "blynk_big_long_alphanumeric_string_here";
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PW "your_wifi_password"

WidgetLED waterWarnLed(V2);

SimpleTimer timer;  //blynk likes us to use a timer


bool isWaterDetected() {
  int isDry=HIGH; //HIGH == no water, LOW==water detected
  isDry = digitalRead(WATER_SENSOR_PIN);
  if (isDry) {
    return false;
  }
  return true;
}


void checkForWater() {
  if (isWaterDetected()) {
    waterWarnLed.on();
    Blynk.notify("Water detected in garage.  You have $$ in repairs coming..!");
    Serial.println("Water detected!  Panic Now!");
  }
  else {
    waterWarnLed.off();
  }
}


void setup()
{
  Serial.begin(115200);
  pinMode(WATER_SENSOR_PIN, INPUT); //water sensor pin, init as input
  
  Blynk.begin(auth, WIFI_SSID, WIFI_PW);
  while (Blynk.connect() == false) {
    // Wait until connected
  }
  // Setup a function to be called every second
  timer.setInterval(2000L, checkForWater);
}

void loop()
{
  Blynk.run(); // Initiates Blynk
  timer.run(); // Initiates SimpleTimer
}

