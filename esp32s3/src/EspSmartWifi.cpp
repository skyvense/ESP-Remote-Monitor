#include <Arduino.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <EasyLed.h>
#include <FS.h>
#include "EspSmartWifi.h"

bool EspSmartWifi::LoadConfig()
{
  fs::File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile)
  {
    Serial_debug.println("Failed to open config file");
    return false;
  }
  StaticJsonDocument<1024> doc;
  DeserializationError error = deserializeJson(doc, configFile);
  if (error)
  {
    Serial_debug.println("Failed to read file, using default configuration");
    return false;
  }
  _config.SSID = doc["SSID"] | "fail";
  _config.Passwd = doc["Passwd"] | "fail";
  if (_config.SSID == "fail" || _config.Passwd == "fail")
  {
    return false;
  }
  else
  {
    Serial_debug.println("Load wifi config from spiffs successful.");
    Serial_debug.print("Loaded ssid: ");
    Serial_debug.println(_config.SSID);
    Serial_debug.print("Loaded passwd: ");
    Serial_debug.println(_config.Passwd);
    return true;
  }
}

//save wifi config to fs
bool EspSmartWifi::SaveConfig()
{
  DynamicJsonDocument doc(1024);
  JsonObject root = doc.to<JsonObject>();
  root["SSID"] = _config.SSID;
  root["Passwd"] = _config.Passwd;
  root["Server"] = _config.Server;
  root["Token"] = _config.Token;

  fs::File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile)
  {
    Serial_debug.println("Failed to open config file for writing");
    return false;
  }
  if (serializeJson(doc, configFile) == 0)
  {
    Serial_debug.println("Failed to write to file");
    return false;
  }
  configFile.close();
  return true;
}

void EspSmartWifi::SmartConfig()
{
  Serial_debug.println("Use smart config to connect wifi.");
  display_.fillScreen(0);
  display_.setCursor(0, 0);
  display_.print("Wifi SmartConfig");
  WiFi.mode(WIFI_STA);
  WiFi.beginSmartConfig();
  while (1)
  {
    Serial_debug.println("Wait to connect wifi...");
    led_.flash(10, 50, 50, 0, 0);
    display_.print(".");
    delay(1000);
    if (WiFi.smartConfigDone())
    {
      Serial_debug.println("WiFi connected by smart config.");
      Serial_debug.print("SSID:");
      Serial_debug.println(WiFi.SSID());
      Serial_debug.print("IP Address:");
      Serial_debug.println(WiFi.localIP().toString());
      display_.print("Smartconfig connected.\nIP:");
      display_.print(WiFi.localIP());
      display_.println("");
      display_.print("AP: ");
      display_.println(WiFi.SSID());

      _config.SSID = WiFi.SSID();
      _config.Passwd = WiFi.psk();
      if (!SaveConfig())
      {
        Serial_debug.println("Failed to save config");
      }
      else
      {
        Serial_debug.println("Config saved");
      }
      break;
    }
  }
}


//Connect wifi
void EspSmartWifi::ConnectWifi()
{
  SaveConfig();
  if (LoadConfig())
  {
    BaseConfig();
  }
  else
  {
    SmartConfig();
  }
}

bool EspSmartWifi::WiFiWatchDog()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    BaseConfig();
  }
  return true;
}

void EspSmartWifi::BaseConfig()
{
  Serial_debug.println("Use base config to connect wifi.");
  led_.flash(4, 125, 125, 0, 0);
  display_.fillScreen(0);
  display_.setCursor(0, 0);
  display_.print("Connecting ");
  display_.print(_config.SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(_config.SSID, _config.Passwd);
  //连接超时时间，30s后没有连接将会转入SmartConfig
  int timeout = 30000;
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial_debug.println("Wait to connect wifi...");
    display_.print(".");
    delay(500);
    timeout = timeout - 300;
    if (timeout <= 0)
    {
      Serial_debug.println("Wifi connect timeout, use smart config to connect...");
      display_.print("FAIL, begin SMARTCONFIG");
      SmartConfig();
      return;
    }

    led_.flash(2, 125, 125, 0, 0);
  }
  Serial_debug.println("WiFi connected by base config.");
  Serial_debug.print("SSID:");
  Serial_debug.println(WiFi.SSID());
  Serial_debug.print("IP Address:");
  Serial_debug.println(WiFi.localIP().toString());
  display_.print("WiFi connected.\nIP:");
  display_.print(WiFi.localIP());
}

void EspSmartWifi::initFS()
{
  //Mount FS
  Serial_debug.println("Mounting FS...");
  if (!SPIFFS.begin(true))
  {
    if (!SPIFFS.format())
    {
      Serial_debug.println("Failed to format file system");
      return;
    }
    Serial_debug.println("Failed to mount file system");
    return;
  }
}

void EspSmartWifi::DisplayIP()
{
  display_.fillScreen(0);
  display_.setTextColor(0xFFFF);
  display_.setTextSize(1); // Draw 2X-scale text
  display_.setCursor(2, 2);
  display_.println("IP: ");
  display_.setCursor(22, 2);
  display_.println(WiFi.localIP().toString());
}

Config &EspSmartWifi::getConfig()
{
  return _config;
}