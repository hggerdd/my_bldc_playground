/**
 * SimpleFOC bring-up for ESP32 + AS5600 + 3PWM driver.
 *
 * Sensor pins are taken from the verified AS5600 test:
 * - SDA -> GPIO21
 * - SCL -> GPIO22
 *
 * Driver pins:
 * - PWM A -> GPIO25
 * - PWM B -> GPIO26
 * - PWM C -> GPIO27
 * - EN    -> GPIO14
 *
 * Motor parameters used for the initial setup:
 * - pole pairs: 7
 * - phase resistance: 2.3 Ohm
 * - phase inductance: 0.86 mH
 * - KV rating: 220 rpm/V
 *
 * This is a conservative first-start configuration:
 * - low voltage limits
 * - low velocity limit
 * - angle control via serial command
 */
#include <Arduino.h>
#include <Wire.h>
#include <SimpleFOC.h>

static constexpr uint8_t I2C_SDA_PIN = 21;
static constexpr uint8_t I2C_SCL_PIN = 22;
static constexpr uint8_t AS5600_I2C_ADDRESS = 0x36;

static constexpr uint8_t PWM_A_PIN = 25;
static constexpr uint8_t PWM_B_PIN = 26;
static constexpr uint8_t PWM_C_PIN = 27;
static constexpr uint8_t DRIVER_ENABLE_PIN = 14;

static constexpr uint32_t SERIAL_BAUD = 115200;
static constexpr uint32_t TELEPLOT_INTERVAL_MS = 100;

#define MOTOR_POLE_PAIRS 7
static constexpr float MOTOR_PHASE_RESISTANCE_OHM = 2.3f;
static constexpr float MOTOR_KV = 220.0f;
static constexpr float MOTOR_PHASE_INDUCTANCE_H = 0.00086f;

MagneticSensorI2C sensor = MagneticSensorI2C(AS5600_I2C);
BLDCMotor motor = BLDCMotor(
  MOTOR_POLE_PAIRS,
  MOTOR_PHASE_RESISTANCE_OHM,
  MOTOR_KV,
  MOTOR_PHASE_INDUCTANCE_H,
  MOTOR_PHASE_INDUCTANCE_H
);

BLDCDriver3PWM driver = BLDCDriver3PWM(PWM_A_PIN, PWM_B_PIN, PWM_C_PIN, DRIVER_ENABLE_PIN);

Commander command = Commander(Serial);
void doMotor(char* cmd) { command.motor(&motor, cmd); }
float targetAngle = 0.0f;

void onTargetCommand(char* cmd) {
  command.scalar(&targetAngle, cmd);
}

bool scanForAS5600() {
  Wire.beginTransmission(AS5600_I2C_ADDRESS);
  return Wire.endTransmission() == 0;
}

void setupMotor() {
  sensor.init(&Wire);
  motor.linkSensor(&sensor);

  driver.voltage_power_supply = 12.0f;
  driver.voltage_limit = 2.0f;
  driver.init();
  motor.linkDriver(&driver);

  motor.foc_modulation = FOCModulationType::SpaceVectorPWM;
  // motor.controller = MotionControlType::angle;
  motor.controller = MotionControlType::velocity;

  motor.voltage_limit = 12.0f;
  motor.velocity_limit = 10.0f;
  motor.voltage_sensor_align = 1.0f;

  motor.PID_velocity.P = 0.1f;
  motor.PID_velocity.I = 1.0f;
  motor.PID_velocity.D = 0.0f;
  motor.PID_velocity.output_ramp = 100.0f;
  motor.LPF_velocity.Tf = 0.02f;

  motor.P_angle.P = 10.0f;

  motor.useMonitoring(Serial);
  motor.monitor_downsample = 0;

  motor.init();
  motor.initFOC();
}

void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(300);

  Serial.println();
  Serial.println("SimpleFOC ESP32 bring-up");
  Serial.println("========================");
  Serial.printf("AS5600 I2C on SDA=%u SCL=%u\n", I2C_SDA_PIN, I2C_SCL_PIN);
  Serial.printf("PWM pins A/B/C = %u/%u/%u, EN = %u\n", PWM_A_PIN, PWM_B_PIN, PWM_C_PIN, DRIVER_ENABLE_PIN);

  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  Wire.setClock(400000);

  if (!scanForAS5600()) {
    Serial.printf("AS5600 not found at 0x%02X\n", AS5600_I2C_ADDRESS);
    Serial.println("Run main_as5600.cpp again or check wiring.");
    while (true) {
      delay(1000);
    }
  }

  SimpleFOCDebug::enable(&Serial);
  setupMotor();

  command.add('T', onTargetCommand, "target angle [rad]");
  command.add('M',doMotor,"motor");

  Serial.println("FOC init done.");
  Serial.println("Use serial command: T<angle_in_rad>");
  Serial.println("Example: T1.57");
}

void loop() {
  motor.loopFOC();
  motor.move(targetAngle);
  motor.monitor();
  command.run();

  static uint32_t lastTeleplotMs = 0;
  const uint32_t now = millis();
  if ((now - lastTeleplotMs) >= TELEPLOT_INTERVAL_MS) {
    sensor.update();

    // Serial.print(">target_angle:");
    // Serial.println(targetAngle, 4);
    // Serial.print(">shaft_angle:");
    // Serial.println(motor.shaft_angle, 4);
    // Serial.print(">shaft_velocity:");
    // Serial.println(motor.shaft_velocity, 4);

    lastTeleplotMs = now;
  }
}
