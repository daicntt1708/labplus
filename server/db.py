# db.py
import psycopg2

conn = psycopg2.connect(
    dbname="CREATE DATABASE iot_lab7;",
    user="postgres",
    password="123456",
    host="localhost",
    port=5432
)

conn.autocommit = True
cur = conn.cursor()

def insert_telemetry(device, temp, alarm, ts):
    cur.execute(
        """
        INSERT INTO telemetry (device_code, temperature, alarm, ts)
        VALUES (%s, %s, %s, %s)
        """,
        (device, temp, alarm, ts)
    )

def get_latest(limit=20):
    cur.execute(
        """
        SELECT device_code, temperature, alarm, ts
        FROM telemetry
        ORDER BY ts DESC
        LIMIT %s
        """,
        (limit,)
    )
    return cur.fetchall()
