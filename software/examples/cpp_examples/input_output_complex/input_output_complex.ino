/*************************************************
 * COMPLEX INPUT+OUTPUT TEST - MPR121QR2 + ESP32-S3
 *
 * Inputs:
 * - Touch on E0..E3
 *
 * Outputs:
 * - GPIO on ELE4..ELE11
 *
 * Behavior:
 * - ELE8..ELE11 mirror touch state (E0..E3)
 * - ELE4..ELE7 show a dynamic status pattern
 *************************************************/

#include <Wire.h>

static const uint8_t SDA_PIN = 22;
static const uint8_t SCL_PIN = 23;
static const uint8_t MPR_ADDR = 0x5A;

static const uint8_t REG_TOUCH_STATUS_L = 0x00;
static const uint8_t REG_ELECTRODE_CONFIG = 0x5E;
static const uint8_t REG_GPIO_CONTROL_0 = 0x73;
static const uint8_t REG_GPIO_CONTROL_1 = 0x74;
static const uint8_t REG_GPIO_DIRECTION = 0x76;
static const uint8_t REG_GPIO_ENABLE = 0x77;
static const uint8_t REG_GPIO_SET = 0x78;
static const uint8_t REG_GPIO_CLEAR = 0x79;

static const uint8_t TOUCH_THRESHOLD = 0x20;
static const uint8_t RELEASE_THRESHOLD = 0x10;

static const uint8_t GPIO_MASK_ALL = 0xFF;   // ELE4..ELE11
static const uint8_t GPIO_MASK_LOW = 0x0F;   // ELE4..ELE7
static const uint8_t GPIO_MASK_HIGH = 0xF0;  // ELE8..ELE11

uint8_t lastTouchBits = 0;
uint8_t chaseIndex = 0;
unsigned long lastPatternMs = 0;

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

void setOutputsRaw(uint8_t rawMask) {
  writeReg(REG_GPIO_CLEAR, GPIO_MASK_ALL);
  writeReg(REG_GPIO_SET, rawMask);
}

void applyOutputBanks(uint8_t low4, uint8_t high4) {
  uint8_t raw = (uint8_t)((low4 & 0x0F) | ((high4 & 0x0F) << 4));
  setOutputsRaw(raw);
}

void printTouchEdges(uint8_t touchBits) {
  for (uint8_t i = 0; i < 4; ++i) {
    uint8_t bit = (uint8_t)(1u << i);
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

uint8_t getSingleTouchIndex(uint8_t touchBits) {
  for (uint8_t i = 0; i < 4; ++i) {
    if (touchBits == (uint8_t)(1u << i)) {
      return i;
    }
  }
  return 0xFF;
}

void initTouchAndGpio() {
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

  // 3) Touch thresholds for E0..E3
  for (uint8_t i = 0; i < 4; ++i) {
    writeReg((uint8_t)(0x41 + (i * 2)), TOUCH_THRESHOLD);
    writeReg((uint8_t)(0x42 + (i * 2)), RELEASE_THRESHOLD);
  }

  // 4) GPIO on ELE4..ELE11
  writeReg(REG_GPIO_CONTROL_0, 0x00);
  writeReg(REG_GPIO_CONTROL_1, 0x00);
  writeReg(REG_GPIO_DIRECTION, GPIO_MASK_ALL);  // 1 = output
  writeReg(REG_GPIO_ENABLE, GPIO_MASK_ALL);

  // 5) Initial outputs low
  writeReg(REG_GPIO_CLEAR, GPIO_MASK_ALL);

  // 6) RUN mode with only E0..E3 enabled
  writeReg(REG_ELECTRODE_CONFIG, 0x04);
  delay(20);
}

void setup() {
  Serial.begin(115200);
  delay(200);

  Wire.begin(SDA_PIN, SCL_PIN);
  initTouchAndGpio();

  Serial.println("Complex input/output test ready");
  Serial.println("High bank(ELE8..ELE11)=touch mirror | Low bank(ELE4..ELE7)=status pattern");
}

void loop() {
  uint16_t touchStatus = readTouchStatus();
  uint8_t touchBits = (uint8_t)(touchStatus & 0x0F);

  printTouchEdges(touchBits);

  // High bank mirrors touch E0..E3 directly.
  uint8_t highBank = touchBits;

  // Low bank shows richer status behavior.
  uint8_t lowBank = 0x00;
  unsigned long now = millis();

  if (touchBits == 0) {
    // No touch: running chaser on ELE4..ELE7.
    if (now - lastPatternMs >= 180) {
      lastPatternMs = now;
      chaseIndex = (uint8_t)((chaseIndex + 1) & 0x03);
    }
    lowBank = (uint8_t)(1u << chaseIndex);
  } else {
    uint8_t single = getSingleTouchIndex(touchBits);
    if (single != 0xFF) {
      // One finger: indicate index on low bank.
      lowBank = (uint8_t)(1u << single);
    } else {
      // Multi-touch: low bank all ON.
      lowBank = GPIO_MASK_LOW;
    }
  }

  applyOutputBanks(lowBank, highBank);
  delay(10);
}
