# app.py
from fastapi import FastAPI
from db import get_latest
import threading
from mqtt_ingest import start_mqtt

app = FastAPI(title="IoT Lab Plus API")

@app.get("/")
def root():
    return {"status": "IoT Lab Plus API running"}

@app.get("/telemetry/latest")
def latest_data(limit: int = 20):
    rows = get_latest(limit)
    return [
        {
            "device": r[0],
            "temperature": r[1],
            "alarm": r[2],
            "timestamp": r[3]
        }
        for r in rows
    ]

# chạy MQTT song song API
threading.Thread(target=start_mqtt, daemon=True).start()
