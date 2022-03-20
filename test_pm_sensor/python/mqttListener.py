from time import sleep
from paho.mqtt.client import Client, MQTTMessage
from pprint import pprint

def on_message(client : Client, userdata, message : MQTTMessage):
    print(f"Client {client._client_id} on mode {client._client_mode}")
    pprint(userdata)
    pprint(message.payload.decode('utf-8'))

sergio = Client("stinky_client")
sergio.connect("broker.emqx.io")
sergio.on_message = on_message
sergio.subscribe("poopy/data/cose")
sergio.loop_start()
while True:
    try:
        sleep(100000)
    except:
        break
sergio.loop_stop()