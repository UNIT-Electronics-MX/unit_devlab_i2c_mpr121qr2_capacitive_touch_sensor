/*************************************************
 * BASIC OUTPUT TEST - MPR121QR2 + ESP32-S3
 *
 * Purpose:
 * - GPIO outputs only (no touch logic)
 * - Sweeps one output at a time on ELE4..ELE11
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

static const uint8_t REG_ELECTRODE_CONFIG = 0x5E;
static const uint8_t REG_GPIO_CONTROL_0 = 0x73;
static const uint8_t REG_GPIO_CONTROL_1 = 0x74;
static const uint8_t REG_GPIO_DIRECTION = 0x76;
static const uint8_t REG_GPIO_ENABLE = 0x77;
static const uint8_t REG_GPIO_SET = 0x78;
static const uint8_t REG_GPIO_CLEAR = 0x79;
static const uint8_t REG_GPIO_DATA = 0x75;

static const uint8_t GPIO_MASK_ALL = 0xFF;  // ELE4..ELE11
static const uint8_t SWEEP_ORDER[8] = {5, 4, 6, 7, 8, 9, 10, 11};

void writeReg(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(MPR_ADDR);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
  delayMicroseconds(100);
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

void setOutputsRaw(uint8_t rawMask) {
  writeReg(REG_GPIO_CLEAR, GPIO_MASK_ALL);
  writeReg(REG_GPIO_SET, rawMask);
}

void initGpioOnly() {
  // Stop touch engine so all electrode channels remain free for GPIO.
  writeReg(REG_ELECTRODE_CONFIG, 0x00);
  delay(10);

  writeReg(REG_GPIO_CONTROL_0, 0x00);  // GPIO mode
  writeReg(REG_GPIO_CONTROL_1, 0x00);  // no pull-up
  writeReg(REG_GPIO_DIRECTION, GPIO_MASK_ALL);  // 1 = output
  writeReg(REG_GPIO_ENABLE, GPIO_MASK_ALL);     // enable ELE4..ELE11

  setOutputsRaw(0x00);
}

void setup() {
  Serial.begin(115200);
  delay(200);

  Wire.begin(SDA_PIN, SCL_PIN);
  initGpioOnly();

  Serial.println("Basic output-only test ready");
}

void loop() {
  for (uint8_t i = 0; i < 8; ++i) {
    uint8_t electrode = SWEEP_ORDER[i];
    uint8_t mask = (uint8_t)(1u << (electrode - 4));

    setOutputsRaw(mask);

    uint8_t data = readReg(REG_GPIO_DATA);
    Serial.print("ON ELE");
    Serial.print((int)electrode);
    Serial.print(" | GPIO_DATA=0x");
    Serial.println(data, HEX);

    delay(10);
  }

  // Quick all-on/all-off confirmation.
  setOutputsRaw(GPIO_MASK_ALL);
  delay(250);
  setOutputsRaw(0x00);
  delay(250);
}
