#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

#define NICLA_I2C_ADDRESS 0x08

// WiFi credentials
const char* ssid = "IoT_EAA8";
const char* password = "26815718";

// MQTT broker
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* mqtt_topic = "nicla/data";

//WiFiClient espClient;
//PubSubClient client(espClient);

unsigned long lastMicros = 0;
const unsigned long intervalMicros = 5000; // 400 Hz = every 2.5 ms

/*
void setup_wifi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("PortentaClient")) {
      Serial.println("connected!");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds");
      delay(5000);
    }
  }
}
*/

void setup() {
  Serial.begin(1000000);
  delay(1000);
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.println("🔧 Starting Portenta MQTT stream...");

  Wire.begin();
  Wire.setClock(1000000);  // Set to 1 MHz
  
  /*setup_wifi();
  client.setSocketTimeout(10);
  client.setKeepAlive(15);
  client.setBufferSize(512);
  client.setServer(mqtt_server, mqtt_port);*/
}

/*
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = micros();
  if (now - lastMicros >= intervalMicros) {
    lastMicros = now;

  // Blink to show it's alive
  //digitalWrite(LED_BUILTIN, HIGH);
  delay(50);
  //digitalWrite(LED_BUILTIN, LOW);

  // Read from Nicla
  Wire.requestFrom(NICLA_I2C_ADDRESS, 128);
  String data = "";
  while (Wire.available()) {
    char c = Wire.read();
    data += c;
  }

  float values[9];
  int index = 0;
  char* token = strtok(const_cast<char*>(data.c_str()), ",");

  while (token != NULL && index < 9) {
    values[index++] = atof(token);
    token = strtok(NULL, ",");
  }

  if (index == 9) {
    digitalWrite(LED_BUILTIN, HIGH); // Light LED when valid data is streamed
    String payload = "{";
    payload += "\"ax\":" + String(values[0], 2) + ",";
    payload += "\"ay\":" + String(values[1], 2) + ",";
    payload += "\"az\":" + String(values[2], 2) + ",";
    payload += "\"gx\":" + String(values[3], 2) + ",";
    payload += "\"gy\":" + String(values[4], 2) + ",";
    payload += "\"gz\":" + String(values[5], 2) + ",";
    payload += "\"temp\":" + String(values[6], 2) + ",";
    payload += "\"press\":" + String(values[7], 2) + ",";
    payload += "\"hum\":" + String(values[8], 2);
    payload += "}";

    // Just print instead of MQTT publish
    Serial.println(payload);

    /*
    // MQTT publish
    if (!client.publish(mqtt_topic, payload.c_str())) {
      // Double-blink on publish failure
      for (int i = 0; i < 2; i++) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(100);
        digitalWrite(LED_BUILTIN, LOW);
        delay(100);
      }
    }*/

void loop() {
  unsigned long now = micros();
  if (now - lastMicros >= intervalMicros) {
    lastMicros = now;

    int numBytes = Wire.requestFrom(NICLA_I2C_ADDRESS, 128);
    if (numBytes == 0) {
      Serial.println("⚠️ No data received from Nicla");
      return;
    }

    String data = "";
    while (Wire.available()) {
      char c = Wire.read();
      data += c;
    }

    if (data.length() == 0) {
      Serial.println("⚠️ Wire.available() returned 0 bytes");
      return;
    }

    digitalWrite(LED_BUILTIN, HIGH); // Light up as soon as data is received

    Serial.print("📦 Raw I2C Data: ");
    Serial.println(data);

    // Optional: Parse values if you want
    float values[9];
    int index = 0;
    char* token = strtok(const_cast<char*>(data.c_str()), ",");

    while (token != NULL && index < 9) {
      values[index++] = atof(token);
      token = strtok(NULL, ",");
    }

    if (index == 9) {
      Serial.print("✅ Parsed ax: "); Serial.println(values[0], 2);
    } else {
      Serial.println("⚠️ Parsing failed, not 9 values");
    }
  }
}


