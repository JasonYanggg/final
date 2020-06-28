import time
import sys
import paho.mqtt.client as paho

mqttc = paho.Client()
# Settings for connection
host = "localhost"
topic= "Mbed"
port = 1883


# Callbacks
def on_connect(self, mosq, obj, rc):
    print("Connected rc: " + str(rc))

def on_message(mosq, obj, msg):
    global file, first
    print("Current motion: " + str(msg.payload) + "\n")
    if (first == 0):
        first = 1
        file.write(str(msg.payload) + "\n")
    else:
        file.write(str(msg.payload) + "\n -> ")

def on_subscribe(mosq, obj, mid, granted_qos):
    print("Subscribed OK")

def on_unsubscribe(mosq, obj, mid, granted_qos):
    print("Unsubscribed OK")

# Set callbacks

mqttc.on_message = on_message
mqttc.on_connect = on_connect
mqttc.on_subscribe = on_subscribe
mqttc.on_unsubscribe = on_unsubscribe

# Connect and subscribe
print("Connecting to " + host + "/" + topic)
mqttc.connect(host, port=1883, keepalive=60)
mqttc.subscribe(topic, 0)

file = open('log.txt', 'w')
first = 0
while True:
    try:
        mqttc.loop()
    except KeyboardInterrupt:
        file.close()
        break
print("Save file in log.txt")