# mqtt_ingest.py
import json
from datetime import datetime
import paho.mqtt.client as mqtt
from db import insert_telemetry

MQTT_BROKER = "broker.emqx.io"
MQTT_PORT = 1883
TOPIC = "lab7/telemetry"

def on_connect(client, userdata, flags, rc):
    print("MQTT connected")
    client.subscribe(TOPIC)

def on_message(client, userdata, msg):
    payload = json.loads(msg.payload.decode())
    print("[MQTT] payload =", payload)
    device = payload["device"]
    batch = payload["batch"]

    for sample in batch:
        ts = datetime.fromtimestamp(sample["ts"])
        insert_telemetry(
            device=device,
            temp=sample["temp"],
            alarm=sample["alarm"],
            ts=ts
        )

    print(f"[MQTT] Stored batch {len(batch)} samples")

def start_mqtt():
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(MQTT_BROKER, MQTT_PORT)
    client.loop_forever()
