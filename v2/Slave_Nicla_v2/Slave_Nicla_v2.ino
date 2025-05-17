#include <Arduino.h>
#include <Wire.h>
#include <Arduino_BHY2.h>

#define I2C_SLAVE_ADDRESS 0x08

// Sensor declarations
SensorXYZ accel(SENSOR_ID_ACC);
SensorXYZ gyro(SENSOR_ID_GYRO);
Sensor temp(SENSOR_ID_TEMP);
Sensor baro(SENSOR_ID_BARO);
Sensor hum(SENSOR_ID_HUM);

// Output buffer
char i2cBuffer[128];

// Sensor value holders
int ax = 0, ay = 0, az = 0;
int gx = 0, gy = 0, gz = 0;
float t = 0.0, p = 0.0, h = 0.0;

void requestEvent() {
  // Format current sensor values into buffer
  snprintf(i2cBuffer, sizeof(i2cBuffer),
    "%d,%d,%d,%d,%d,%d,%.2f,%.2f,%.2f",
    ax, ay, az,
    gx, gy, gz,
    t, p, h
  );
  Wire.write(i2cBuffer);
}

void setup() {
  Serial.begin(115200);
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

  Wire.begin(I2C_SLAVE_ADDRESS);
  Wire.onRequest(requestEvent);
}

void loop() {
  BHY2.update();

  // Get latest values into global variables
  ax = accel.x();
  ay = accel.y();
  az = accel.z();

  gx = gyro.x();
  gy = gyro.y();
  gz = gyro.z();

  t = temp.value();
  p = baro.value();
  h = hum.value();
}
