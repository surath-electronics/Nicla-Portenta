#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(1000);

  Serial.println("Scanning networks...");
  int n = WiFi.scanNetworks();
  
  if (n == 0) {
    Serial.println("No networks found. Check Wi-Fi hardware.");
  } else {
    Serial.println("Networks found:");
    for (int i = 0; i < n; i++) {
      Serial.println(WiFi.SSID(i) + " (" + WiFi.RSSI(i) + " dBm)");
    }
  }
}

void loop() {}