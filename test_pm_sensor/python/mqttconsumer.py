import struct
from typing import Any, Dict, List
from paho.mqtt.client import Client as Mqtt, MQTTMessage
from influxdb_client import InfluxDBClient as Influx, WriteApi,Point
from influxdb_client.client.write_api import SYNCHRONOUS


#----------------------------------------------#
INFLUX_INFO = ("http://127.0.0.1:8086", "ha3qJv2CKjdb_9-o55xCqd8qD_tN3kwpixwJ7ze6s7M4CKpH2GV3ReOu7n7qcbDoucS8z2jQRzjPBRqlxed0Cw==", "Bobo","SensorMonitoring")
MQTT_BROKER = ("broker.emqx.io",1883)
MQTT_INFO = {
    "client_id":"influx_client",
    "pm_topic":"poopy/data/cose",
    "voc_topic":""
    }
#--------------------globals-------------------#
api : WriteApi


def store_pm_data(payload : bytes):
    samples = [x[0] for x in struct.iter_unpack("@H",payload)]
    api.write(INFLUX_INFO[3],INFLUX_INFO[2],Point("PM").tag("SN",samples[0]).tag("size","1.0").field("StandarParticulate",samples[1]).field("AtmosphericEnviroment",samples[4]))        
    api.write(INFLUX_INFO[3],INFLUX_INFO[2],Point("PM").tag("SN",samples[0]).tag("size","2.5").field("StandarParticulate",samples[2]).field("AtmosphericEnviroment",samples[5]))
    api.write(INFLUX_INFO[3],INFLUX_INFO[2],Point("PM").tag("SN",samples[0]).tag("size","10").field("StandarParticulate",samples[3]).field("AtmosphericEnviroment",samples[6]))

def store_voc_data(payload : bytes):
    raise RuntimeError("Not Implemented")

def on_message(client : Mqtt, userdata : Any, message : MQTTMessage):
    global api
    print(f"New publish from {userdata if userdata else 'unknown'} on topic {message.topic} {message.payload.hex()}")
    try:
        if message.topic == MQTT_INFO["pm_topic"]:
            store_pm_data(message.payload)
        elif message.topic == MQTT_INFO["voc_topic"]:
                store_voc_data(message.payload)
        else:
            raise RuntimeError("Not Implemented")
    except struct.error as e:
        print(f"Data format is wrong payload len={len(message.payload)}")
    except Exception as e:
        print("Unknow exception")
        print(e)
    #check for http ecxeption
    #send to influx db

def connectMQTT():
    print("Connecting to mqtt")
    client = Mqtt(MQTT_INFO["client_id"])
    client.connect(MQTT_BROKER[0],MQTT_BROKER[1])
    client.on_message = on_message
    client.subscribe(MQTT_INFO["pm_topic"])
    return client

def connectDB():
    print("Conneting to influx")
    db = Influx(url=INFLUX_INFO[0],token=INFLUX_INFO[1],org=INFLUX_INFO[2])
    cursor = db.write_api(SYNCHRONOUS)
    return db,cursor

database, api = connectDB()
mqtt = connectMQTT()
try:
    mqtt.loop_forever()
except KeyboardInterrupt:
    print("Ending mqtt connection")
    mqtt.disconnect()
    print("Ending database connection")
    database.close()
