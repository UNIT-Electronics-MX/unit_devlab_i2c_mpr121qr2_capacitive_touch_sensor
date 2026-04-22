# Single Module IRQ

Main sketch: single_module_irq.ino

Description:
- Single MPR121 module example.
- Uses IRQ pin for event-driven touch detection.
- Tracks touch and release for electrodes E0..E11.

Default setup:
- I2C address: 0x5A
- SDA: GPIO5
- SCL: GPIO6
- IRQ: GPIO47
