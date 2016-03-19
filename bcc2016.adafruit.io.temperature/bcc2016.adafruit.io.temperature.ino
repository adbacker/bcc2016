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


/************************* DHT sensor setup *********************************/
#include <DHT.h>
#define DHTPIN 4     // what digital pin we're connected to
// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);


/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "wifi_ssid"
#define WLAN_PASS       "wifi_password"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "adafruit_userid"
#define AIO_KEY         "adafruit_big_long_alphanumeric_string_here"


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

// Setup a feeds called called "temperature" and "humidity" for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
const char TEMPERATURE_FEED[] PROGMEM = AIO_USERNAME "/feeds/temperature";
Adafruit_MQTT_Publish temperature = Adafruit_MQTT_Publish(&mqtt, TEMPERATURE_FEED);

const char HUMIDITY_FEED[] PROGMEM = AIO_USERNAME "/feeds/humidity";
Adafruit_MQTT_Publish humidity = Adafruit_MQTT_Publish(&mqtt, HUMIDITY_FEED);


/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

void setup() {
  Serial.begin(115200);
   
  dht.begin();
  pinMode(DHTPIN, INPUT); //temp sensor pin, init as input

  delay(10);

  Serial.println(F("Adafruit MQTT demo"));

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
}

uint32_t x=0;

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  //get the temperature and humidity
  
  float humid = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float temp_c = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float temp_f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(humid) || isnan(temp_c) || isnan(temp_f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  //publish the temperature
  Serial.print("temp: ");
  Serial.print(temp_f);
  if (! temperature.publish(temp_f)) {
    Serial.println(F(" publish failed"));
  } else {
    Serial.println(F(" pub OK!"));
  }
  
  //publish the humidity
  Serial.print("humid: ");
  Serial.print(humid);
  if (! humidity.publish(humid)) {
    Serial.println(F(" publish failed"));
  } else {
    Serial.println(F(" pub OK!"));
  }

  //wait for 3 second
  delay(3000);
  
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
