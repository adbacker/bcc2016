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
#include "Ultrasonic.h"
#include <SimpleTimer.h>

/* ultrasonic sensor stuff */
#define ULTRASONIC_ECHO_PIN 12 //D6 on nodeMCU
#define ULTRASONIC_TRIGGER_PIN 13 //D7 on nodeMCU
Ultrasonic ultrasonic(ULTRASONIC_TRIGGER_PIN,ULTRASONIC_ECHO_PIN); // (Trig PIN,Echo PIN)

#define GARAGE_DOOR_CLOSED_THRESHOLD 25

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "big_long_blynk_auth_key";

#define WIFI_SSID "your_ssid"
#define WIFI_PW "your_wifi_pw"

WidgetLCD lcd(V1);

SimpleTimer timer;  //blynk likes us to use a timer
int distanceToGarageDoor=0;

bool garageDoorIsClosed() {
  if (distanceToGarageDoor > GARAGE_DOOR_CLOSED_THRESHOLD) {
    return false;
  } 
  return true;
}


void updateLcd() {
  distanceToGarageDoor = ultrasonic.distanceRead();
  Serial.println(distanceToGarageDoor);
  lcd.clear(); //Use it to clear the LCD Widget
  lcd.print(1,0, "Garage door is");
  if (garageDoorIsClosed()) {
    lcd.print(1,1, "    CLOSED     ");
  }
  else {
    lcd.print(1,1, "     OPEN      ");
  }
}

void setup()
{
  Serial.begin(115200);
  Blynk.begin(auth, WIFI_SSID, WIFI_PW);
  while (Blynk.connect() == false) {
    Serial.print(".");
  }
  Serial.println("blynk connected!");
  // Setup a function to be called every second
  timer.setInterval(1000L, updateLcd);
}

void loop()
{
  Blynk.run(); // Initiates Blynk
  timer.run(); // Initiates SimpleTimer
}
