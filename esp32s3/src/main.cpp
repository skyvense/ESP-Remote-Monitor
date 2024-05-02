#include <Arduino.h>
#include <TFT_eSPI.h> // Hardware-specific library

#include <I2S.h>
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

#define SERIAL_SPEED     115200  // serial baud rate
#define PRINT_DEC_POINTS 2       // decimal points to print



/*audio bug added*/
#define OLED_RESET -1  
#define STATUS_LED  2
TFT_eSPI display = TFT_eSPI();                   // Invoke custom library with default width and height

EasyLed led(STATUS_LED, EasyLed::ActiveLevel::Low, EasyLed::State::Off);  //Use this for an active-low LED
EspSmartWifi wifi(display, led);
// NeoPixel光带信息
const int maxNumPixels = 100;  // 光带最大长度
const int dataPin = 4;     // NeoPixel数据引脚
Adafruit_NeoPixel pixels(maxNumPixels, dataPin, NEO_GRB + NEO_KHZ800);

// MQTT服务器信息
const int mqttPort = 1883;

// MQTT主题
const char* vStatusTopic = "espvdetect/data";

WiFiClient espClient;
PubSubClient mqtt(espClient);


#define WIRE Wire
void detect_i2c() 
{
  WIRE.begin();

  byte error, address;
  int nDevices;

//  display.fillScreen(BLACK);
  display.println("Scanning ");
  Serial.println("Scanning...");

  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    WIRE.beginTransmission(address);
    error = WIRE.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      display.print("I2C device found at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);
      display.print(address,HEX);
      Serial.println("  !");
      display.println("  !");

      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknown error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);

      display.print("Unknown error at address 0x");
      if (address<16)
        display.print("0");
      display.println(address,HEX);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");

    if (nDevices == 0)
    display.println("No I2C devices found\n");
  else
    display.println("done\n");

  //delay(5000);           // wait 5 seconds for next scan
}

// 回调函数，用于处理接收到的MQTT消息
void callback(char* topic, byte* payload, unsigned int length) {
  // 仅处理与controlTopic匹配的消息
  if (strcmp(topic, "---------") == 0) {
    String message;
    for (int i = 0; i < length; i++) {
      message += (char)payload[i];
    }
    Serial.print(message.c_str());
  }

}
// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) 
{
  for(uint16_t i=0; i<8; i++) 
  {
    pixels.setPixelColor(i, c);
    pixels.show();
    delay(wait);
  }
}


void setup() {
  Serial.begin(9600);
  //Serial.setDebugOutput(true);
  Wire.begin();

  pixels.begin();
  pixels.clear();
  pixels.setBrightness(100);
  //strip.setPixelColor((int)currentTemp%6, pixels.Color(fanspeed, setpoint*5, hysteresis*50));
  pixels.show();

  display.begin();
  display.setRotation(1);
  display.fillScreen(BLACK);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("V-Detection Starting...");

  detect_i2c();

  colorWipe(pixels.Color(255, 0, 0), 50); // Red
  colorWipe(pixels.Color(0, 255, 0), 50); // Green
  colorWipe(pixels.Color(0, 0, 255), 50); // Blue
  pixels.clear();
  pixels.show();
  
  wifi.initFS();
  wifi.ConnectWifi(); //This may loop forever if wifi is not connected
  
  wifi.DisplayIP();
  
  // 设置MQTT服务器和回调函数
  mqtt.setServer(wifi.getConfig().Server.c_str(), mqttPort);
  mqtt.setBufferSize(2048);
  mqtt.setCallback(callback);

}

// ESP8266 芯片ID
String chipId = String(ESP.getChipModel(), HEX);
String ClientId = "EspVDetect" + chipId;
int loop_count = 0;

void loop() {
  wifi.WiFiWatchDog();

  // 如果WiFi和MQTT都连接成功
  if (WiFi.status() == WL_CONNECTED && mqtt.connected()) 
  {
    // 定期处理MQTT消息
    mqtt.loop();
  } 
  else 
  {
    // 如果WiFi或MQTT连接断开，尝试重新连接
    if (!mqtt.connected()) 
    {
      // 尝试连接到MQTT服务器
      if (mqtt.connect(ClientId.c_str())) 
      {
        Serial.println("Connected to MQTT server!");
        // 订阅MQTT主题
        //mqtt.subscribe(textBitmapTopic);
      } 
      else 
      {
        Serial.print("Failed to connect to MQTT server, rc=");
        Serial.println(mqtt.state());
        delay(100);
        //return;
      }
      delay(100);
    }
  }


}