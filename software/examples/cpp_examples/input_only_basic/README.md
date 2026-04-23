# Input Only Basic

Main sketch: input_only_basic.ino

Description:
- Basic touch input test for MPR121.
- Reads touch state on E0..E11 (polling, no IRQ).
- Prints `TOUCH Ex` / `RELEASE Ex` events on the Serial Monitor.

Current hardware configuration in this sketch:
- SDA: GPIO22
- SCL: GPIO23
- MPR121: 0x5A
- Serial: 115200 baud
