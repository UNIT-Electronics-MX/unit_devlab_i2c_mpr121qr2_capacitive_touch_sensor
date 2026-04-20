# Hardware

<div align="center">
<a href="./unit_sch_v_0_0_1ue0116_mpr121qr2_capacitive_touch_sensor.pdf"><img src="resources/Schematics_icon.jpg?raw=false" width="200px"><br/>Schematic</a>
</div>

## Key Technical Specifications

<div align="center">

| **Parameter** |               **Description**                | **Min** | **Typ** | **Max** | **Unit** |
|:-------------:|:--------------------------------------------:|:-------:|:-------:|:-------:|:--------:|
|      Vcc      |     Input voltage to power on the module     |   2.5   |   3.3   |    5    |    V     |
|      Vin      |         Input voltage for MPR121QR2          |   2.5   |   3.3   |    5    |    V     |
|      Vih      |       High-level input voltage for I2C       | 0.7xVin |    -    |    -    |    V     |
|      Vil      |       Low-level input voltage for I2C        |    -    |    -    | 0.3xVin |    V     |
|      Vol      |   Low-level output voltage for SDA and IRQ   |    -    |    -    |  0.5V   |    V     |
|    Vohgpio    | Output High Voltage ELE4 - ELE11 (GPIO mode) | Vin-0.5 |    -    |    -    |    V     |
|    Volgpio    | Output High Voltage ELE4 - ELE11 (GPIO mode) |    -    |    -    |   0.5   |    V     |
|      Icc      |                Supply Current                |    -    |    -    |   393   |    uA    |
|     fscl      |            Serial Clock Frequency            |    -    |    -    |   400   |   kHz    |
 
</div>

### DIP Switch Configuration

<div align="center">

| DIP1 | DIP2 | DIP3 | Address |
|:----:|:----:|:----:|:-------:|
| OFF  | OFF  | OFF  | 0x5A    |
| ON   | OFF  | OFF  | 0x5B    |
| OFF  | ON   | OFF  | 0x5C    |
| OFF  | OFF  | ON   | 0x5D    |

</div>

**Note:** Only activate ONE DIP switch at a time.

## Pinout

<div align="center">
    <a href="unit_pinout_v_1_0_0_ue00116_12C_MPR121QR2_capacitive_touch_sensor_en.pdf"><img src="./resources/unit_pinout_v_1_0_0_ue00116_12C_MPR121QR2_capacitive_touch_sensor_en.jpg" width="500px"><br/>Pinout</a>
    <br/>
    <br/>
    <br/>
    

| Pin Label | Function    | Notes                             |
|-----------|-------------|-----------------------------------|
| VCC       | Power Supply| 3.3V or 5V                       |
| GND       | Ground      | Common ground for all components  |

</div>

## Pin & Connector Layout
| Pin   | Voltage Level | Function                                                  |
|-------|---------------|-----------------------------------------------------------|
| VCC   | 3.3 V – 5.5 V | Provides power to the on-board regulator and sensor core. |
| GND   | 0 V           | Common reference for power and signals.                   |
| SDA   | 1.8 V to VCC  | Serial data line for I²C communications.                  |
| SCL   | 1.8 V to VCC  | Serial clock line for I²C communications.                 |

> **Note:** The module also includes a Qwiic/STEMMA QT connector carrying the same four signals (VCC, GND, SDA, SCL) for effortless daisy-chaining.

## Topology

<div align="center">
<a href="./resources/unit_topology_v_0_0_1ue0116_mpr121qr2_capacitive_touch_sensor.png"><img src="./resources/unit_topology_v_0_0_1ue0116_mpr121qr2_capacitive_touch_sensor.png" width="500px"><br/>Topology</a>
<br/>
<br/>
<br/>

| Ref.  | Description                                    |
|-------|------------------------------------------------|
| IC1   | MPR121QR2                                      |
| L1    | Power On LED                                   |
| U1    | AP2112K 3.3V Regulator                         |
| JP1   | 2.54 mm Pin Headers for Control Signals        |
| JP2   | 2.54 mm Pin Headers for Electrodes or LEDs     |
| J1-J2 | QWIIC Connector for I2C (JST 4-Pin 1mm)        |
| J3-J5 | JST 4-Pin 1mm Connector for Electrodes or LEDs |
| SW1   | Dip Switch for Address Selection               |

</div>

## Dimensions

<div align="center">
<a href="./resources/unit_dimensions_v_0_0_1ue0116_mpr121qr2_capacitive_touch_sensor.png"><img src="./resources/unit_dimensions_v_0_0_1ue0116_mpr121qr2_capacitive_touch_sensor.png" width="500px"><br/> Dimensions</a>
</div>

# References

- <a href="./resources/unit_datasheet_v_0_0_1ue0116_mpr121qr2.pdf">MPR121QR2 Datasheet</a>
