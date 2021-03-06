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
#include <SimpleTimer.h>


//defines for the temperature sensor
#include <DHT.h>
#define DHTPIN 14     // what digital pin we're connected to
// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);


// Blynk App auth token.
char auth[] = "big_long_blynk_access_key";

#define WIFI_SSID "your_ssid"
#define WIFI_PW "your_wifi_pw"

//use simple timer events instead of running inside the loop()
SimpleTimer timer;


// This function sends temp and humidity readings to Virtual Pins 5 and 6.
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
void sendTemp()
{
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
  Blynk.begin(auth, WIFI_SSID, WIFI_PW);
  dht.begin();


  // Setup a function to be called every second
  timer.setInterval(1000L, sendTemp);
}

void loop()
{
  Blynk.run(); // Initiates Blynk
  timer.run(); // Initiates SimpleTimer
}
