# -*- coding:utf-8 -*-
#
#   author: nate
#
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
from urllib.parse import urlencode
import pyaudio
import random
import time
from paho.mqtt import client as mqtt_client
from wsgiref.handlers import format_date_time
from datetime import datetime
from time import mktime
import threading
import queue


broker = '192.168.8.3'
port = 1883
topic = "espaudio/data"
msg_sendback_topic = "espaudio/text"
msg_screen_bitmap_topic = "espaudio/text_bitmap"
client_id = f'python-mqtt-{random.randint(0, 1000)}'

response_text = '-'

def on_mqtt_connect(client, userdata, flags, rc, properties):
    if rc == 0:
        print("Connected to MQTT Broker!")
        subscribe(client)
    else:
        print("Failed to connect, return code %d\n", rc)

def connect_mqtt():
    # Set Connecting Client ID
    client = mqtt_client.Client(mqtt_client.CallbackAPIVersion.VERSION2, client_id)
    client.on_connect = on_mqtt_connect
    client.suppress_exceptions = True
    try:
        client.connect(broker, port)
    except BaseException:
        print("mqtt connection exception")
    return client

def on_message(client, userdata, msg):
    if msg.topic == msg_sendback_topic:
        print(f"received msg: {msg.payload}")
    elif msg.topic == msg_screen_bitmap_topic:
        print(f"received msg_screen_bitmap_topic, len {len(msg.payload)}")
    pass
    
    #print(f"Received bytes: `{len(msg.payload)}` from `{msg.topic}` topic")

def subscribe(client: mqtt_client):
    client.subscribe(msg_sendback_topic)
    client.subscribe(msg_screen_bitmap_topic)
    client.on_message = on_message


if __name__ == "__main__":   
    chunk = 640  # Record in chunks of 640 samples
    sample_format = pyaudio.paInt16  # 16 bits per sample
    channels = 1
    fs = 8000  # Record at 16000 samples per second
    seconds = 30000

    p = pyaudio.PyAudio()  # Create an interface to PortAudio

    print('Recording')

    stream = p.open(format=sample_format,
                    channels=channels,
                    rate=fs,
                    frames_per_buffer=chunk,
                    input=True)

    frames = []  # Initialize array to store frames

    print(f"Connecting with mqtt: {broker}, topic: {topic}")
    client = connect_mqtt()
    client.loop_start()

    # Store data in chunks for 3 seconds
    for i in range(0, int(fs / chunk * seconds)):
        data = stream.read(chunk)
        frames.append(data)
        client.publish(topic, data)