/**
 * Arduino SparkFun ICM-20948 9DoF IMU Test
 * Minimal example to verify IMU functionality
 * I2C pins: SDA=21, SCL=22 (default ESP32)
 */
#include <Arduino.h>
#include "ICM_20948.h"
#include <math.h>

#define WIRE_PORT Wire
#define I2C_SDA 21
#define I2C_SCL 22
#define I2C_FREQ_HZ 100000
#define I2C_FAIL_THRESHOLD 4
#define NO_SAMPLE_TIMEOUT_MS 800
#define I2C_HARD_RESTART_DELAY_MS 250
#define MA_WINDOW_SIZE 10
#define MA_EPSILON 0.05

uint8_t activeAd0 = 0;
uint8_t i2cErrorCount = 0;
uint32_t lastGoodSampleMs = 0;

ICM_20948_I2C myICM;

struct MovingAverage {
  float buffer[MA_WINDOW_SIZE];
  float sum = 0.0f;
  uint8_t head = 0;
  uint8_t count = 0;

  float add(float value) {
    if (count == MA_WINDOW_SIZE) {
      sum -= buffer[head];
    } else {
      count++;
    }

    buffer[head] = value;
    sum += value;
    head = (head + 1) % MA_WINDOW_SIZE;

    return sum / count;
  }
};

MovingAverage accAvg[3], gyrAvg[3], magAvg[3];
float lastAcc[3] = {NAN, NAN, NAN};
float lastGyr[3] = {NAN, NAN, NAN};
float lastMag[3] = {NAN, NAN, NAN};

inline bool changed(float a, float b) {
  if (isnan(b)) return true;
  return fabs(a - b) > MA_EPSILON;
}

