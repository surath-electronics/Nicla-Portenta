import time
import json
from pymongo import MongoClient
import paho.mqtt.client as mqtt
from threading import Timer

# Config
MQTT_BROKER = "broker.hivemq.com"
MQTT_PORT = 1883
MQTT_TOPIC = "nicla/data"

MONGO_URI = "mongodb+srv://nicla_user:nicla@cluster0.fkzil.mongodb.net/?retryWrites=true&w=majority&appName=Cluster0"
MONGO_DB = "test"
MONGO_COLLECTION = "portenta_stream"

BATCH_SIZE = 50
BATCH_INTERVAL = 5  # seconds

# MongoDB client
mongo_client = MongoClient(MONGO_URI)
collection = mongo_client[MONGO_DB][MONGO_COLLECTION]

# Buffer
message_buffer = []
last_batch_time = time.time()

def upload_batch():
    global message_buffer, last_batch_time
    if message_buffer:
        try:
            collection.insert_many(message_buffer)
            print(f"✅ Inserted {len(message_buffer)} records into MongoDB.")
            message_buffer.clear()
        except Exception as e:
            print("❌ MongoDB insert error:", e)
    last_batch_time = time.time()

# MQTT callbacks
def on_connect(client, userdata, flags, rc):
    print("Connected to MQTT broker.")
    client.subscribe(MQTT_TOPIC)

def on_message(client, userdata, msg):
    global message_buffer
    try:
        payload = json.loads(msg.payload.decode())
        payload["timestamp"] = time.time()
        message_buffer.append(payload)
    except Exception as e:
        print("Message error:", e)

    if len(message_buffer) >= BATCH_SIZE:
        upload_batch()

# Periodic flush
def periodic_flush():
    if time.time() - last_batch_time >= BATCH_INTERVAL:
        upload_batch()
    Timer(1, periodic_flush).start()

# MQTT setup
mqtt_client = mqtt.Client()
mqtt_client.on_connect = on_connect
mqtt_client.on_message = on_message
mqtt_client.connect(MQTT_BROKER, MQTT_PORT, 60)

# Run everything
periodic_flush()
mqtt_client.loop_forever()
