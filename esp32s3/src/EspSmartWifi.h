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





#define LTBLUE    0xB6DF
#define LTTEAL    0xBF5F
#define LTGREEN   0xBFF7
#define LTCYAN    0xC7FF
#define LTRED     0xFD34
#define LTMAGENTA 0xFD5F
#define LTYELLOW  0xFFF8
#define LTORANGE  0xFE73
#define LTPINK    0xFDDF
#define LTPURPLE  0xCCFF
#define LTGREY    0xE71C

#define BLUE      0x001F
#define TEAL      0x0438
#define GREEN     0x07E0
#define CYAN      0x07FF
#define RED       0xF800
#define MAGENTA   0xF81F
#define YELLOW    0xFFE0
#define ORANGE    0xFC00
#define PINK      0xF81F
#define PURPLE    0x8010
#define GREY      0xC618
#define WHITE     0xFFFF
#define BLACK     0x0000

#define DKBLUE    0x000D
#define DKTEAL    0x020C
#define DKGREEN   0x03E0
#define DKCYAN    0x03EF
#define DKRED     0x6000
#define DKMAGENTA 0x8008
#define DKYELLOW  0x8400
#define DKORANGE  0x8200
#define DKPINK    0x9009
#define DKPURPLE  0x4010
#define DKGREY    0x4A49