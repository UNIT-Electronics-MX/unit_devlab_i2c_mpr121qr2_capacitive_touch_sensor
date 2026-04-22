#include <Wire.h>
#include "USB.h"
#include "USBHIDKeyboard.h"

USBHIDKeyboard Keyboard;

#define SDA_PIN 5
#define SCL_PIN 6

// SOLO USAMOS DOS MÓDULOS
uint8_t MPR[2] = {0x5A, 0x5B};

// INDICA SI EL MÓDULO EXISTE
bool mprPresent[2] = {false, false};

uint16_t prevState[2] = {0, 0};

// ------------ MAPEO DE TECLAS (solo módulo 1 y 2) ------------
struct KeyMap {
  uint8_t module;     // 0 = 0x5A, 1 = 0x5B
  uint8_t electrode;

    
   int keycode;
};

KeyMap keys[] = {
  // ----- MÓDULO 1 (0x5A) – DIRECCIONES Y CONTROL -----
  {0,0,KEY_UP_ARROW},
  {0,1,KEY_DOWN_ARROW},
  {0,2,KEY_LEFT_ARROW},
  {0,3,KEY_RIGHT_ARROW},
  {0,4,' '},               // Space
  {0,5,KEY_RETURN},
  {0,6,KEY_BACKSPACE},
  {0,7,KEY_TAB},
  {0,8,KEY_ESC},

  // ----- MÓDULO 2 (0x5B) – NÚMEROS Y LETRAS -----
  {1,0,'1'},
  {1,1,'2'},
  {1,2,'3'},
  {1,3,'4'},
  {1,4,'5'},
  {1,5,'6'},
  {1,6,'7'},
  {1,7,'8'},
  {1,8,'9'},
  {1,9,'0'},
  {1,10,'Q'},
  {1,11,'W'}
};

const int NKEYS = sizeof(keys) / sizeof(keys[0]);

// ----------------------------------------------------------
// I2C TOOL
// ----------------------------------------------------------
bool testMPR(uint8_t addr) {
  Wire.beginTransmission(addr);
  return (Wire.endTransmission() == 0);
}

void writeReg(uint8_t addr, uint8_t reg, uint8_t val) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}

uint16_t readTouch(uint8_t addr) {
  Wire.beginTransmission(addr);
  Wire.write(0x00);
  Wire.endTransmission(false);

  Wire.requestFrom(addr, (uint8_t)2);
  if (Wire.available() < 2) return 0;

  uint8_t L = Wire.read();
  uint8_t H = Wire.read();
  return (H << 8) | L;
}

// ----------------------------------------------------------
// CONFIGURACIÓN MPR121
// ----------------------------------------------------------
void configMPR(uint8_t addr) {

  writeReg(addr, 0x5E, 0x00); // STOP

  // filtros base
  writeReg(addr, 0x2B, 0x01);
  writeReg(addr, 0x2C, 0x01);
  writeReg(addr, 0x2D, 0x00);
  writeReg(addr, 0x2E, 0x00);
  writeReg(addr, 0x2F, 0x01);
  writeReg(addr, 0x30, 0x01);
  writeReg(addr, 0x31, 0xFF);
  writeReg(addr, 0x32, 0x02);

  // thresholds
  for (int i = 0; i < 12; i++) {
    writeReg(addr, 0x41 + i * 2, 12);
    writeReg(addr, 0x42 + i * 2, 6);
  }

  writeReg(addr, 0x5D, 0x04);
  writeReg(addr, 0x5E, 0x8F); // RUN mode + 12 electrodes
}

// ----------------------------------------------------------
// SETUP
// ----------------------------------------------------------
void setup() {
  USB.begin();
  Keyboard.begin();

  Wire.begin(SDA_PIN, SCL_PIN);
  delay(200);

  // Detectar módulos presentes
  for (int i = 0; i < 2; i++) {
    if (testMPR(MPR[i])) {
      mprPresent[i] = true;
      configMPR(MPR[i]);
      delay(20);
    } else {
      mprPresent[i] = false;
    }
  }
}

// ----------------------------------------------------------
// LOOP PRINCIPAL
// ----------------------------------------------------------
void loop() {

  for (int m = 0; m < 2; m++) {

    // si el módulo NO está presente → saltamos
    if (!mprPresent[m]) continue;

    uint16_t state = readTouch(MPR[m]);
    uint16_t prev  = prevState[m];

    for (int i = 0; i < NKEYS; i++) {
      if (keys[i].module != m) continue;

      uint8_t e = keys[i].electrode;
      int key   = keys[i].keycode;

      bool now = state & (1 << e);
      bool was = prev  & (1 << e);

      if (now && !was) Keyboard.press(key);
      if (!now && was) Keyboard.release(key);
    }

    prevState[m] = state;
  }

  delay(6);
}
