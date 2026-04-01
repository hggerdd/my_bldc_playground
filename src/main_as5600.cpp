/**
 * ESP32 AS5600 I2C test based on the SimpleFOC magnetic sensor example.
 *
 * Wiring:
 * - AS5600 VCC -> ESP32 3V3
 * - AS5600 GND -> ESP32 GND
 * - AS5600 SDA -> GPIO21
 * - AS5600 SCL -> GPIO22
 *
 * Notes:
 * - AS5600 default I2C address is 0x36
 * - make sure SDA and SCL have pull-up resistors
 */
#include <Arduino.h>
#include <Wire.h>
#include <SimpleFOC.h>

static constexpr uint8_t I2C_SDA_PIN = 21;
static constexpr uint8_t I2C_SCL_PIN = 22;
static constexpr uint8_t AS5600_I2C_ADDRESS = 0x36;
static constexpr uint32_t SERIAL_BAUD = 115200;
static constexpr uint32_t PRINT_INTERVAL_MS = 200;

MagneticSensorI2C sensor = MagneticSensorI2C(AS5600_I2C);

bool scanForAS5600() {
  Wire.beginTransmission(AS5600_I2C_ADDRESS);
  return Wire.endTransmission() == 0;
}

void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(200);

  Serial.println();
  Serial.println("AS5600 I2C test");
  Serial.println("================");
  Serial.printf("Using SDA=%u SCL=%u\n", I2C_SDA_PIN, I2C_SCL_PIN);

  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  Wire.setClock(400000);

  if (!scanForAS5600()) {
    Serial.printf("AS5600 not found at I2C address 0x%02X\n", AS5600_I2C_ADDRESS);
    Serial.println("Check wiring, pull-ups, and pin mapping.");
    while (true) {
      delay(1000);
    }
  }

  Serial.printf("AS5600 detected at I2C address 0x%02X\n", AS5600_I2C_ADDRESS);

  sensor.init(&Wire);

  Serial.println("Sensor initialized.");
  Serial.println("Rotate the magnet to see angle updates.");
}

void loop() {
  sensor.update();

  const float angleRad = sensor.getAngle();
  const float angleDeg = angleRad * 180.0f / _PI;

  Serial.print(">angle_rad:");
  Serial.println(angleRad, 4);
  Serial.print(">angle_deg:");
  Serial.println(angleDeg, 2);

  delay(PRINT_INTERVAL_MS);
}
