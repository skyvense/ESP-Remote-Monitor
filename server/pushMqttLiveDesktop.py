# -*- coding:utf-8 -*-
#
#   author: nate
#
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
import random
import time
from paho.mqtt import client as mqtt_client
from datetime import datetime
from time import mktime
from PIL import ImageGrab
from io import BytesIO  

broker = '192.168.8.3'
port = 1883
topic = "/espvideo/data"
feedbacktopic = "/espvideo/echo"
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
    elif msg.topic == feedbacktopic:       
        send_frame()
        pass
    
    #print(f"Received bytes: `{len(msg.payload)}` from `{msg.topic}` topic")

def subscribe(client: mqtt_client):
    client.subscribe(feedbacktopic)
    client.on_message = on_message

def send_frame():
    #do the grab
    ss_region = (100, 300, 340, 435)
    ss_img = ImageGrab.grab(ss_region)

    #JPG does not support transparency - RGBA means Red, Green, Blue, Alpha - Alpha is transparency.
    ss_img = ss_img.convert('RGB'); 
    mem_file = BytesIO()
    ss_img.save(mem_file, 'jpeg', quality=75)
    data = mem_file.getvalue()

    client.publish(topic, data)
    # Knowing the Type of our data
    #print(type(data))



if __name__ == "__main__":   

    print(f"Connecting with mqtt: {broker}, topic: {topic}")
    client = connect_mqtt()
    client.loop_start()

    while True:
        #send one frame every 10 seconds for a start 
        send_frame()
        time.sleep(10)
        
        