#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>
#include <EasyLed.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "EspSmartWifi.h"
#include <WiFiUdp.h>
#include <TFT_eSPI.h>

#include "Displays/TFT.h"
#include "Displays/Matrix.h"

#include "VideoPlayer.h"
#include "VideoSource/NetworkVideoSource.h"

VideoSource *videoSource = NULL;
VideoPlayer *videoPlayer = NULL;

#ifdef LED_MATRIX
Matrix display;
#else
TFT display;
#endif



#define STATUS_LED  2

EasyLed led(STATUS_LED, EasyLed::ActiveLevel::Low, EasyLed::State::Off);  //Use this for an active-low LED
EspSmartWifi wifi(display.getTFT(), led);

void setup() {
  Serial.begin(9600);


  display.getTFT().setTextColor(0xFFFF);
  display.getTFT().setCursor(0, 0);
  display.getTFT().print("V-Detection Starting...");

  wifi.initFS();
  wifi.ConnectWifi(); //This may loop forever if wifi is not connected
  
  wifi.DisplayIP();
}


void loop() {
  wifi.WiFiWatchDog();
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  videoSource = new NetworkVideoSource("192.168.8.3", 1833);
  videoPlayer = new VideoPlayer(
    videoSource,
    display
  );
  videoPlayer->start();
  videoPlayer->play();
}