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
topic = "/espvideo/data"
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
    if msg.topic == "":
        print(f"received msg: {msg.payload}")
    elif msg.topic == "/espvideo/data":
        print(f"received msg_screen_bitmap_topic, len {len(msg.payload)}")
    pass
    
    #print(f"Received bytes: `{len(msg.payload)}` from `{msg.topic}` topic")

def subscribe(client: mqtt_client):
    client.subscribe(topic)
    client.on_message = on_message


if __name__ == "__main__":   

    print(f"Connecting with mqtt: {broker}, topic: {topic}")
    client = connect_mqtt()
    client.loop_start()

    

    while True:
        # Opening the binary file in binary mode as rb(read binary)
        f = open("test.jpg", mode="rb")

        # Reading file data with read() method
        data = f.read()
        client.publish(topic, data)
        # Knowing the Type of our data
        print(type(data))

        # Printing our byte sequenced data 
        #print(data)

        # Closing the opened file
        f.close()
        time.sleep(0.01)
        
        