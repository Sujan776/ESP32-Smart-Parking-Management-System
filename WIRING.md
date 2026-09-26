# Smart Parking Wiring

## Ultrasonic Sensors
S1: VCC->5V, GND->GND, TRIG->GPIO5, ECHO->GPIO19
S2: VCC->5V, GND->GND, TRIG->GPIO16, ECHO->GPIO17
S3: VCC->5V, GND->GND, TRIG->GPIO13, ECHO->GPIO14
S4: VCC->5V, GND->GND, TRIG->GPIO27, ECHO->GPIO26

## PIR Sensors
Entry: VCC->3V3, OUT->GPIO34, GND->GND
Exit: VCC->3V3, OUT->GPIO35, GND->GND

## OLED
VCC->3V3, GND->GND, SDA->GPIO21, SCL->GPIO22

## Servo
PWM->GPIO18, V+->5V, GND->GND

## LEDs
Green->GPIO25 through 220 ohm resistor; cathode->GND
Yellow->GPIO33 through 220 ohm resistor; cathode->GND
Red->GPIO32 through 220 ohm resistor; cathode->GND

## Buzzer
+->GPIO23, -->GND

## Pushbutton
One side->GPIO4, opposite side->GND
