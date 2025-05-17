#include <WiFi.h>
#include <PubSubClient.h>
#include "Arduino_BHY2.h"

// WiFi credentials
const char* ssid = "Root_5G";
const char* password = "Appleuiux";

// MQTT Broker config
const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;
const char* mqtt_topic = "nicla/data";

// MQTT/WiFi
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Sensors
SensorXYZ accel(SENSOR_ID_ACC);
SensorXYZ gyro(SENSOR_ID_GYRO);
Sensor temp(SENSOR_ID_TEMP);
Sensor baro(SENSOR_ID_BARO);
Sensor hum(SENSOR_ID_HUM);

// Buffer
char jsonBuffer[512];

void setup_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void mqtt_reconnect() {
  while (!mqttClient.connected()) {
    mqttClient.connect("PortentaNiclaClient");
    delay(1000);
  }
}

void setup() {
  setup_wifi();
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqtt_reconnect();

  BHY2.begin(NICLA_STANDALONE);

  accel.configure(400, 0);
  gyro.configure(400, 0);
  temp.configure(1, 100);
  baro.configure(1, 100);
  hum.configure(1, 100);

  accel.begin();
  gyro.begin();
  temp.begin();
  baro.begin();
  hum.begin();
}

void loop() {
  mqttClient.loop();  // Keep connection alive
  if (!mqttClient.connected()) {
    mqtt_reconnect();
  }

  BHY2.update();

  static uint32_t lastTime = 0;
  static uint32_t count = 0;
  uint32_t now = millis();

  if (now - lastTime >= 10) {
    lastTime = now;
    count++;

    snprintf(jsonBuffer, sizeof(jsonBuffer),
      "{\"n\":%lu,\"ms\":%lu,"
      "\"aX\":%d,\"aY\":%d,\"aZ\":%d,"
      "\"gX\":%d,\"gY\":%d,\"gZ\":%d,"
      "\"t\":%.2f,\"p\":%.2f,\"h\":%.2f}",
      count, now,
      accel.x(), accel.y(), accel.z(),
      gyro.x(), gyro.y(), gyro.z(),
      temp.value(), baro.value(), hum.value()
    );

    mqttClient.publish(mqtt_topic, jsonBuffer);
  }
}
