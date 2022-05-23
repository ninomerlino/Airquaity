import struct
from typing import Any, Dict, List
from paho.mqtt.client import Client as Mqtt, MQTTMessage
from influxdb_client import InfluxDBClient as Influx, WriteApi, Point
from influxdb_client.client.write_api import SYNCHRONOUS


# ----------------------------------------------#
INFLUX_INFO = (
    "http://127.0.0.1:8086",
    "ww9YvwgUalqFZgG69txggANcxyK2FhzmaP85iKTjNa6L-YxIme24iTuBDN1tNZF2xTCcx5TF-MtVGzP2P2YwvA==",
    "Bobo",
    "airquality",
)
MQTT_BROKER = ("broker.emqx.io", 1883)
MQTT_INFO = {
    "client_id": "influx_client",
    "mqtt_topic": "poopy/data/cose",
}
# --------------------globals-------------------#
api: WriteApi

# payload format [SID,SP1,SP2,SP10,AE1,AE2,AE10]


def payload_to_point(payload: bytes) -> Point:
    samples = [x[0] for x in struct.iter_unpack("@H", payload)]
    point = Point("AIR QUALITY")
    point.tag("Sensor Id", samples[0])
    point.field("SP_1.0", samples[1])
    point.field("SP_2.5", samples[2])
    point.field("SP_10", samples[3])
    point.field("AE_1.0", samples[4])
    point.field("AE_2.5", samples[5])
    point.field("AE_10", samples[6])
    point.field("TVOC_PM", samples[7])
    point.field("CO2_EQ", samples[8])
    return point


def on_message(client: Mqtt, userdata: Any, message: MQTTMessage):
    global api
    print(
        f"New publish from {userdata if userdata else 'unknown'} on topic {message.topic} {message.payload.hex()}"
    )
    try:
        api.write(INFLUX_INFO[3], INFLUX_INFO[2], payload_to_point(message.payload))
    except struct.error as e:
        print(f"Data format is wrong payload len={len(message.payload)}")
    except Exception as e:
        print("---Unknow exception---")
        print(e)
    # check for http ecxeption
    # send to influx db


def connectMQTT():
    print("Connecting to mqtt")
    client = Mqtt(MQTT_INFO["client_id"])
    client.connect(MQTT_BROKER[0], MQTT_BROKER[1])
    client.on_message = on_message
    client.subscribe(MQTT_INFO["mqtt_topic"])
    return client


def connectDB():
    print("Conneting to influx")
    db = Influx(url=INFLUX_INFO[0], token=INFLUX_INFO[1], org=INFLUX_INFO[2])
    cursor = db.write_api(SYNCHRONOUS)
    return db, cursor


database, api = connectDB()
mqtt = connectMQTT()
try:
    mqtt.loop_forever()
except KeyboardInterrupt:
    print("Ending mqtt connection")
    mqtt.disconnect()
    print("Ending database connection")
    database.close()
