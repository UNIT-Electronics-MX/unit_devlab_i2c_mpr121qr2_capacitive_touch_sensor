# USB HID Keyboard - MPR121

## Technical Overview

This implementation converts the ESP32-S3 TouchDot with MPR121 capacitive touch sensor arrays into a USB HID-compliant keyboard device. The system operates as a native USB peripheral, eliminating the requirement for additional driver software or serial-to-USB conversion.

**Application Domains:**
- Custom input devices and game controllers
- Arcade cabinet interface replacement
- Musical instrument digital interface (MIDI) triggers
- Assistive technology and accessibility devices
- Interactive installation interfaces

## Hardware Requirements

**Microcontroller:**
- ESP32-S3 TouchDot (or equivalent ESP32-S3 with native USB-OTG support)
- USB 2.0 Full Speed (12 Mbps) native peripheral
- Dual-core Xtensa LX7 @ 240 MHz

**Sensor Modules:**
- 2x Freescale/NXP MPR121QR2 capacitive touch sensor controllers
- 24 total capacitive sensing channels (12 per module)
- I2C interface @ 400 kHz (Fast Mode)

**Interface Connections:**
- SDA: GPIO5 (I2C Data Line)
- SCL: GPIO6 (I2C Clock Line)
- VDD: 3.3V ±5% (regulated supply)
- GND: Common ground reference
- Pull-up resistors: 2.2kΩ to 4.7kΩ (typically integrated on breakout boards)

### I2C Address Configuration

MPR121 I2C addresses are configurable via hardware address pins (ADD pin). Standard 7-bit addressing:

| Module | I2C Address | ADD Pin State | Binary  | Functional Assignment    |
|--------|-------------|---------------|---------|--------------------------|
| 1      | 0x5A        | 0000          | 1011010 | Navigation & Control     |
| 2      | 0x5B        | 0001          | 1011011 | Numeric & Alphanumeric   |

**Address Range:** 0x5A to 0x5D (4 possible addresses)  
**Bus Speed:** Standard (100 kHz) or Fast Mode (400 kHz)  
**Electrical:** Open-drain outputs with external pull-ups

## Key Mapping Configuration

### Module 1 (I2C Address: 0x5A) - Navigation & Control Keys
| Electrode | HID Usage ID | Keycode Constant  | Function          |
|-----------|--------------|-------------------|-------------------|
| ELE0      | 0x52         | KEY_UP_ARROW      | Cursor Up         |
| ELE1      | 0x51         | KEY_DOWN_ARROW    | Cursor Down       |
| ELE2      | 0x50         | KEY_LEFT_ARROW    | Cursor Left       |
| ELE3      | 0x4F         | KEY_RIGHT_ARROW   | Cursor Right      |
| ELE4      | 0x2C         | ' ' (Space)       | Spacebar          |
| ELE5      | 0x28         | KEY_RETURN        | Enter/Return      |
| ELE6      | 0x2A         | KEY_BACKSPACE     | Backspace         |
| ELE7      | 0x2B         | KEY_TAB           | Tab               |
| ELE8      | 0x29         | KEY_ESC           | Escape            |

### Module 2 (I2C Address: 0x5B) - Numeric & Alphanumeric Keys
| Electrode | ASCII Code | Character | Electrode | ASCII Code | Character |
|-----------|------------|-----------|-----------|------------|-----------|
| ELE0      | 0x31       | '1'       | ELE6      | 0x37       | '7'       |
| ELE1      | 0x32       | '2'       | ELE7      | 0x38       | '8'       |
| ELE2      | 0x33       | '3'       | ELE8      | 0x39       | '9'       |
| ELE3      | 0x34       | '4'       | ELE9      | 0x30       | '0'       |
| ELE4      | 0x35       | '5'       | ELE10     | 0x51       | 'Q'       |
| ELE5      | 0x36       | '6'       | ELE11     | 0x57       | 'W'       |

## Firmware Configuration and Deployment

### Development Environment Setup

**Arduino IDE Configuration:**

1. **Board Support Package Installation:**
   - Navigate to File → Preferences
   - Add board manager URL:
     ```
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
     ```
   - Install ESP32 platform: Tools → Board → Boards Manager → ESP32 by Espressif Systems

