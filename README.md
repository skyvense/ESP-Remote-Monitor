# Esp32 Remote Screen Streamer
This is A very simple &amp; fast ESP32 Screen streaming solution, coding by PlatformIO/Arduino
The program running on an ESP32-S3 board allows you to remote monitoring a small area from your desktop.
Screen data is shared by mqtt protocol between desktop server(python code) and ESP32-S3 device

The main reason I decided to start this project is to provide a flexible way of dashboards:
Which I got desperate after searched the internet for several days: There is no good graphic dashboard running on this small devices, and it's also difficult to draw those fonts and lines beautifully.
But our computer is good at this, and there are thousands of ways to provide such function or interface, after I built the project, I can display everything on that small screen, without changing the code inside it.

And this code runs about 5-10 fps, which is fast enough for our inital purpose.

## ESP32 Build steps
0. Buy a board call: adafruit_feather_esp32s3_tft
1. Install PlatformIO(https://www.youtube.com/watch?v=JmvMvIphMnY)
2. Open the directory ESP32S3, and setup TFT libary correctly
3. Build the project and upload to an ESP32-S3 board
4. Modify SSID information in data/config.json.
5. Build SPIFFS image with "Build Filesystem Image" and upload to the board.

## Server
1. Setup a mosquitto message server, in my case, it is running at 192.168.8.3:1833
2. pip install -r server/requiements.txt
3. python server/pushMqttLiveDesktop.py
4. the python code then captures screen area:
   [code]
   ss_region = (100, 300, 340, 435)
   [/code] 
## Running    

1. My ESP32 screen shot
![Screen](/doc/241714918445_.pic.jpg)

2. While my computer screen display like this
![Screen capture](/doc/231714918416_.pic.jpg)

3. Let see the framerate:
![Movie](/pic/25_1714918450.mp4)

## What's next
The project is still in it's early stage, A proper C/C++ Desktop Application is need to select display area
