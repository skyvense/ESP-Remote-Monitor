#include <Arduino.h>
#include <WiFi.h>

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

#include "NetworkVideoSource.h"

const char *MQTT_DATA_SUBJECT = "/espvideo/data";

NetworkVideoSource *networkVideoSource_this = NULL;

// 回调函数，用于处理接收到的MQTT消息
void callback(char* topic, byte* payload, unsigned int length) 
{
  if (strcmp(topic, MQTT_DATA_SUBJECT) == 0) 
  {
    //String message;
    //for (int i = 0; i < length; i++) {
    //  message += (char)payload[i];
    //}
    //Serial.print(message.c_str());
    if (networkVideoSource_this)
      networkVideoSource_this->OnMqttData(payload, length);
    Serial.printf("mqtt got %d bytes\n", length);
  }
}

void NetworkVideoSource::OnMqttData(byte* payload, unsigned int length)
{
  // read the image into our local buffer
  int jpegLength = length;
  // lock the image buffer
  xSemaphoreTake(mCurrentFrameMutex, portMAX_DELAY);
  // reallocate the image buffer if necessary
  if (jpegLength > mCurrentFrameBufferLength)
  {
    mCurrentFrameBuffer = (uint8_t *)realloc(mCurrentFrameBuffer, jpegLength);
    mCurrentFrameBufferLength = jpegLength;
  }
  // copy the image buffer
  memcpy(mCurrentFrameBuffer, payload, jpegLength);
  mCurrentFrameLength = jpegLength;
  // don't set this flag if we aren't playing otherwise we might trigger a draw
  if (mState == VideoPlayerState::PLAYING)
  {
    mFrameReady = true;
  }
  // unlock the image buffer
  xSemaphoreGive(mCurrentFrameMutex);
  Serial.printf("Read %d bytes in %d ms\n", jpegLength, millis());
}

void NetworkVideoSource::_frameDownloaderTask(void *param)
{
  NetworkVideoSource *networkVideoSource = (NetworkVideoSource *)param;
  networkVideoSource->frameDownloaderTask();
}

void NetworkVideoSource::frameDownloaderTask()
{
  WiFiClient espClient;
  PubSubClient mqtt(espClient);
  // 设置MQTT服务器和回调函数
  Serial.printf("Connecting to MQTT server: %s:%d\n", mMqttServer.c_str(), mPort);
  mqtt.setServer(mMqttServer.c_str(), mPort);
  mqtt.setBufferSize(65535);
  mqtt.setCallback(callback);

  // ESP 芯片ID
  String chipId = String(ESP.getChipModel(), HEX);
  String ClientId = "EspVideo" + chipId;
  while (true)
  {
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
          mqtt.subscribe(MQTT_DATA_SUBJECT);
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
    delay(10);
  }
}


NetworkVideoSource::NetworkVideoSource(const char *mqttServer, unsigned short port):
mMqttServer(mqttServer), mPort(port)
{
}

void NetworkVideoSource::start() {
  // create a mutex to control access to the JPEG buffer
  mCurrentFrameMutex = xSemaphoreCreateMutex();
  networkVideoSource_this = this;
  // launch the frame downloader task
  xTaskCreatePinnedToCore(
      _frameDownloaderTask,
      "Frame Downloader",
      10000,
      this,
      1,
      NULL,
      0);
}

bool NetworkVideoSource::getVideoFrame(uint8_t **buffer, size_t &bufferLength, size_t &frameLength) {
  if(mCurrentFrameBuffer == NULL) {
    return false;
  }
  bool copiedFrame = false;
  // lock the image buffer
  xSemaphoreTake(mCurrentFrameMutex, portMAX_DELAY);
  // if the frame is ready, copy it to the buffer
  if (mFrameReady) {
    mFrameReady=false;
    copiedFrame = true;
    // reallocate the image buffer if necessary
    if (mCurrentFrameBufferLength > bufferLength) {
      *buffer = (uint8_t *)realloc(*buffer, mCurrentFrameBufferLength);
      bufferLength = mCurrentFrameBufferLength;
    }
    // copy the image buffer
    memcpy(*buffer, mCurrentFrameBuffer, mCurrentFrameLength);
    frameLength = mCurrentFrameLength;
  }
  // unlock the image buffer
  xSemaphoreGive(mCurrentFrameMutex);
  // return true if we copied a frame, false otherwise
  return copiedFrame;
}
