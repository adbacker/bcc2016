#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <SimpleTimer.h>


#define LEDTOBLINK 0

const char* ssid = "iotdemo";
const char* password = "snoopy123";
SimpleTimer timer;  //timer leaves headroom for wifi overhead
int ledState=0;


//simple routine to blink onboard LED
void blinkLed() {
  if (ledState==0) {
    digitalWrite(LEDTOBLINK,HIGH);
    ledState=1; 
  } else {
    digitalWrite(LEDTOBLINK,LOW);
    ledState=0;
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  //while (WiFi.waitForConnectResult() != WL_CONNECTED) {
  //  Serial.println("Connection Failed! Rebooting...");
    delay(5000);
  //  ESP.restart();
  //}

  /*
   * BEGIN Arduino Over-the-air update stuff
   */
   //printToLcd("starting OTA","initialization");
   // Port defaults to 8266
  // ArduinoOTA.setPort(8266);
 
  // Hostname defaults to esp8266-[ChipID]
  //ArduinoOTA.setHostname("bcc_esp8266");

  // No authentication by default...but good to do!
  //ArduinoOTA.setPassword((const char *)"Abcd1234");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("End");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\n", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  /*
  * END Arduino Over-the-air update stuff
  */
  
  timer.setInterval(500L, blinkLed);
  pinMode(LEDTOBLINK, OUTPUT);
}

void loop() {
  ArduinoOTA.handle();
  timer.run(); // Initiates SimpleTimer

}
