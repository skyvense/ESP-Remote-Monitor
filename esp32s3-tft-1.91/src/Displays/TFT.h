#pragma once
#ifndef LED_MATRIX
#include "Display.h"

class Arduino_DataBus;
class Arduino_GFX;


class TFT: public Display {
private:
  Arduino_GFX *tft;
  Arduino_DataBus *bus;
  //uint16_t *dmaBuffer[2] = {NULL, NULL};
  int dmaBufferIndex = 0;
public:
  TFT();
  void drawPixels(int x, int y, int width, int height, uint16_t *pixels);
  void begin();
  void startWrite();
  void endWrite();
  int width();
  int height();
  void fillScreen(uint16_t color);
  void drawChannel(int channelIndex);
  void drawTuningText();
  void drawFPS(int fps);
  void drawSDCardFailed();

  Arduino_GFX &getTFT();

};
#endif