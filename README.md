
# DevLab: I2C MPR121QR2 Capacitive Touch Sensor

<div align="center">
  <img src="./hardware/resources/unit_top_v_0_0_1ue0116_mpr121qr2_capacitive_touch_sensor.png" width="450px" alt="DevLab MPR121 Capacitive Touch Sensor">
</div>

<div align="center">

### Quick Start

[<img src="https://img.shields.io/badge/Product%20Wiki-blue?style=for-the-badge" alt="Product Wiki">](https://wiki.uelectronics.com/wiki/devlab_i2c_mpr121qr2_capacitive_touch_sensor)
[<img src="https://img.shields.io/badge/Datasheet-green?style=for-the-badge" alt="Datasheet">](https://www.nxp.com/docs/en/data-sheet/MPR121.pdf)
[<img src="https://img.shields.io/badge/Buy%20Now-orange?style=for-the-badge" alt="Buy Now">](#)
[<img src="https://img.shields.io/badge/Getting%20Started-purple?style=for-the-badge" alt="Getting Started">](docs/)

</div>

## Overview

A development module featuring the NXP MPR121 capacitive touch sensor controller with 12 independent touch inputs, I2C communication, and configurable addressing via DIP switch.

**Key Specifications:**
- 12 capacitive touch electrodes (E0–E11)
- I2C interface (0x5A, 0x5B, 0x5C, 0x5D)
- 3.0V – 3.6V operation (3.3V recommended)
- IRQ interrupt support
- Triple connector options: Qwiic, JST, 2.54mm headers

## Quick Start

### Hardware Connection
```
Controller       MPR121 Module
----------       -------------
3.3V      -----> VCC
GND       -----> GND
SDA       -----> SDA
SCL       -----> SCL
IRQ       -----> IRQ (optional)
```

### DIP Switch Configuration

| DIP1 | DIP2 | DIP3 | Address |
|:----:|:----:|:----:|:-------:|
| OFF  | OFF  | OFF  | 0x5A    |
| ON   | OFF  | OFF  | 0x5B    |
| OFF  | ON   | OFF  | 0x5C    |
| OFF  | OFF  | ON   | 0x5D    |

**Note:** Only activate ONE DIP switch at a time.

## Applications

- Touch-sensitive interfaces and control panels
- Musical instruments (keyboards, MIDI controllers)
- Interactive art installations
- Robotics and wearable devices
- Industrial sealed controls

## Resources

- [Complete Examples & Documentation](software/examples/)
- [Hardware Documentation](hardware/)
- [Schematic Diagram](hardware/)
- [MPR121 Datasheet (NXP)](https://www.nxp.com/docs/en/data-sheet/MPR121.pdf)
- [Getting Started Guide](docs/)

## 📝 License

All hardware and documentation in this project are licensed under the **MIT License**.  
See [`LICENSE.md`](LICENSE.md) for details.

<div align="center">
  <sub>Template created by UNIT Electronics</sub>
</div>

