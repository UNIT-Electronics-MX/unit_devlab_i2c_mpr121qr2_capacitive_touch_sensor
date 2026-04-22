/*************************************************
 *  OUTPUT SIGNAL TEST - MPR121QR2 + ESP32-S3
 *
 *  Touch inputs:
 *    E0..E3 (electrode mode)
 *
 *  LED outputs:
 *    ELE8..ELE11 as GPIO0..GPIO3 (push-pull)
 *
 *  Behavior:
 *    - No touch: runs LED chaser test on GPIO0..GPIO3
 *    - Touch E0..E3: mirrors touch state to LEDs
 *************************************************/

#include <Wire.h>

static const uint8_t SDA_PIN = 22;
static const uint8_t SCL_PIN = 23;
static const uint8_t MPR_ADDR = 0x5A;

// MPR121 registers used in this test.
static const uint8_t REG_TOUCH_STATUS_L = 0x00;
static const uint8_t REG_ELECTRODE_CONFIG = 0x5E;
static const uint8_t REG_GPIO_CONTROL_0 = 0x73;
static const uint8_t REG_GPIO_CONTROL_1 = 0x74;
static const uint8_t REG_GPIO_DIRECTION = 0x76;
static const uint8_t REG_GPIO_ENABLE = 0x77;
static const uint8_t REG_GPIO_SET = 0x78;
static const uint8_t REG_GPIO_CLEAR = 0x79;
static const uint8_t REG_GPIO_DATA = 0x75;

static const uint8_t TOUCH_THRESHOLD = 0x20;
static const uint8_t RELEASE_THRESHOLD = 0x10;
static const uint8_t GPIO_ALL_MASK = 0xFF;    // ELE4..ELE11 (bits 0..7)
static const uint8_t DIAG_SWEEP_ORDER[8] = {5, 4, 6, 7, 8, 9, 10, 11};

// Diagnostic mode: force outputs even if touch is not used.
// true  -> sweeps a single HIGH across ELE4..ELE11
// false -> normal behavior (touch mirror + chaser)
static const bool DIAG_FORCE_OUTPUT = true;

uint8_t lastTouchBits = 0;
uint8_t chaseIndex = 0;
unsigned long lastStepMs = 0;

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

uint8_t readReg(uint8_t reg) {
  Wire.beginTransmission(MPR_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false);

  Wire.requestFrom(MPR_ADDR, (uint8_t)1);
  if (Wire.available() < 1) {
    return 0;
  }
  return Wire.read();
}

void gpioWriteMask(uint8_t mask) {
  // touchBits/chaser use 0..3, map to ELE8..ELE11 (bits 4..7).
  uint8_t shiftedMask = (uint8_t)((mask & 0x0F) << 4);
  writeReg(REG_GPIO_CLEAR, GPIO_ALL_MASK);
  writeReg(REG_GPIO_SET, shiftedMask);
}

void printTouchEdges(uint8_t touchBits) {
  for (uint8_t i = 0; i < 4; ++i) {
    uint8_t bit = (1u << i);
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

void mpr121InitTouchAndGpio() {
  // 1) STOP mode
  writeReg(REG_ELECTRODE_CONFIG, 0x00);
  delay(10);

  // 2) Baseline filter registers (datasheet defaults used in examples)
  writeReg(0x2B, 0x01);
  writeReg(0x2C, 0x01);
  writeReg(0x2D, 0x00);
  writeReg(0x2E, 0x00);
  writeReg(0x2F, 0x01);
  writeReg(0x30, 0x01);
  writeReg(0x31, 0xFF);
  writeReg(0x32, 0x02);

  // 3) Thresholds only for E0..E3
  for (uint8_t i = 0; i < 4; ++i) {
    writeReg(0x41 + (i * 2), TOUCH_THRESHOLD);
    writeReg(0x42 + (i * 2), RELEASE_THRESHOLD);
  }

  // 4) ELE8..ELE11 as GPIO outputs
  writeReg(REG_GPIO_CONTROL_0, 0x00);  // GPIO mode for bits [3:0]
  writeReg(REG_GPIO_CONTROL_1, 0x00);  // no pull-up / no alternate function
  writeReg(REG_GPIO_DIRECTION, GPIO_ALL_MASK);  // output direction (1 = output)
  writeReg(REG_GPIO_ENABLE, GPIO_ALL_MASK);  // enable ELE4..ELE11

  // 5) Initial output state LOW
  writeReg(REG_GPIO_CLEAR, GPIO_ALL_MASK);

  // 6) RUN mode with only E0..E3 enabled (avoid GPIO conflict on E4..E11)
  writeReg(REG_ELECTRODE_CONFIG, 0x04);
  delay(20);
}

void setup() {
  Serial.begin(115200);
  delay(200);

  Wire.begin(SDA_PIN, SCL_PIN);
  mpr121InitTouchAndGpio();

  Serial.println("MPR121 output_signal test ready");
  Serial.println("No touch: LED chaser | Touch E0..E3: LED mirror");
  if (DIAG_FORCE_OUTPUT) {
    Serial.println("DIAG MODE: sweeping HIGH from ELE4 to ELE11");
  }
}

void loop() {
  if (DIAG_FORCE_OUTPUT) {
    static uint8_t sweepIndex = 0;
    static unsigned long lastDiagMs = 0;

    unsigned long now = millis();
    if (now - lastDiagMs >= 500) {
      lastDiagMs = now;
      uint8_t electrode = DIAG_SWEEP_ORDER[sweepIndex];
      uint8_t mask = (uint8_t)(1u << (electrode - 4));
      writeReg(REG_GPIO_CLEAR, GPIO_ALL_MASK);
      writeReg(REG_GPIO_SET, mask);

      uint8_t gpioData = readReg(REG_GPIO_DATA);
      Serial.print("GPIO_DATA=0x");
      Serial.print(gpioData, HEX);
      Serial.print(" | ON: ELE");
      Serial.println((int)electrode);

      sweepIndex = (uint8_t)((sweepIndex + 1) & 0x07);
    }

    delay(5);
    return;
  }

  uint16_t touchStatus = readTouchStatus();
  uint8_t touchBits = (uint8_t)(touchStatus & 0x0F);

  printTouchEdges(touchBits);

  if (touchBits != 0) {
    // Manual test: each touch electrode controls one LED.
    gpioWriteMask(touchBits);
    delay(20);
    return;
  }

  // Automatic LED output test sequence when no touch is active.
  unsigned long now = millis();
  if (now - lastStepMs >= 200) {
    lastStepMs = now;
    gpioWriteMask((uint8_t)(1u << chaseIndex));
    chaseIndex = (chaseIndex + 1) & 0x03;
  }

  delay(5);
}