void scanI2C() {
  Serial.println("Scanning I2C bus...");
  byte devicesFound = 0;

  for (byte address = 1; address < 127; address++) {
    WIRE_PORT.beginTransmission(address);
    byte error = WIRE_PORT.endTransmission();

    if (error == 0) {
      Serial.print("Found I2C device at 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.println();
      devicesFound++;
    }
  }

  if (devicesFound == 0) {
    Serial.println("No I2C devices found.");
  } else {
    Serial.print("Total devices found: ");
    Serial.println(devicesFound);
  }
  Serial.println();
}

void initI2CBus() {
  WIRE_PORT.begin(I2C_SDA, I2C_SCL);
  WIRE_PORT.setClock(I2C_FREQ_HZ);
}

void recoverI2CBus() {
  Serial.println("[I2C] attempting bus recovery...");

  WIRE_PORT.end();
  pinMode(I2C_SDA, INPUT_PULLUP);
  pinMode(I2C_SCL, INPUT_PULLUP);
  delay(5);

  // clock pulse recovery (if SDA is stuck low)
  pinMode(I2C_SCL, OUTPUT);
  for (uint8_t i = 0; i < 9; i++) {
    digitalWrite(I2C_SCL, HIGH);
    delayMicroseconds(5);
    digitalWrite(I2C_SCL, LOW);
    delayMicroseconds(5);
  }
  pinMode(I2C_SCL, INPUT_PULLUP);

  initI2CBus();
}

bool initIMUWithAD0(uint8_t ad0Val) {
  Serial.print("Trying AD0 = ");
  Serial.print(ad0Val);
  Serial.print(" (I2C addr: 0x");
  uint8_t i2cAddr = (0x68 | (ad0Val & 0x01));
  Serial.print(i2cAddr, HEX);
  Serial.println(")");

  myICM.begin(WIRE_PORT, ad0Val);
  if (myICM.status == ICM_20948_Stat_Ok) {
    Serial.print("IMU init ok on AD0 = ");
    Serial.println(ad0Val);
    activeAd0 = ad0Val;
    i2cErrorCount = 0;
    return true;
  }

  Serial.print("IMU init failed on AD0 = ");
  Serial.print(ad0Val);
  Serial.print(" -> ");
  Serial.println(myICM.statusString());
  return false;
}

void setup() {
  Serial.begin(115200);
  delay(100);
  
  Serial.println("\n\nICM-20948 9DoF IMU Test");
  Serial.println("=======================\n");
  
  // Initialize I2C
  initI2CBus();

  // Optional first check: scan the bus for active addresses
  scanI2C();
  
  // Initialize IMU with I2C address (AD0 pin determines if 0x68 or 0x69)
  Serial.println("Checking IMU addresses...");
  bool ok = false;
  if (initIMUWithAD0(0)) {
    ok = true;
  } else if (initIMUWithAD0(1)) {
    ok = true;
  } else {
    Serial.println("ICM-20948 initialization failed on both AD0 = 0 and AD0 = 1.");
    while (1);
  }

  if (!ok) {
    while (1);
  }
  
  Serial.println("IMU initialized successfully!");
  lastGoodSampleMs = millis();
  delay(500);
}

bool recoverIMU() {
  Serial.println("[IMU] recovering...");
  recoverI2CBus();

  // Try both AD0 possibilities in case wiring / floating AD0 changed
  return initIMUWithAD0(0) || initIMUWithAD0(1);
}

void hardRestart() {
  Serial.println("[IMU] persistent I2C failure, restarting ESP32...");
  Serial.flush();
  delay(I2C_HARD_RESTART_DELAY_MS);
  ESP.restart();
}

void loop() {
  // Read sensor data
  if (myICM.dataReady()) {
    myICM.getAGMT();  // Get all sensor data

    if (myICM.status != ICM_20948_Stat_Ok) {
      i2cErrorCount++;
      Serial.print("[I2C] read error: ");
      Serial.println(myICM.statusString());
      Serial.print("[I2C] consecutive errors: ");
      Serial.println(i2cErrorCount);

  if (i2cErrorCount >= I2C_FAIL_THRESHOLD) {
    if (!recoverIMU()) {
      hardRestart();
    } else {
      Serial.println("[IMU] recovery succeeded.");
          i2cErrorCount = 0;
          lastGoodSampleMs = millis();
        }
      }

      return;
    }

    i2cErrorCount = 0;
    
    float accVals[3] = { float(myICM.accX()), float(myICM.accY()), float(myICM.accZ()) };
    float gyrVals[3] = { float(myICM.gyrX()), float(myICM.gyrY()), float(myICM.gyrZ()) };
    float magVals[3] = { float(myICM.magX()), float(myICM.magY()), float(myICM.magZ()) };

    float avgAcc[3];
    float avgGyr[3];
    float avgMag[3];

    for (uint8_t i = 0; i < 3; i++) {
      avgAcc[i] = accAvg[i].add(accVals[i]);
      avgGyr[i] = gyrAvg[i].add(gyrVals[i]);
      avgMag[i] = magAvg[i].add(magVals[i]);
    }

    bool shouldPrint =
      changed(avgAcc[0], lastAcc[0]) || changed(avgAcc[1], lastAcc[1]) || changed(avgAcc[2], lastAcc[2]) ||
      changed(avgGyr[0], lastGyr[0]) || changed(avgGyr[1], lastGyr[1]) || changed(avgGyr[2], lastGyr[2]) ||
      changed(avgMag[0], lastMag[0]) || changed(avgMag[1], lastMag[1]) || changed(avgMag[2], lastMag[2]);

    if (shouldPrint) {
      // Teleplot-friendly one-value-per-line output:
      Serial.print(">acc_x:");
      Serial.println(avgAcc[0], 3);
      Serial.print(">acc_y:");
      Serial.println(avgAcc[1], 3);
      Serial.print(">acc_z:");
      Serial.println(avgAcc[2], 3);

      Serial.print(">gyr_x:");
      Serial.println(avgGyr[0], 3);
      Serial.print(">gyr_y:");
      Serial.println(avgGyr[1], 3);
      Serial.print(">gyr_z:");
      Serial.println(avgGyr[2], 3);

      Serial.print(">mag_x:");
      Serial.println(avgMag[0], 3);
      Serial.print(">mag_y:");
      Serial.println(avgMag[1], 3);
      Serial.print(">mag_z:");
      Serial.println(avgMag[2], 3);

      memcpy(lastAcc, avgAcc, sizeof(lastAcc));
      memcpy(lastGyr, avgGyr, sizeof(lastGyr));
      memcpy(lastMag, avgMag, sizeof(lastMag));
    }

    lastGoodSampleMs = millis();
  }

  if ((millis() - lastGoodSampleMs) > NO_SAMPLE_TIMEOUT_MS) {
    Serial.println("[I2C] no sample for too long, forcing restart");
    hardRestart();
  }

  delay(100);  // 10 Hz update rate
}
