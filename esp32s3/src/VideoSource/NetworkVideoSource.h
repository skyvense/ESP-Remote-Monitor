#pragma once

#include "VideoSource.h"
class WiFiClient;
class PubSubClient;
class NetworkVideoSource: public VideoSource {
  private:
    WiFiClient espClient;
    PubSubClient mqtt;
    String mMqttServer;
    unsigned short mPort;

    bool mFrameReady=false;
    uint8_t *mCurrentFrameBuffer = NULL;
    size_t mCurrentFrameLength = 0;
    size_t mCurrentFrameBufferLength = 0;
    SemaphoreHandle_t mCurrentFrameMutex = NULL;

    static void _frameDownloaderTask(void *arg);
    void frameDownloaderTask();
    
  public:
    NetworkVideoSource(const char *mqttServer, unsigned short port);
    void OnMqttData(byte* payload, unsigned int length);
    void start();
    // see superclass for documentation
    bool getVideoFrame(uint8_t **buffer, size_t &bufferLength, size_t &frameLength);
};
