import struct
from typing import Any, Dict
from paho.mqtt.client import Client as Mqtt, MQTTMessage
from influxdb_client import InfluxDBClient as Influx, WriteApi
from influxdb_client.client.write_api import SYNCHRONOUS


#----------------------------------------------#
INFLUX_INFO = ("http://127.0.0.1:8086", "ha3qJv2CKjdb_9-o55xCqd8qD_tN3kwpixwJ7ze6s7M4CKpH2GV3ReOu7n7qcbDoucS8z2jQRzjPBRqlxed0Cw==", "Bobo","SensorMonitoring")
MQTT_BROKER = ("broker.emqx.io",1883)
MQTT_INFO = ("influx_client","poopy/data/cose")
#--------------------globals-------------------#
api : WriteApi

# message |sensor id|sp1|sp2|sp10|at1|at2|at3|
def on_message(client : Mqtt, userdata : Any, message : MQTTMessage):
    global api
    print(f"New publish from {userdata if userdata else 'unknown'} on topic {message.topic} {message.payload.hex()}")
    try:
        samples = list(map(lambda x: x[0],struct.iter_unpack("@H",message.payload)))
        api.write(INFLUX_INFO[3],INFLUX_INFO[2],f"PM,SN={samples[0]},size=1.0,type=StandarParticulate concentration={samples[1]}")
        api.write(INFLUX_INFO[3],INFLUX_INFO[2],f"PM,SN={samples[0]},size=2.5,type=StandarParticulate concentration={samples[2]}")
        api.write(INFLUX_INFO[3],INFLUX_INFO[2],f"PM,SN={samples[0]},size=10.0,type=StandarParticulate concentration={samples[3]}")
        api.write(INFLUX_INFO[3],INFLUX_INFO[2],f"PM,SN={samples[0]},size=1.0,type=AtmosphericEnviroment concentration={samples[4]}")
        api.write(INFLUX_INFO[3],INFLUX_INFO[2],f"PM,SN={samples[0]},size=2.5,type=AtmosphericEnviroment concentration={samples[5]}")
        api.write(INFLUX_INFO[3],INFLUX_INFO[2],f"PM,SN={samples[0]},size=10.0,type=AtmosphericEnviroment concentration={samples[6]}")
    except Exception as e:
        print("Data format is wrong")
        print(e)
    #send to influx db

def connectMQTT():
    print("Connecting to mqtt")
    client = Mqtt(MQTT_INFO[0])
    client.connect(MQTT_BROKER[0],MQTT_BROKER[1])
    client.on_message = on_message
    client.subscribe(MQTT_INFO[1])
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
