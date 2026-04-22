# MPR121QR2 Capacitive Touch and GPIO Output Implementation

## Sketches in this folder

- `output_signal.ino`: current diagnostic/tuning sketch used during board bring-up.

Separated examples (one per directory):
- `../output_only_basic/output_only_basic.ino`: basic output-only GPIO test.
- `../input_output_complex/input_output_complex.ino`: combined touch input + GPIO output test.

## Technical Overview

This implementation provides register-level control of the  DevLab I2C MPR121QR2 Capacitive Touch Sensor controller interfaced with the ESP32-S3 microcontroller. The firmware operates without external library dependencies, utilizing direct I2C register access for complete hardware control.

**System Configuration:**
- 4 capacitive sensing channels (E0-E3)
- 4 digital GPIO outputs (ELE8-ELE11)
- Push-pull output drive mode
- Direct I2C communication protocol implementation
- Register-level configuration per MPR121 datasheet specification

**Target Hardware:**
- MPR121QR2 controller with breakout access to electrodes ELE0-ELE11
- ESP32-S3 microcontroller platform
- 3.3V single-supply operation
- I2C Fast Mode compatible (400 kHz maximum)


## Functional Specifications

### Capacitive Touch Sensing

**Active Electrodes:** E0, E1, E2, E3  
**Detection Method:** Polling-based register read (IRQ support optional)  
**Threshold Configuration:** Independent touch and release thresholds per electrode  
**Register Access:** 16-bit status word at address 0x00-0x01  
**Sampling Rate:** Configurable via electrode configuration register (0x5E)

**Threshold Parameters:**
- Touch Threshold (TOU_THRESH): Configurable 0-255
- Release Threshold (REL_THRESH): Configurable 0-255
- Baseline Tracking: Automatic with configurable filters

### GPIO Output Configuration

**Pin Mapping:**
- ELE8  → GPIO0 (Register bit 0)
- ELE9  → GPIO1 (Register bit 1)
- ELE10 → GPIO2 (Register bit 2)
- ELE11 → GPIO3 (Register bit 3)

**Electrical Characteristics:**
- Output Mode: Push-pull CMOS
- Drive Capability: 12mA source/sink maximum
- Logic Levels: VDD = HIGH, GND = LOW
- Output Voltage: Rail-to-rail (0V to VDD)

**Register Control Interface:**
- 0x78 (GPIO Data Set): Write 1 to set output HIGH
- 0x79 (GPIO Data Clear): Write 1 to set output LOW
- 0x7A (GPIO Data Toggle): Write 1 to invert output state
- 0x73 (GPIO Control 0): Mode selection (0 = GPIO, 1 = Electrode)
- 0x76 (GPIO Direction): Direction control (0 = Output, 1 = Input)
- 0x77 (GPIO Enable): Pin enable control (1 = Enabled, 0 = Disabled)

### ESP32-S3 Interface

**I2C Configuration:**
- Master mode operation
- Bus speed: 100 kHz (Standard) or 400 kHz (Fast Mode)
- 7-bit addressing: 0x5A default (configurable 0x5A-0x5D)
- Pull-up resistors: 2.2kΩ - 4.7kΩ external required

**Pin Assignment:**
- SDA: GPIO5 (I2C Data Line)
- SCL: GPIO6 (I2C Clock Line)
- Operating Voltage: 3.3V ±5%

---

## Electrode vs GPIO Mode Priority

The MPR121QR2 architecture implements 8 GPIO-capable pins (ELE4-ELE11). However, the internal multiplexing logic enforces electrode mode as the dominant operating state when both modes are configured simultaneously on the same pin.

**Mode Priority Hierarchy:**
1. Electrode sensing mode (highest priority)
2. GPIO mode (active only when electrode mode disabled)

**Configuration Strategy:**

To achieve deterministic GPIO output behavior, the following electrode allocation is implemented:

