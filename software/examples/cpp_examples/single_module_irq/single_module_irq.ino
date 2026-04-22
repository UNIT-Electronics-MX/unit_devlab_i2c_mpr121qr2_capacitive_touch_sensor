/*************************************************
 *   EJEMPLO 1 — MPR121 con IRQ (solo módulo 0)
 *   ESP32-S3: SDA=5, SCL=6, IRQ=47
 *   
 *   Todos los electrodos E0-E11 como entradas táctiles
 *   
 *   Para desactivar IRQ y usar polling:
 *   Comenta la línea: #define USE_IRQ
 *************************************************/

#include <Wire.h>

#define SDA_PIN 5
#define SCL_PIN 6
#define MPR 0x5A

//#define USE_IRQ        // Comentar esta línea para usar polling sin IRQ

#ifdef USE_IRQ
  #define IRQ_PIN 47   // IRQ del MPR121
  volatile bool irq_event = false;
#endif

bool estadoAnterior[12];

// ---------------- IRQ HANDLER ----------------
#ifdef USE_IRQ
  void IRAM_ATTR irqHandler() {
    irq_event = true;
  }
#endif

void setup() {
  Serial.begin(115200);
  delay(200);

  Wire.begin(SDA_PIN, SCL_PIN);

  for (int i = 0; i < 12; i++) estadoAnterior[i] = false;

  // IMPORTANTE: Configurar MPR121 ANTES de habilitar IRQ
  mpr121_Config(MPR);

  #ifdef USE_IRQ
    pinMode(IRQ_PIN, INPUT_PULLUP);  
    attachInterrupt(IRQ_PIN, irqHandler, FALLING);
    Serial.println("MPR121 listo con IRQ.");
  #else
    Serial.println("MPR121 listo con POLLING (sin IRQ).");
  #endif
}

void loop() {

  #ifdef USE_IRQ
    // Modo IRQ: solo leer cuando hay interrupción
    if (irq_event) {
      irq_event = false;
      
      uint16_t touched = leerTouch(MPR);

      for (int i = 0; i < 12; i++) {
        bool actual = bitRead(touched, i);

        if (!estadoAnterior[i] && actual) {
          Serial.print("TOCADO: "); Serial.println(i);
        }

        if (estadoAnterior[i] && !actual) {
          Serial.print("LIBERADO: "); Serial.println(i);
        }

        estadoAnterior[i] = actual;
      }
    }
    delay(5);
    
  #else
    // Modo POLLING: leer constantemente
    uint16_t touched = leerTouch(MPR);

    for (int i = 0; i < 12; i++) {
      bool actual = bitRead(touched, i);

      if (!estadoAnterior[i] && actual) {
        Serial.print("TOCADO: "); Serial.println(i);
      }

      if (estadoAnterior[i] && !actual) {
        Serial.print("LIBERADO: "); Serial.println(i);
      }

      estadoAnterior[i] = actual;
    }
    delay(40);  // Delay más largo para polling
  #endif
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
  delay(10);

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
  
  delay(10);
}

void setReg(uint8_t addr, uint8_t reg, uint8_t val) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
  delayMicroseconds(100);  // Delay pequeño entre escrituras I2C
}
