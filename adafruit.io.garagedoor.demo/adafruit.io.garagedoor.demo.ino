/***************************************************
  Adafruit MQTT Library ESP8266 Example

  Must use ESP8266 Arduino from:
    https://github.com/esp8266/Arduino

  Works great with Adafruit's Huzzah ESP board & Feather
  ----> https://www.adafruit.com/product/2471
  ----> https://www.adafruit.com/products/2821

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Tony DiCola for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/
#include <ESP8266WiFi.h>
#include <Ultrasonic.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "Aaron's iPhone"
#define WLAN_PASS       "snoopy123"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    ""
#define AIO_KEY         ""

#define WATER_SENSOR_PIN 16
#define LED_PIN 14
#define MOTION_SENSOR_PIN 4
#define ULTRASONIC_ECHO_PIN 12
#define ULTRASONIC_TRIGGER_PIN 13

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Store the MQTT server, username, and password in flash memory.
// This is required for using the Adafruit MQTT library.
const char MQTT_SERVER[] PROGMEM    = AIO_SERVER;
const char MQTT_USERNAME[] PROGMEM  = AIO_USERNAME;
const char MQTT_PASSWORD[] PROGMEM  = AIO_KEY;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, AIO_SERVERPORT, MQTT_USERNAME, MQTT_PASSWORD);

/****************************** Feeds ***************************************/

// Setup a feed called 'garage' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
const char GARAGE_FEED[] PROGMEM = AIO_USERNAME "/feeds/garage";
Adafruit_MQTT_Publish garage = Adafruit_MQTT_Publish(&mqtt, GARAGE_FEED);

const char WATER_FEED[] PROGMEM = AIO_USERNAME "/feeds/water";
Adafruit_MQTT_Publish water = Adafruit_MQTT_Publish(&mqtt, WATER_FEED);

const char MOTION_FEED[] PROGMEM = AIO_USERNAME "/feeds/motion";
Adafruit_MQTT_Publish motion = Adafruit_MQTT_Publish(&mqtt, MOTION_FEED);


// Setup a feed called 'onoff' for subscribing to changes.
const char ONOFF_FEED[] PROGMEM = AIO_USERNAME "/feeds/flipled";
Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, ONOFF_FEED);

/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

Ultrasonic ultrasonic(ULTRASONIC_TRIGGER_PIN,ULTRASONIC_ECHO_PIN); // (Trig PIN,Echo PIN)

void setup() {
  Serial.begin(115200);
  delay(10);

  Serial.println(F("Adafruit MQTT demo"));
  pinMode(LED_PIN, OUTPUT);     // Initialize the GPIO0 pin as an output
  pinMode(WATER_SENSOR_PIN, INPUT); //water sensor pin, init as input
  pinMode(MOTION_SENSOR_PIN, INPUT);
  
  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&onoffbutton);
}

uint32_t x=0;

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &onoffbutton) {
      Serial.print(F("Got: "));
      Serial.println((char *)onoffbutton.lastread);

      // Switch on the LED if an 1 was received as first character
//
      if ((char)onoffbutton.lastread[0] == '1') {
        Serial.println("you turn me ON baby!");
        digitalWrite(LED_PIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
        // but actually the LED is on; this is because
        // it is acive low on the ESP-01)
      } else {
        Serial.println("I'm so turned OFF right now...");
        digitalWrite(LED_PIN, HIGH);  // Turn the LED off by making the voltage HIGH
      }
   } 
  
}
   
  

  // Now we can publish stuff!
  // check if the garage door is open or closed, and publish the state
  int distanceToGarageDoor = ultrasonic.Ranging(CM);
  int openClose=1;

  if (distanceToGarageDoor > 25) {
    openClose=0;
  }
  Serial.print("Garage door is this many cm away: ");
  Serial.print(distanceToGarageDoor);
  Serial.print(F(":: Sending garage door open/closed val "));
  Serial.print(openClose);
  Serial.print("...");
  if (! garage.publish(openClose)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }

  //check the water sensor, publish it's state
  // for this one, we'll only alert if it's found water.

  int hasWater=HIGH; //HIGH == no water, LOW==water detected
  hasWater = digitalRead(WATER_SENSOR_PIN);
  
  Serial.print(F(":: Sending water sensed: "));
  Serial.print(!hasWater);
  Serial.print("...");
  if (! water.publish(!hasWater)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }

  int hasMotion=LOW; 
  hasMotion = digitalRead(MOTION_SENSOR_PIN);
  Serial.print(F(":: Sending motion sensed: "));
  Serial.print(hasMotion);
  Serial.print("...");
  if (! motion.publish(hasMotion)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }

  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
  /*
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }
  */
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