2. **Target Board Selection:**
   - Board: ESP32S3 Dev Module
   - USB CDC On Boot: Disabled
   - USB Firmware MSC On Boot: Disabled
   - USB DFU On Boot: Disabled
   - Upload Mode: UART0 / Hardware CDC
   - USB Mode: USB-OTG (TinyUSB)
   - PSRAM: QSPI PSRAM (if available)
   - Flash Mode: QIO 80MHz
   - Flash Size: 4MB (or available)
   - Partition Scheme: Default 4MB with spiffs
   - Core Debug Level: None (or as required)
   - Arduino Runs On: Core 1
   - Events Run On: Core 1

### Firmware Upload Procedure

1. **Physical Connection:** Connect ESP32-S3 to host computer via USB 2.0 data cable
2. **Serial Port Selection:** Tools → Port → Select appropriate COM/ttyUSB device
3. **Compilation & Upload:** Initiate upload sequence (Ctrl+U or Upload button)
4. **Verification:** Monitor serial output for upload completion confirmation

**Expected Upload Sequence:**
```
Sketch uses XXXXX bytes (XX%) of program storage space
Global variables use XXXXX bytes (XX%) of dynamic memory
esptool.py v4.x.x
Connecting...
Writing at 0x00000000... (100%)
Hash of data verified.
```

### Hardware Integration

**MPR121 Module 1 (Address: 0x5A):**
```
Pin     Connection      Electrical Spec
VDD  →  3.3V           3.0V - 3.6V, 29µA typical
GND  →  GND            Common ground
SDA  →  GPIO5          I2C Data, 400kHz max
SCL  →  GPIO6          I2C Clock, 400kHz max
ADD  →  GND            Address select: 0x5A
IRQ  →  Not connected  Optional interrupt output
```

**MPR121 Module 2 (Address: 0x5B):**
```
Pin     Connection      Electrical Spec
VDD  →  3.3V           Parallel with Module 1
GND  →  GND            Parallel with Module 1
SDA  →  GPIO5          Multi-drop I2C bus
SCL  →  GPIO6          Multi-drop I2C bus
ADD  →  VDD            Address select: 0x5B
IRQ  →  Not connected  Optional interrupt output
```

**Bus Topology:** Multi-drop I2C with 2.2kΩ - 4.7kΩ pull-up resistors on SDA and SCL lines.

## System Operation

### Functional Testing

**Test Procedure:**
1. Open host system text input application (text editor, terminal, IDE)
2. Apply capacitive touch to electrode pads on MPR121 sensor arrays
3. Observe USB HID keyboard events translated to character input

**Expected Behavior:**
- Touch detection latency: 6ms ± 2ms
- Key press event: Transmitted on rising edge of capacitive threshold
- Key release event: Transmitted on falling edge below release threshold
- Multi-touch support: Up to 6 simultaneous key presses (USB HID Boot Protocol limit)

### Application Example: Game Controller Interface

**Directional Input Mapping:**
- ELE0-3 (Module 1, 0x5A): Cardinal direction keys (Arrow keys)
- ELE4 (Module 1): Primary action button (Spacebar)
- ELE5 (Module 1): Secondary action button (Enter/Return)

### Key Mapping Customization

Modify the `KeyMap keys[]` array structure to reconfigure electrode-to-key mappings:

```cpp
struct KeyMap {
  uint8_t module;     // I2C module index (0 = 0x5A, 1 = 0x5B)
  uint8_t electrode;  // Electrode number (0-11)
  int keycode;        // USB HID keycode or ASCII character
};

KeyMap keys[] = {
  {0, 0, KEY_UP_ARROW},    // Module 0, Electrode 0 → Up Arrow
  {0, 4, ' '},             // Module 0, Electrode 4 → Spacebar
  {1, 0, '1'},             // Module 1, Electrode 0 → Numeral 1
};
```

**USB HID Standard Keycodes:**
```cpp
// Navigation Keys (0x4F - 0x52)
KEY_UP_ARROW, KEY_DOWN_ARROW, KEY_LEFT_ARROW, KEY_RIGHT_ARROW

// Control Keys
KEY_RETURN (0x28), KEY_ESC (0x29), KEY_BACKSPACE (0x2A), KEY_TAB (0x2B)

// Modifier Keys (0xE0 - 0xE7)
KEY_LEFT_CTRL, KEY_LEFT_SHIFT, KEY_LEFT_ALT, KEY_LEFT_GUI
KEY_RIGHT_CTRL, KEY_RIGHT_SHIFT, KEY_RIGHT_ALT, KEY_RIGHT_GUI

// Alphanumeric Characters
'A' to 'Z' (0x41-0x5A), '0' to '9' (0x30-0x39), ' ' (0x20)
```

## Device Auto-Detection Protocol

