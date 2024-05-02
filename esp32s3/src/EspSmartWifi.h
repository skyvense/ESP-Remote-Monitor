#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <FS.h>

struct Config {
  String SSID = "SkyWifi2";
  String Passwd = "checkinooxx";
  String Server = "192.168.8.3";
  String Token = "0000";
};

struct EMPTY_SERIAL
{
  void println(const char *){}
  void println(String){}
  void printf(const char *, ...){}
  void print(const char *){}
  //void print(Printable) {}
  void begin(int){}
  void end(){}
};
//_EMPTY_SERIAL _EMPTY_SERIAL;
//#define Serial_debug  _EMPTY_SERIAL
#define Serial_debug  Serial

class TFT_eSPI;
class EasyLed;
class EspSmartWifi
{
private:
    TFT_eSPI &display_;
    EasyLed &led_;
    fs::File root;
    Config _config;

    void BaseConfig();
    void SmartConfig();
    bool LoadConfig();
    bool SaveConfig();
public:
    EspSmartWifi(TFT_eSPI &display, EasyLed &led):
    display_(display), led_(led)
    {
    }
    ~EspSmartWifi(){}

    void initFS();
    bool WiFiWatchDog();
    void ConnectWifi();
    void DisplayIP();
    Config &getConfig();
};


