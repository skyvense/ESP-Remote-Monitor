#ifndef LED_MATRIX
#include <Arduino.h>
#include "Arduino_GFX_Library.h"
#include "pin_config.h"
#include "TFT.h"

TFT::TFT() {
  // power on the tft
  #ifdef TFT_POWER
  if (TFT_POWER != GPIO_NUM_NC) {
    Serial.println("Powering on TFT");
    pinMode(TFT_POWER, OUTPUT);
    digitalWrite(TFT_POWER, TFT_POWER_ON);
  }
  #endif
  bus = new Arduino_ESP32LCD8(7 /* DC */, 6 /* CS */, 8 /* WR */, 9 /* RD */, 39 /* D0 */, 40 /* D1 */, 41 /* D2 */, 42 /* D3 */,
        45 /* D4 */, 46 /* D5 */, 47 /* D6 */, 48 /* D7 */);
  tft = new Arduino_ST7789(bus, 5 /* RST */, 0 /* rotation */, true /* IPS */, 170 /* width */, 320 /* height */, 35 /* col offset 1 */,
                                      0 /* row offset 1 */, 35 /* col offset 2 */, 0 /* row offset 2 */);
  
}

void TFT::begin()
{
  pinMode(PIN_POWER_ON, OUTPUT);
  digitalWrite(PIN_POWER_ON, HIGH);
  ledcSetup(0, 2000, 8);
  ledcAttachPin(PIN_LCD_BL, 0);
  ledcWrite(0, 255); /* Screen brightness can be modified by adjusting this parameter. (0-255) */
  tft->begin();
  #ifdef M5CORE2
  tft->setRotation(6);
  #else
  tft->setRotation(1);
  #endif
  tft->fillScreen(BLACK);
  #ifdef USE_DMA
  tft->initDMA();
  #endif

  tft->setTextSize(2);
  tft->setTextColor(GREEN, BLACK);
}

Arduino_GFX &TFT::getTFT()
{
  return *tft;
}

void TFT::drawPixels(int x, int y, int width, int height, uint16_t *pixels) {
  /*
  int numPixels = width * height;
  if (dmaBuffer[dmaBufferIndex] == NULL)
  {
    dmaBuffer[dmaBufferIndex] = (uint16_t *)malloc(numPixels * 2);
  }
  memcpy(dmaBuffer[dmaBufferIndex], pixels, numPixels * 2);
  #ifdef USE_DMA
  tft->dmaWait();
  #endif
  //tft->setAddrWindow(x, y, width, height);
  #ifdef USE_DMA
  tft->pushPixelsDMA(dmaBuffer[dmaBufferIndex], numPixels);
  #else
  //tft->pushPixels(dmaBuffer[dmaBufferIndex], numPixels);
  #endif
  dmaBufferIndex = (dmaBufferIndex + 1) % 2;
  */
 tft->draw16bitBeRGBBitmap(x,y,pixels,width,height);
}

void TFT::startWrite() {
  tft->startWrite();
}

void TFT::endWrite() {
  tft->endWrite();
}

int TFT::width() {
  return tft->width();
}

int TFT::height() {
  return tft->height();
}

void TFT::fillScreen(uint16_t color) {
  tft->fillScreen(color);
}

void TFT::drawChannel(int channelIndex) {
  tft->setCursor(20, 20);
  tft->setTextColor(GREEN, BLACK);
  tft->printf("%d", channelIndex);
}

void TFT::drawTuningText() {
  tft->setCursor(20, 20);
  tft->setTextColor(GREEN, BLACK);
  tft->println("TUNING...");
}

void TFT::drawSDCardFailed() {
  tft->fillScreen(RED);
  tft->setCursor(0, 20);
  tft->setTextColor(WHITE);
  tft->setTextSize(2);
  tft->println("Failed to mount SD Card");
}

void TFT::drawFPS(int fps) {
    // show the frame rate in the top right
    tft->setCursor(width() - 50, 0);
    tft->setTextColor(GREENYELLOW, BLACK);
    tft->printf("%d", fps);
}
#endif