The firmware implements runtime I2C device enumeration to identify connected MPR121 modules:

```cpp
// I2C Bus Scan Algorithm
for (int i = 0; i < 2; i++) {
  Wire.beginTransmission(MPR[i]);
  if (Wire.endTransmission() == 0) {
    mprPresent[i] = true;    // ACK received - device present
    configMPR(MPR[i]);       // Initialize MPR121 configuration
  } else {
    mprPresent[i] = false;   // NACK - no device at this address
  }
}
```

**Dynamic Configuration:**
- Minimum: 1 MPR121 module (12 electrodes)
- Maximum: 2 MPR121 modules (24 electrodes)
- Missing modules are automatically excluded from polling loop
- No firmware recompilation required for different module configurations

## Diagnostic Procedures

### Issue: USB Device Enumeration Failure

**Symptom:** Host operating system does not recognize ESP32-S3 as HID keyboard device

**Root Causes & Solutions:**
1. **USB Cable Specification:** Verify cable supports USB 2.0 data lines (D+ and D-), not power-only cable
2. **Firmware USB Configuration:** Confirm USB Mode = "USB-OTG (TinyUSB)" in Arduino IDE compilation settings
3. **USB Port Power:** Test alternate USB port with minimum 500mA current capability
4. **Device Reset:** Power cycle ESP32-S3 after firmware upload to reinitialize USB stack
5. **Driver Issues:** Check Device Manager (Windows) or dmesg (Linux) for USB enumeration errors

**Verification:**
```bash
# Linux: Check USB device enumeration
lsusb | grep -i esp
dmesg | tail -20

# Windows: Device Manager → Human Interface Devices
# Should show "USB Input Device" or "HID Keyboard Device"
```

### Issue: Capacitive Touch Non-Responsive

**Symptom:** Electrode touch events do not generate USB HID reports

**Diagnostic Steps:**
1. **I2C Communication Test:**
   - Enable serial debug output (115200 baud, 8N1)
   - Verify I2C ACK/NACK responses during initialization
   - Expected: `mprPresent[i] = true` for connected modules

2. **Electrical Verification:**
   - Measure SDA/SCL voltage: Should be 3.3V when idle (pull-up resistors functional)
   - Check VDD supply: 3.0V - 3.6V nominal
   - Verify GND continuity between ESP32-S3 and MPR121 modules

3. **Address Conflict:**
   - Confirm unique I2C addresses: 0x5A and 0x5B
   - Use I2C scanner utility to enumerate bus devices
   - Check ADD pin hardware configuration on MPR121

### Issue: Incorrect Key Mapping

**Symptom:** Electrode activation produces unexpected keycode output

**Resolution:**
1. **Array Index Verification:** Electrode numbering is zero-indexed (ELE0-ELE11)
2. **Module Index:** Verify `module` field in `KeyMap` structure (0 or 1)
3. **I2C Address Correlation:** Module 0 = 0x5A, Module 1 = 0x5B
4. **Keycode Format:** Ensure correct constant or ASCII value in `keycode` field

### Issue: I2C Address Collision

**Symptom:** Both MPR121 modules respond to same address (typically 0x5A)

**Corrective Actions:**
1. **Hardware Configuration:**
   - Module 1: ADD pin → GND (Address 0x5A)
   - Module 2: ADD pin → VDD (Address 0x5B)
2. **DIP Switch Verification:** If using breakout with switches, confirm DIP1 position
3. **Power Cycle:** Reset MPR121 modules after address configuration change
4. **Bus Scan:** Use I2C scanner to confirm unique addresses before running application

## MPR121 Register Configuration

### Initialization Sequence

```cpp
// Transition to STOP mode for configuration access
writeReg(addr, 0x5E, 0x00);

// Baseline Filtering Configuration
writeReg(addr, 0x2B, 0x01);  // MHD_R: Max Half Delta Rising = 1
writeReg(addr, 0x2C, 0x01);  // NHD_R: Noise Half Delta Rising = 1
writeReg(addr, 0x2D, 0x00);  // NCL_R: Noise Count Limit Rising = 0
writeReg(addr, 0x2E, 0x00);  // FDL_R: Filter Delay Limit Rising = 0
writeReg(addr, 0x2F, 0x01);  // MHD_F: Max Half Delta Falling = 1
writeReg(addr, 0x30, 0x01);  // NHD_F: Noise Half Delta Falling = 1
writeReg(addr, 0x31, 0xFF);  // NCL_F: Noise Count Limit Falling = 255
writeReg(addr, 0x32, 0x02);  // FDL_F: Filter Delay Limit Falling = 2

// Electrode Threshold Configuration (ELE0-ELE11)
for (int i = 0; i < 12; i++) {
  writeReg(addr, 0x41 + i*2, 12);  // Touch Threshold (TOU_THRESH)
  writeReg(addr, 0x42 + i*2, 6);   // Release Threshold (REL_THRESH)
}

// Auto-Configuration Registers
writeReg(addr, 0x5D, 0x04);  // AFE Configuration: 6 samples, 16µs charge time

// Enable Run Mode with 12 electrodes
writeReg(addr, 0x5E, 0x8F);  // ECR: 0x8F = 10001111b
                              // Bits[5:4] = 10b: Baseline tracking enabled
                              // Bits[3:0] = 1111b: 12 electrodes enabled (ELE0-ELE11)
```