- **E0-E3:** Configured as capacitive sensing electrodes
  - Electrode Configuration Register (0x5E) bits [3:0] enabled
  - Touch/release thresholds configured
  - Baseline tracking active

- **E4-E7:** Reserved (not configured)
  - Electrode Configuration Register (0x5E) bits [7:4] disabled
  - Prevents ambiguous pin state

- **ELE8-ELE11:** Configured as GPIO outputs
  - Electrode Configuration Register (0x5E) does not enable these pins
  - GPIO Control Register (0x73) bits [3:0] = 0 (GPIO mode)
  - GPIO Direction Register (0x76) bits [3:0] = 0 (Output)
  - GPIO Enable Register (0x77) bits [3:0] = 1 (Enabled)

**Rationale:**

If electrodes E4-E11 remain enabled in electrode mode, the internal analog front-end will maintain these pins in high-impedance sensing mode, rendering GPIO control registers (0x78, 0x79, 0x7A) ineffective. By explicitly disabling electrode functionality on pins designated for GPIO use, the digital output drivers are activated and register commands execute as specified.


## Hardware Integration

### I2C Bus Connections

Interconnection between ESP32-S3 and MPR121QR2 requires the following signal paths:

| ESP32-S3 Pin | MPR121QR2 Pin | Signal Function        | Electrical Specification       |
|--------------|---------------|------------------------|--------------------------------|
| GPIO5        | SDA           | I2C Data Line          | Open-drain, 3.3V logic level  |
| GPIO6        | SCL           | I2C Clock Line         | Open-drain, 3.3V logic level  |
| 3.3V         | VDD           | Power Supply           | 3.0V - 3.6V, 29µA typical     |
| GND          | GND           | Ground Reference       | Common ground plane           |


### GPIO Output Pinout

Digital outputs are accessed via MPR121QR2 electrode pins configured in GPIO mode:

| GPIO Designation | MPR121 Electrode | PCB Connector | Register Bit Position |
|------------------|------------------|---------------|-----------------------|
| GPIO0            | ELE8             | J6-1          | Bit 0                 |
| GPIO1            | ELE9             | J6-2          | Bit 1                 |
| GPIO2            | ELE10            | J6-3          | Bit 2                 |
| GPIO3            | ELE11            | J6-4          | Bit 3                 |

**Output Specifications:**
- Maximum output current: 12mA per pin
- VOH (output high): VDD - 0.5V minimum @ 12mA
- VOL (output low): 0.5V maximum @ 12mA

### Capacitive Touch Input Pinout

Sensing electrodes require direct connection to touch-sensitive conductive pads:

| Touch Channel | Electrode Pin | PCB Connector | Threshold Register Base |
|---------------|---------------|---------------|-------------------------|
| T0            | E0            | J4-1          | 0x41 (Touch), 0x42 (Release) |
| T1            | E1            | J4-2          | 0x43 (Touch), 0x44 (Release) |
| T2            | E2            | J4-3          | 0x45 (Touch), 0x46 (Release) |
| T3            | E3            | J4-4          | 0x47 (Touch), 0x48 (Release) |

**Electrode Design Guidelines:**
- Minimum pad size: 5mm × 5mm
- Dielectric overlay: 1-3mm plastic/glass
- Ground plane clearance: 1mm minimum
- Trace routing: Keep away from high-frequency signals



## Firmware Initialization Sequence

The configuration procedure executes the following register programming sequence:

### 1. STOP Mode Transition
```
Register 0x5E = 0x00
```
Disables all electrode scanning and places MPR121 in configuration-accessible state. All touch sensing operations cease until RUN mode is re-enabled.

### 2. Baseline Filter Configuration
```
Registers 0x2B-0x32: Baseline tracking parameters
```
Configures Maximum Half Delta (MHD), Noise Half Delta (NHD), Noise Count Limit (NCL), and Filter Delay Limit (FDL) for rising and falling edge detection.

