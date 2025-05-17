#include "Arduino_BHY2.h"
#include <Wire.h>

SensorXYZ accel(SENSOR_ID_ACC);
SensorXYZ gyro(SENSOR_ID_GYRO);
Sensor temp(SENSOR_ID_TEMP);
Sensor baro(SENSOR_ID_BARO);
Sensor hum(SENSOR_ID_HUM);

char latestMsg[128];  // Latest data sample
volatile bool dataReady = false;

void setup() {
  Serial.begin(115200);
  while (!Serial);

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

  // Setup I2C slave
  Wire.begin(0x08);  // I2C address
  Wire.onRequest(onRequestHandler);  // Respond to master
}

void loop() {
  static uint32_t lastMicros = 0;
  BHY2.update();

  if (micros() - lastMicros >= 2500) {  // 400 Hz
    lastMicros = micros();

    snprintf(latestMsg, sizeof(latestMsg),
      "ms:%lu,aX:%d,aY:%d,aZ:%d,gX:%d,gY:%d,gZ:%d,t:%.2f,p:%.2f,h:%.2f\n",
      millis(),
      accel.x(), accel.y(), accel.z(),
      gyro.x(), gyro.y(), gyro.z(),
      temp.value(), baro.value(), hum.value());

    dataReady = true;

    Serial.print(latestMsg);  // Still print to Serial
  }
}

void onRequestHandler() {
  if (dataReady) {
    Wire.write(latestMsg, strlen(latestMsg));
    dataReady = false;
  } else {
    Wire.write("no_data\n");  // Avoid sending empty or junk
  }
}