### Sampling Parameters

**Main Loop Timing:**
```cpp
delay(6);  // 6ms inter-scan delay → ~166 Hz effective scan rate
```

**Timing Trade-offs:**

| Delay (ms) | Scan Rate (Hz) | Latency (ms) | CPU Load | Use Case                    |
|------------|----------------|--------------|----------|-----------------------------|
| 3          | 333            | 3            | High     | High-speed gaming, rhythm   |
| 6          | 166            | 6            | Medium   | Standard keyboard (default) |
| 10         | 100            | 10           | Low      | Low-power, casual input     |
| 16         | 62.5           | 16           | Minimal  | Battery-powered operation   |

**MPR121 Internal Timing:**
- Baseline update rate: 4 samples (configurable)
- Charge/discharge time: 16µs per electrode
- Total scan time (12 electrodes): ~192µs hardware + software overhead

### USB HID Protocol Implementation

**Device Descriptor Configuration:**
- Device Class: 0x00 (Defined at Interface level)
- Interface Class: 0x03 (HID - Human Interface Device)
- Interface Subclass: 0x01 (Boot Interface)
- Interface Protocol: 0x01 (Keyboard)
- Endpoint: IN, Interrupt, 10ms polling interval

**HID Report Descriptor:**
- Report Type: Input Report
- Report Size: 8 bytes
  - Byte 0: Modifier keys bitmap (Ctrl, Shift, Alt, GUI)
  - Byte 1: Reserved (0x00)
  - Bytes 2-7: Key array (6-key rollover, NKRO not implemented)

**Key Event Handling:**
```cpp
// Press event: Rising edge detection
if (now && !was) Keyboard.press(key);

// Release event: Falling edge detection  
if (!now && was) Keyboard.release(key);
```

**USB Compliance:**
- Standard: USB 2.0 Full Speed (12 Mbps)
- Power Consumption: Self-powered or bus-powered (configurable)
- Supported OS: Windows 10/11, macOS 10.15+, Linux kernel 3.x+
- Driver Requirements: None (uses OS-native HID keyboard driver)

## Performance Characteristics

**Temporal Performance:**
- End-to-end latency: 6ms ± 2ms (touch to USB HID report)
- Scan rate: 166 Hz nominal (configurable 62.5 Hz - 333 Hz)
- USB report interval: 10ms (HID specification)
- Touch detection time: ~192µs (MPR121 hardware scan)

**Input Capacity:**
- Total electrodes: 24 (12 per MPR121 module)
- Simultaneous key presses: 6 (USB HID boot protocol limitation)
- Modifier key support: 8 (Ctrl, Shift, Alt, GUI × L/R)
- Key rollover: 6KRO (6-key rollover)

**Signal Processing:**
- Debouncing: Hardware baseline filtering + software edge detection
- False trigger mitigation: Configurable touch/release thresholds
- Noise immunity: Internal digital filtering (MHD, NHD, NCL, FDL registers)

**Power Consumption:**
- ESP32-S3 active mode: 40-50mA @ 240MHz, 3.3V
- MPR121 per module: 29µA typical (standby), 3.4mA peak (scanning)
- Total system: ~50-60mA typical, <100mA peak
- USB bus power: 100mA maximum draw (USB 2.0 specification)

## Application Configuration Examples

### Configuration 1: Directional Game Controller

**Key Assignment:**
```cpp
KeyMap gameController[] = {
  {0, 0, KEY_UP_ARROW},      // ELE0: Forward/Jump
  {0, 1, KEY_DOWN_ARROW},    // ELE1: Crouch/Backward
  {0, 2, KEY_LEFT_ARROW},    // ELE2: Strafe Left
  {0, 3, KEY_RIGHT_ARROW},   // ELE3: Strafe Right
  {0, 4, ' '},               // ELE4: Primary Action (Fire/Select)
  {0, 5, KEY_RETURN},        // ELE5: Pause/Start
  {0, 8, KEY_ESC}            // ELE8: Exit/Menu
};
```