### 3. Touch Threshold Programming (E0-E3 Only)
```
For each electrode i (0-3):
  Register 0x41 + (i × 2) = Touch Threshold
  Register 0x42 + (i × 2) = Release Threshold
```
Establishes capacitance delta thresholds for touch detection. Electrodes E4-E11 thresholds remain unprogrammed (inactive).

### 4. GPIO Mode Enable (ELE8-ELE11)
```
Register 0x73 (GPIO Control 0) = 0x00 (bits [3:0])
Register 0x76 (GPIO Direction)  = 0x00 (bits [3:0])
Register 0x77 (GPIO Enable)     = 0x0F (bits [3:0])
```
Explicitly configures pins ELE8-ELE11 as push-pull digital outputs. Bit value 0 in register 0x73 disables electrode mode on these pins.

### 5. Initial Output State
```
Register 0x79 (GPIO Data Clear) = 0x0F
```
Initializes all GPIO outputs (GPIO0-GPIO3) to logic LOW state.

### 6. RUN Mode Activation
```
Register 0x5E = 0x0C (bits [3:0] = 0b1100)
```
Enables touch sensing on electrodes E0-E3 (4 electrodes). Bits [5:4] enable baseline tracking. Higher bits remain clear to prevent E4-E11 electrode activation.

### 7. Continuous Operation
```
Main Loop:
  - Read touch status (0x00-0x01)
  - Execute GPIO control commands (0x78/0x79/0x7A)
  - Monitor state changes
```

## Touch Detection Protocol

**Status Register Access:**
- Address: 0x00 (LSB), 0x01 (MSB)
- Data Format: 16-bit word, little-endian
- Active Bits: [3:0] corresponding to E0-E3
- Update Rate: Hardware-dependent, typically 10-20ms per complete scan cycle

**Detection Algorithm:**
```
touchStatus = readRegister16(0x00)
electrode0 = touchStatus & 0x0001
electrode1 = touchStatus & 0x0002
electrode2 = touchStatus & 0x0004
electrode3 = touchStatus & 0x0008
```

**Expected Serial Output:**
```
TOUCH E1
TOUCH E3
RELEASE E1
RELEASE E3
```

---

## GPIO Control Interface

**Function Prototypes:**
```cpp
void gpio_set(uint8_t pin);      // Set GPIO pin HIGH
void gpio_clear(uint8_t pin);    // Set GPIO pin LOW
void gpio_toggle(uint8_t pin);   // Invert GPIO pin state
```

**Register Operations:**
```cpp
// Set GPIO0 HIGH
writeRegister(0x78, 0x01);

// Clear GPIO2 LOW
writeRegister(0x79, 0x04);

// Toggle GPIO1
writeRegister(0x7A, 0x02);
```

**Operational Independence:**

GPIO output control executes independently of capacitive touch sensing. Digital output state changes do not affect touch baseline tracking or electrode scan timing. Conversely, touch events do not alter GPIO output states unless explicitly programmed in firmware logic.

---

## Configuration Constraints and Limitations

### Critical Configuration Requirements:

1. **Electrode Mode Priority:**
   - Pins configured as active electrodes (via register 0x5E) cannot function as GPIO
   - Register 0x73 must explicitly disable electrode mode (bit = 0) for GPIO operation
   - Failure to disable electrode mode results in high-impedance output state

2. **Register Write Sequence:**
   - GPIO configuration (0x73, 0x76, 0x77) must occur during STOP mode (0x5E = 0x00)
   - Attempting GPIO configuration during RUN mode may produce undefined behavior
   - Always execute configuration sequence before RUN mode activation

3. **Baseline Tracking Interference:**
   - Electrodes E0-E3 require stable capacitive environment during initialization
   - Rapid touch events during first 500ms may cause baseline instability
   - Allow 1-second settling time after RUN mode activation

4. **I2C Bus Timing:**
   - Minimum inter-register write delay: 100µs (recommended)
   - Bus clock stretching: Not supported by MPR121
   - ACK polling: Required for write operations

