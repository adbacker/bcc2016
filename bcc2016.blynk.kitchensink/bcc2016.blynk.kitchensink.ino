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
 * This example shows how value can be pushed from Arduino to
 * the Blynk App.
 *
 * WARNING :
 * For this example you'll need SimpleTimer library:
 *   https://github.com/jfturcot/SimpleTimer
 * Visit this page for more information:
 *   http://playground.arduino.cc/Code/SimpleTimer
 *
 * App project setup:
 *   Value Display widget attached to V5
 *
 **************************************************************/

#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Ultrasonic.h>
#include <SimpleTimer.h>


//****************  DHT22 Temperature Sensor Stuff  ****************
#include <DHT.h>
#define DHTPIN 4     // what digital pin we're connected to
// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);

//****************  Ultrasonic Sensor Stuff  ****************
#define ULTRASONIC_ECHO_PIN 12
#define ULTRASONIC_TRIGGER_PIN 13
Ultrasonic ultrasonic(ULTRASONIC_TRIGGER_PIN,ULTRASONIC_ECHO_PIN); // (Trig PIN,Echo PIN)
#define GARAGE_DOOR_CLOSED_THRESHOLD 25
int distanceToGarageDoor=0;
WidgetLCD lcd(V1);

//****************  Water Sensor Stuff  ****************
#define WATER_SENSOR_PIN 16
WidgetLED waterWarnLed(V2);

//****************  LED/Light Stuff  ****************
WidgetLED garageLightIndicatorLed(V2);

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "blynk_big_long_alphanumeric_string_here";
#define WIFI_SSID = "your_wifi_ssid"
#define WIFI_PW = "your_wifi_password"


SimpleTimer timer;


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
    Blynk.notify("Water detected in garage.  Panic Now!");
    Serial.println("Water detected in garage!  Panic Now!");
  }
  else {
    waterWarnLed.off();
  }
}

bool garageDoorIsClosed() {
  if (distanceToGarageDoor > GARAGE_DOOR_CLOSED_THRESHOLD) {
    return false;
  } 
  return true;
}

void updateLcd() {
  distanceToGarageDoor = ultrasonic.Ranging(CM);

  lcd.clear(); //Use it to clear the LCD Widget
  lcd.print(1,0, "Garage door is");
  if (garageDoorIsClosed()) {
    lcd.print(4,1, "CLOSED");
  }
  else {
    lcd.print(6,1, "OPEN");
  }
}


// This function sends Arduino's up time every second to Virtual Pin (5).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
void sendTemp() {
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V5, h);
  Blynk.virtualWrite(V6, f);
}

void setup()
{
  Serial.begin(115200); // See the connection status in Serial Monitor
  pinMode(WATER_SENSOR_PIN, INPUT); //water sensor pin, init as input
  Blynk.begin(auth, WIFI_SSID, WIFI_PW);
    while (Blynk.connect() == false) {
    // Wait until connected
  }
  
  dht.begin();


  // Setup a function to be called every second
  timer.setInterval(2000L, sendTemp);
  timer.setInterval(2000L, updateLcd);
  timer.setInterval(2000L, checkForWater);
}

void loop()
{
  Blynk.run(); // Initiates Blynk
  timer.run(); // Initiates SimpleTimer
}

