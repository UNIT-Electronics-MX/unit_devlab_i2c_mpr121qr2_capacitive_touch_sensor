# Input Output Complex

Main sketch: input_output_complex.ino

Description:
- Combined MPR121 input/output example.
- Touch inputs on E0..E3.
- GPIO outputs on ELE4..ELE11.
- High bank (ELE8..ELE11) mirrors touch state.
- Low bank (ELE4..ELE7) shows a dynamic pattern.

Current hardware configuration in this sketch:
- SDA: GPIO22
- SCL: GPIO23
- MPR121: 0x5A
