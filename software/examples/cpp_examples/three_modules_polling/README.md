# Three Modules Polling

Main sketch: three_modules_polling.ino

Description:
- Three MPR121 modules on the same I2C bus.
- Polling-based detection (no IRQ required).
- Independent touch/release tracking for each module.

Default setup:
- Module addresses: 0x5A, 0x5B, 0x5C
- SDA: GPIO5
- SCL: GPIO6