**Typical Latency:** 6-8ms touch-to-input  
**Recommended Scan Rate:** 166 Hz (6ms delay)

### Configuration 2: Musical Keyboard Interface

**Chromatic Scale Mapping:**
```cpp
KeyMap musicalKeys[] = {
  {0, 0, 'C'},  // C4 (Middle C)
  {0, 1, 'D'},  // D4
  {0, 2, 'E'},  // E4
  {0, 3, 'F'},  // F4
  {0, 4, 'G'},  // G4
  {0, 5, 'A'},  // A4
  {0, 6, 'B'},  // B4
  {0, 7, 'c'},  // C5 (uppercase/lowercase for octave distinction)
};
```

**Note:** Requires external MIDI software to map keystrokes to MIDI note-on/note-off events.

### Configuration 3: Macro Pad with Modifier Keys

**Keyboard Shortcut Implementation:**
```cpp
void handleMacroKey(uint8_t module, uint8_t electrode) {
  if (module == 0 && electrode == 0) {
    // Ctrl+C: Copy
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press('C');
    delay(50);
    Keyboard.releaseAll();
  }
  else if (module == 0 && electrode == 1) {
    // Ctrl+V: Paste
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press('V');
    delay(50);
    Keyboard.releaseAll();
  }
  else if (module == 0 && electrode == 2) {
    // Ctrl+Z: Undo
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press('Z');
    delay(50);
    Keyboard.releaseAll();
  }
}
```

**Important:** Requires modification of main loop to call `handleMacroKey()` instead of direct key press/release.

## Technical Specifications Summary

| Parameter                    | Value                                      |
|-----------------------------|--------------------------------------------|
| Microcontroller             | ESP32-S3 (Xtensa LX7 @ 240MHz)            |
| USB Interface               | USB 2.0 Full Speed (12 Mbps)              |
| Sensor IC                   | Freescale/NXP MPR121QR2                   |
| I2C Bus Speed               | 400 kHz (Fast Mode)                        |
| Total Sensing Channels      | 24 (12 per module)                         |
| Scan Rate                   | 166 Hz nominal (62.5 - 333 Hz adjustable) |
| End-to-End Latency          | 6ms ± 2ms                                  |
| USB HID Report Rate         | 10ms (100 Hz)                              |
| Simultaneous Key Presses    | 6 (USB HID limitation)                     |
| Power Consumption           | 50-60mA typical, <100mA peak               |
| Operating Voltage           | 3.3V ± 5%                                  |
| I2C Pull-up Resistors       | 2.2kΩ - 4.7kΩ                              |
| Touch Threshold Range       | 0-255 (configurable per electrode)        |
| Operating System Support    | Windows 10+, macOS 10.15+, Linux 3.x+     |

## References and Documentation

**Hardware Datasheets:**
- ESP32-S3 Technical Reference Manual: [Espressif Documentation](https://www.espressif.com/sites/default/files/documentation/esp32-s3_technical_reference_manual_en.pdf)
- MPR121 Capacitive Touch Sensor: [NXP Datasheet](https://www.nxp.com/docs/en/data-sheet/MPR121.pdf)
- USB HID Specification v1.11: [USB.org](https://www.usb.org/sites/default/files/hid1_11.pdf)

**Software Libraries:**
- TinyUSB Stack: [GitHub Repository](https://github.com/hathach/tinyusb)
- ESP32 Arduino Core: [GitHub Repository](https://github.com/espressif/arduino-esp32)

**Project Resources:**
- Issue Tracker: https://github.com/UNIT-Electronics/devlab_i2c_mpr121qr2_capacitive_touch_sensor/issues
- Documentation: `/docs` directory in repository
- Additional Examples: `/software/examples/cpp_examples`

## License

This firmware implementation is distributed as part of the UNIT Electronics MPR121 development library under the terms specified in the repository LICENSE file.

## Author Information

Developed by UNIT Electronics for the TouchDot ESP32-S3 development platform.

**Repository:** github.com/UNIT-Electronics/devlab_i2c_mpr121qr2_capacitive_touch_sensor  
**Maintainer:** UNIT Electronics Engineering Team  
**Version:** 1.0  
**Last Updated:** December 2025
