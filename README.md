# ESP32 Smart Parking Management System

ESP32-based smart parking management prototype simulated in Wokwi and developed with PlatformIO.

## Features
- Four parking slots using HC-SR04 ultrasonic sensors
- Entry and exit detection using PIR sensors
- Automatic available-slot counting
- OLED real-time parking status
- Green / Yellow / Red status LEDs
- Servo-controlled entry/exit gate
- Buzzer alert when parking is full
- Start/Stop pushbutton
- Serial monitoring at 115200 baud

## GPIO Mapping

| Function | GPIO |
|---|---:|
| Slot 1 TRIG / ECHO | 5 / 19 |
| Slot 2 TRIG / ECHO | 16 / 17 |
| Slot 3 TRIG / ECHO | 13 / 14 |
| Slot 4 TRIG / ECHO | 27 / 26 |
| Entry PIR | 34 |
| Exit PIR | 35 |
| OLED SDA / SCL | 21 / 22 |
| Servo | 18 |
| Green LED | 25 |
| Yellow LED | 33 |
| Red LED | 32 |
| Buzzer | 23 |
| Start/Stop button | 4 |

## Parking Logic
- Distance <= 20 cm: slot occupied
- More than 2 free slots: AVAILABLE / green
- 1-2 free slots: LIMITED / yellow
- 0 free slots: FULL / red
- Entry opens when a vehicle is detected and space is available
- Entry is denied with buzzer alert when full
- Exit opens the gate when the exit PIR detects a vehicle

## Tools
ESP32, Embedded C/C++, PlatformIO, Wokwi, Arduino framework, OLED, HC-SR04, PIR, servo/PWM.

## Author
Sri Sujan VM
