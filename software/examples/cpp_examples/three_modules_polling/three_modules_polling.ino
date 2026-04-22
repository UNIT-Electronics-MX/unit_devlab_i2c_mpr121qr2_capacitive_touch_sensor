/*************************************************
 *  EJEMPLO 2 — 3 MPR121 SOLO I2C (SIN IRQ)
 *  ESP32-S3: SDA=5, SCL=6
 *************************************************/

#include <Wire.h>

#define SDA_PIN 5
#define SCL_PIN 6

int direcciones[3] = {0x5A, 0x5B, 0x5C};
bool estadoAnterior[3][12];

void setup() {
  Serial.begin(115200);
  delay(200);

  Wire.begin(SDA_PIN, SCL_PIN);

  for (int m = 0; m < 3; m++)
    for (int i = 0; i < 12; i++)
      estadoAnterior[m][i] = false;

  for (int m = 0; m < 3; m++) {
    mpr121_Config(direcciones[m]);
    Serial.print("Módulo "); Serial.print(m);
    Serial.print(" configurado en 0x");
    Serial.println(direcciones[m], HEX);
  }

  Serial.println("3 MPR121 listos solo por I2C.");
}

void loop() {

  for (int m = 0; m < 3; m++) {

    uint16_t touched = leerTouch(direcciones[m]);

    for (int i = 0; i < 12; i++) {

      bool actual = bitRead(touched, i);

      if (!estadoAnterior[m][i] && actual) {
        Serial.print("[M"); Serial.print(m);
        Serial.print("] TOCADO "); Serial.println(i);
      }

      if (estadoAnterior[m][i] && !actual) {
        Serial.print("[M"); Serial.print(m);
        Serial.print("] LIBERADO "); Serial.println(i);
      }

      estadoAnterior[m][i] = actual;
    }
  }

  delay(40);  // recomendado para el MPR121
}


// ---------------- FUNCIONES ----------------
uint16_t leerTouch(int addr) {
  Wire.beginTransmission(addr);
  Wire.write(0x00);
  Wire.endTransmission(false);

  Wire.requestFrom(addr, 2);
  if (Wire.available() < 2) return 0;

  return (Wire.read() | (Wire.read() << 8));
}

void mpr121_Config(int addr) {
  setReg(addr, 0x5E, 0x00);

  setReg(addr, 0x2B, 0x01);
  setReg(addr, 0x2C, 0x01);
  setReg(addr, 0x2D, 0x00);
  setReg(addr, 0x2E, 0x00);
  setReg(addr, 0x2F, 0x01);
  setReg(addr, 0x30, 0x01);
  setReg(addr, 0x31, 0xFF);
  setReg(addr, 0x32, 0x02);

  uint8_t touch = 0x20;
  uint8_t release = 0x10;

  for (int i = 0; i < 12; i++) {
    setReg(addr, 0x41 + (i * 2), touch);
    setReg(addr, 0x42 + (i * 2), release);
  }

  setReg(addr, 0x5D, 0x04);
  setReg(addr, 0x5E, 0x0C);
}

void setReg(uint8_t addr, uint8_t reg, uint8_t val) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}
