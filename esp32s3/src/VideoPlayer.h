#pragma once
#include <Arduino.h>
// #ifdef LED_MATRIX
// struct File; // fixes weird compilation error
// #endif
#include "JPEGDEC.h"

#include "VideoPlayerState.h"

class Display;
class VideoSource; 

class VideoPlayer {
  private:
    int mChannelVisible = 0;
    VideoPlayerState mState = VideoPlayerState::STOPPED;

    // video playing
    Display &mDisplay;
    JPEGDEC mJpeg;

    // video source
    VideoSource *mVideoSource = NULL;


    static void _framePlayerTask(void *param);

    void framePlayerTask();
    void audioPlayerTask();

    friend int _doDraw(JPEGDRAW *pDraw);

  public:
    VideoPlayer(VideoSource *videoSource, Display &display);
    void start();
    void play();
    void stop();
    void pause();
    void playStatic();
};