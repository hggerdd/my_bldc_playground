//Some simple balancing code as an example of how the components work together.

#include <SimpleFOC.h>
#include <Wire.h>
#include <AS5600.h>
#include <Arduino_LSM9DS1.h>

// BALANCE ANGLE OFFSET
const float imu_offset = 1.8f; // measured balanced angle in degrees

// PD GAINS — START HERE
float Kp = 7.0f;      // Proportional: increase for faster reaction, decrease if twitchy
float Kd = 0.0f;    // Derivative: increase to calm overshoot, decrease if sluggish

// MOTOR OUTPUT LIMIT
float output_limit = 20.0f; // rad/s. Raise if motors too weak, lower if too violent

// FILTER SETTINGS
const float CF_GYRO_WEIGHT = 0.98f;
const float CF_ACCEL_WEIGHT = 0.02f;
const float accel_alpha = 0.1f;
const float GYRO_SIGN = 1.0f; // flip sign if angle direction is wrong

// MOTOR DIRECTION
bool INVERT_A = false;
bool INVERT_B = true;

// FALL DETECTION (safety)
const float FALL_ANGLE_DEG = 30.0f;        // shut off if beyond this
const uint16_t FALL_DEBOUNCE_MS = 200;     // must exceed for this long
const float RECOVER_ANGLE_DEG = 10.0f;     // re-enable if within this
const uint16_t RECOVER_DEBOUNCE_MS = 500;  // must be within for this long


// ======== Multiplexer & encoder setup ========
#define TCA_ADDR 0x70
static inline void tcaSelect(uint8_t channel) {
  Wire.beginTransmission(TCA_ADDR);
  Wire.write(1 << channel);
  Wire.endTransmission();
}

#define ENC_CH_A 1
#define ENC_CH_B 0

MagneticSensorI2C sensorA = MagneticSensorI2C(AS5600_I2C);
MagneticSensorI2C sensorB = MagneticSensorI2C(AS5600_I2C);

BLDCMotor motorA(11);
BLDCDriver3PWM driverA(9, 8, 7);

BLDCMotor motorB(11);
BLDCDriver3PWM driverB(12, 11, 10);

// ======== IMU & filter ========
float fused_pitch = 0.0f;
float accel_lp = 0.0f;

// ======== Timing ========
unsigned long last_time = 0;

// ======== Fall detection state ========
bool motors_enabled = true;
unsigned long fall_timer = 0;
unsigned long recover_timer = 0;

// ======== Helper functions ========
static inline void focMotorA() { tcaSelect(ENC_CH_A); motorA.loopFOC(); }
static inline void focMotorB() { tcaSelect(ENC_CH_B); motorB.loopFOC(); }

static inline void disableMotors() {
  motors_enabled = false;

  motorA.move(0);
  motorB.move(0);

  motorA.disable();
  motorB.disable();

  driverA.setPwm(0, 0, 0);
  driverB.setPwm(0, 0, 0);
}

static inline void enableMotors() {
  motorA.enable();
  motorB.enable();
  motors_enabled = true;

  fall_timer = 0;
  recover_timer = 0;
}

void setup() {
  Serial.begin(115200);   // optional; does not block
  Wire.begin();

  // Encoders
  tcaSelect(ENC_CH_A); delay(50); sensorA.init(&Wire);
  tcaSelect(ENC_CH_B); delay(50); sensorB.init(&Wire);

  // Motors
  motorA.linkSensor(&sensorA);
  driverA.voltage_power_supply = 12;
  driverA.init();
  motorA.linkDriver(&driverA);
  motorA.voltage_limit = 10;
  motorA.controller = MotionControlType::velocity;
  motorA.init();
  tcaSelect(ENC_CH_A); motorA.initFOC();

  motorB.linkSensor(&sensorB);
  driverB.voltage_power_supply = 12;
  driverB.init();
  motorB.linkDriver(&driverB);
  motorB.voltage_limit = 10;
  motorB.controller = MotionControlType::velocity;
  motorB.init();
  tcaSelect(ENC_CH_B); motorB.initFOC();

  // IMU
  if (!IMU.begin()) {
    Serial.println("IMU fail");
    while (1);
  }

  fused_pitch = 0.0f;
  accel_lp = 0.0f;
  last_time = millis();

  Serial.println("Balancing only (no BLE).");
}

void loop() {
  unsigned long now = millis();
  float dt = (now - last_time) / 1000.0f;
  if (dt <= 0) dt = 0.001f;
  last_time = now;

  // --- Read IMU ---
  float aX, aY, aZ, gX, gY, gZ;

  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(aX, aY, aZ);
    float accel_pitch = atan2(aX, aZ) * 180.0f / PI;
    accel_lp = accel_alpha * accel_pitch + (1.0f - accel_alpha) * accel_lp;
  }

  float gyro_pitch_rate = 0.0f;
  if (IMU.gyroscopeAvailable()) {
    IMU.readGyroscope(gX, gY, gZ);
    gyro_pitch_rate = GYRO_SIGN * gY;
  }

  // Complementary filter
  fused_pitch = CF_GYRO_WEIGHT * (fused_pitch + gyro_pitch_rate * dt)
              + CF_ACCEL_WEIGHT * accel_lp;

  // Tilt relative to your balanced pose
  float tilt_deg = fused_pitch + imu_offset;

  // --- FALL DETECTION ---
  if (fabs(tilt_deg) > FALL_ANGLE_DEG) {
    if (fall_timer == 0) fall_timer = now;
    if ((now - fall_timer) > FALL_DEBOUNCE_MS && motors_enabled) {
      disableMotors();
      Serial.println("** FALL DETECTED - MOTORS DISABLED **");
    }
    recover_timer = 0;
  } else {
    fall_timer = 0;

    if (!motors_enabled && fabs(tilt_deg) < RECOVER_ANGLE_DEG) {
      if (recover_timer == 0) recover_timer = now;
      if ((now - recover_timer) > RECOVER_DEBOUNCE_MS) {
        enableMotors();
        Serial.println("** RECOVERED - MOTORS ENABLED **");
      }
    } else if (!motors_enabled) {
      recover_timer = 0;
    }
  }

  // --- Run FOC & control ---
  focMotorA();
  focMotorB();

  if (!motors_enabled) {
    delay(5);
    return;
  }

  // PD control (upright target = 0)
  float angle_error = -tilt_deg;
  float cmd = Kp * angle_error - Kd * gyro_pitch_rate;

  // clamp
  if (cmd >  output_limit) cmd =  output_limit;
  if (cmd < -output_limit) cmd = -output_limit;

  motorA.move(INVERT_A ? -cmd : cmd);
  motorB.move(INVERT_B ? -cmd : cmd);
}
