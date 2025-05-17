#include <Wire.h>

#define NICLA_I2C_ADDRESS 0x08

void setup() {
  Wire.begin();
  Serial.begin(115200);
}

void loop() {
  Wire.requestFrom(NICLA_I2C_ADDRESS, 64);

  String data = "";
  while (Wire.available()) {
    char c = Wire.read();
    data += c;
  }

  // Split and label
  float values[9];
  int index = 0;
  char* token = strtok(const_cast<char*>(data.c_str()), ",");

  while (token != NULL && index < 9) {
    values[index++] = atof(token);
    token = strtok(NULL, ",");
  }

  if (index == 9) {
    Serial.print("ax: "); Serial.print(values[0]);
    Serial.print(", ay: "); Serial.print(values[1]);
    Serial.print(", az: "); Serial.print(values[2]);

    Serial.print(", gx: "); Serial.print(values[3]);
    Serial.print(", gy: "); Serial.print(values[4]);
    Serial.print(", gz: "); Serial.print(values[5]);

    Serial.print(", temp: "); Serial.print(values[6]);
    Serial.print(", hum: "); Serial.print(values[7]);
    Serial.print(", press: "); Serial.println(values[8]);
  }

  delay(10);
}
