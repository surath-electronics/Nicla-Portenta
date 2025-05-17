#include "Arduino_BHY2.h"
#include <Wire.h>

#define I2C_SLAVE_ADDRESS 0x08

// Sensor declarations
SensorXYZ accel(SENSOR_ID_ACC);
SensorXYZ gyro(SENSOR_ID_GYRO);
Sensor temp(SENSOR_ID_TEMP);
Sensor baro(SENSOR_ID_BARO);
Sensor hum(SENSOR_ID_HUM);

// Buffer for serial output
char i2cBuffer[256];

/*float computeODR(uint32_t currentTime) {  // Uncomment this for ODR reading on the go
  const int windowSize = 100;
  static uint32_t firstTimestamp = 0;
  static int sampleCount = 0;

  if (sampleCount == 0) {
    firstTimestamp = currentTime;
  }

  sampleCount++;

  if (sampleCount >= windowSize) {
    uint32_t deltaTime = currentTime - firstTimestamp; // in microseconds
    float odr = 1000000.0 * (windowSize - 1) / deltaTime;  // Hz
    sampleCount = 0;  // Reset for next measurement window
    return odr;
  }

  return 0.0;
}*/

void setup() {
  Serial.begin(1000000);
  while (!Serial);
  
  BHY2.begin(NICLA_STANDALONE);

  // Configure sensors
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
  Wire.setClock(1000000);  // Set to 1 MHz
  Wire.onRequest(sendI2CData);

}

void loop() {
  static uint32_t lastPrintTime = 0;
  static uint32_t dataCount = 0;
  
  BHY2.update();

  uint32_t currentTime = micros();
  if (currentTime - lastPrintTime >= 2500) {
    lastPrintTime = currentTime;
    dataCount++;

    // Get current timestamp in microseconds only for ODR calculation
    //uint32_t timestamp_us = currentTime;

    snprintf(i2cBuffer, sizeof(i2cBuffer),
      "%d,%d,%d,%d,%d,%d,%.2f,%.2f,%.2f\n", // Data
      accel.x(), accel.y(), accel.z(),
      gyro.x(), gyro.y(), gyro.z(),
      temp.value(), baro.value(), hum.value());

    //Serial.print("\n");

    Serial.print(i2cBuffer);
    /*float odr = computeODR(timestamp_us); // Uncomment this for ODR reading on the go
    if (odr > 0.0) {
      Serial.print("📈 ODR: ");
      Serial.print(odr, 2);
      Serial.println(" Hz");
    }*/
  }
}

void sendI2CData() {
  Wire.write(i2cBuffer);  // Respond to Portenta I2C request with latest data
}
