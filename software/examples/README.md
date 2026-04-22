# C/C++ Firmware Examples

This folder contains validated C/C++ firmware examples for the **DevLab: I2C MPR121QR2 Capacitive Touch Sensor** module.

## Technical Validation Summary

**Validation Date:** December 11, 2025  
**Responsible:** Cesar Bautista  
**Platform:** ESP32-S3 @ 3.3V  
**IDE:** Arduino IDE  

### ✔ Validated Features:
- Full capacitive touch detection (E0–E11)
- Stable I2C communication at 3.3V
- DIP switch address selection (0x5A, 0x5B, 0x5C, 0x5D)
- All connector types: JST 1.0mm (Qwiic), JST standard, 2.54mm header
- IRQ interrupt functionality

### ⚠ Important Notes:
- **Only activate ONE DIP switch at a time**
- No DIP active = default address 0x5A
- Multiple DIPs active = undefined behavior (no I2C response)

---

## Examples Overview

### 1. Single Module with IRQ (`cpp_examples/single_module_irq/single_module_irq.ino`)

**Description:**  
This example demonstrates the basic usage of one MPR121 module using the **IRQ (Interrupt Request)** pin for efficient touch detection. Instead of constantly polling the sensor, the ESP32 only reads touch data when the IRQ pin goes low, indicating a change in touch status.

**Key Features:**
- Uses hardware interrupts for power-efficient operation
- Detects both touch and release events
- Works with default I2C address (0x5A)
- Real-time serial output for debugging

**Hardware Configuration:**
```
ESP32-S3 Pins:
├── SDA: Pin 5
├── SCL: Pin 6
└── IRQ: Pin 47

MPR121 Module:
└── I2C Address: 0x5A (no DIP switch active)
```

**How it Works:**
1. Configures MPR121 with default sensitivity thresholds
2. Attaches interrupt to IRQ pin (triggers on FALLING edge)
3. When a touch occurs, IRQ goes low → interrupt fires
4. Main loop reads touch status register (0x00) only when IRQ event detected
5. Compares current state with previous state to detect changes
6. Prints "TOUCHED" or "RELEASED" for each electrode (0-11)

**Use Cases:**
- Battery-powered applications (low power consumption)
- Single sensor installations
- Learning basic MPR121 operation
- Projects requiring fast response times

---

### 2. Three Modules without IRQ (`cpp_examples/three_modules_polling/three_modules_polling.ino`)

**Description:**  
This example shows how to use **three MPR121 modules simultaneously** on the same I2C bus, using different addresses configured via DIP switches. It demonstrates polling-based detection without IRQ pins, making wiring simpler when using multiple sensors.

**Key Features:**
- Manages 3 independent MPR121 sensors (36 total touch inputs!)
- Uses DIP switch address selection
- Polling-based detection (no IRQ required)
- Individual state tracking for each module

**Hardware Configuration:**
```
ESP32-S3 Pins:
├── SDA: Pin 5 (shared by all modules)
└── SCL: Pin 6 (shared by all modules)

Module 0:
├── I2C Address: 0x5A
└── DIP Switch: None active

Module 1:
├── I2C Address: 0x5B
└── DIP Switch: DIP1 ON

Module 2:
├── I2C Address: 0x5C
└── DIP Switch: DIP2 ON
```

**How it Works:**
1. Initializes all three MPR121 modules with unique addresses
2. Creates a 2D array to track touch states [module][electrode]
3. Continuously polls each module in sequence (40ms interval)
4. Reads 16-bit touch status register from each module
5. Compares states and prints changes with module identifier
6. Output format: `[M0] TOUCHED 3` (Module 0, Electrode 3)

**Use Cases:**
- Multi-zone touch interfaces (up to 36 inputs)
- Musical instruments with multiple octaves
- Advanced control panels
- Interactive art installations

---

## DIP Switch Address Configuration

| DIP1 | DIP2 | DIP3 | I2C Address | Status |
|:----:|:----:|:----:|:-----------:|:------:|
| OFF  | OFF  | OFF  | **0x5A**    | ✔ Default |
| ON   | OFF  | OFF  | **0x5B**    | ✔ Validated |
| OFF  | ON   | OFF  | **0x5C**    | ✔ Validated |
| OFF  | OFF  | ON   | **0x5D**    | ✔ Validated |
| ON   | ON   | OFF  | Invalid     | ❌ Do not use |
| ON   | OFF  | ON   | Invalid     | ❌ Do not use |
| OFF  | ON   | ON   | Invalid     | ❌ Do not use |
| ON   | ON   | ON   | Invalid     | ❌ Do not use |

**⚠ CRITICAL:** Only activate ONE DIP switch at a time. Multiple active DIPs cause undefined I2C address and module failure.

---

## Common Functions Explained

Both examples share core functions for MPR121 communication:

### `leerTouch(int addr)` - Read Touch Status
```cpp
uint16_t leerTouch(int addr)
```
Reads the 16-bit touch status register (0x00) from the MPR121.  
**Returns:** Bitmask where each bit represents one electrode (bit 0 = E0, bit 11 = E11)

### `mpr121_Config(int addr)` - Initialize Sensor
```cpp
void mpr121_Config(int addr)
```
Configures MPR121 with optimal settings:
- Sets filtering and debounce parameters
- Configures touch/release thresholds (0x20/0x10)
- Enables all 12 electrodes
- Starts continuous touch detection mode

### `setReg(uint8_t addr, uint8_t reg, uint8_t val)` - Write Register
```cpp
void setReg(uint8_t addr, uint8_t reg, uint8_t val)
```
Writes a single byte to a specific MPR121 register via I2C.

---

## Getting Started

### Requirements:
- ESP32-S3 development board
- MPR121 DevLab module(s)
- Arduino IDE with ESP32 board support
- 3.3V power supply

### Installation Steps:
1. Connect hardware according to example pinout
2. Configure DIP switches (if using multiple modules)
3. Open desired `.ino` file in Arduino IDE
4. Select board: **ESP32S3 Dev Module**
5. Upload and open Serial Monitor (115200 baud)

### Expected Output:
```
MPR121 listo con IRQ.
TOCADO: 5
LIBERADO: 5
TOCADO: 8
```

---

## Troubleshooting

| Problem | Solution |
|---------|----------|
| No serial output | Check Serial Monitor baud rate (115200) |
| Module not responding | Verify I2C address matches DIP configuration |
| Unstable readings | Ensure only ONE DIP is active |
| IRQ not working | Check pin 47 connection and pull-up resistor |
| Multiple modules conflict | Verify each has unique address |

---

## Validation Results

All examples were validated on **ESP32-S3** with the following results:

| Test | Status |
|------|:------:|
| Single module @ 0x5A | ✔ Pass |
| Address 0x5B (DIP1) | ✔ Pass |
| Address 0x5C (DIP2) | ✔ Pass |
| Address 0x5D (DIP3) | ✔ Pass |
| IRQ interrupt detection | ✔ Pass |
| 3 modules simultaneously | ✔ Pass |
| All connector types | ✔ Pass |
| Multiple DIPs active | ❌ Fail (expected) |

**Test Environment:**
- Controller: ESP32-S3
- Voltage: 3.3V only
- I2C Pins: SDA=5, SCL=6
- No external pull-ups required (ESP32 internal pull-ups)

---

## Additional Resources

- [MPR121 Datasheet](https://www.nxp.com/docs/en/data-sheet/MPR121.pdf)
- [Hardware Documentation](../../hardware/)
- [MicroPython Examples](../mp/)

---

## License

See [LICENSE](../../LICENSE) file for details.
