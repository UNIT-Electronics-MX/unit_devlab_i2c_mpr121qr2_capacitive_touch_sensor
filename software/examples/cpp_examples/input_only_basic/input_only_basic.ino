/*************************************************
 * BASIC INPUT TEST - MPR121QR2 + ESP32-S3
 *
 * Purpose:
 * - Touch inputs only (no GPIO outputs)
 * - Reads touch status on E0..E11 and prints
 *   TOUCH / RELEASE events on the Serial Monitor
 *
 * I2C:
 * - SDA = GPIO22
 * - SCL = GPIO23
 * - MPR121 address = 0x5A
 *************************************************/

#include <Wire.h>

static const uint8_t SDA_PIN = 22;
static const uint8_t SCL_PIN = 23;
static const uint8_t MPR_ADDR = 0x5A;

static const uint8_t REG_TOUCH_STATUS_L = 0x00;
static const uint8_t REG_ELECTRODE_CONFIG = 0x5E;

static const uint8_t TOUCH_THRESHOLD = 0x20;
static const uint8_t RELEASE_THRESHOLD = 0x10;

static const uint8_t NUM_ELECTRODES = 12;

uint16_t lastTouchBits = 0;

void writeReg(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(MPR_ADDR);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
  delayMicroseconds(100);
}

uint16_t readTouchStatus() {
  Wire.beginTransmission(MPR_ADDR);
  Wire.write(REG_TOUCH_STATUS_L);
  Wire.endTransmission(false);

  Wire.requestFrom(MPR_ADDR, (uint8_t)2);
  if (Wire.available() < 2) {
    return 0;
  }

  uint8_t lsb = Wire.read();
  uint8_t msb = Wire.read();
  return (uint16_t)lsb | ((uint16_t)msb << 8);
}

void printTouchEdges(uint16_t touchBits) {
  for (uint8_t i = 0; i < NUM_ELECTRODES; ++i) {
    uint16_t bit = (uint16_t)(1u << i);
    bool prev = (lastTouchBits & bit) != 0;
    bool curr = (touchBits & bit) != 0;

    if (!prev && curr) {
      Serial.print("TOUCH E");
      Serial.println(i);
    }

    if (prev && !curr) {
      Serial.print("RELEASE E");
      Serial.println(i);
    }
  }

  lastTouchBits = touchBits;
}

void initTouchOnly() {
  // 1) STOP mode
  writeReg(REG_ELECTRODE_CONFIG, 0x00);
  delay(10);

  // 2) Baseline filter configuration
  writeReg(0x2B, 0x01);
  writeReg(0x2C, 0x01);
  writeReg(0x2D, 0x00);
  writeReg(0x2E, 0x00);
  writeReg(0x2F, 0x01);
  writeReg(0x30, 0x01);
  writeReg(0x31, 0xFF);
  writeReg(0x32, 0x02);

  // 3) Touch / release thresholds for E0..E11
  for (uint8_t i = 0; i < NUM_ELECTRODES; ++i) {
    writeReg((uint8_t)(0x41 + (i * 2)), TOUCH_THRESHOLD);
    writeReg((uint8_t)(0x42 + (i * 2)), RELEASE_THRESHOLD);
  }

  // 4) RUN mode with all 12 electrodes enabled (0x0C = 12)
  writeReg(REG_ELECTRODE_CONFIG, 0x0C);
  delay(20);
}

void setup() {
  Serial.begin(115200);
  delay(200);

  Wire.begin(SDA_PIN, SCL_PIN);
  initTouchOnly();

  Serial.println("Basic input-only test ready");
  Serial.println("Touch electrodes E0..E11 to see events");
}

void loop() {
  uint16_t touchStatus = readTouchStatus();
  uint16_t touchBits = (uint16_t)(touchStatus & 0x0FFF);

  printTouchEdges(touchBits);

  delay(10);
}
