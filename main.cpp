#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define S1_TRIG 5
#define S1_ECHO 19
#define S2_TRIG 16
#define S2_ECHO 17
#define S3_TRIG 13
#define S3_ECHO 14
#define S4_TRIG 27
#define S4_ECHO 26
#define ENTRY_PIR 34
#define EXIT_PIR 35
#define OLED_SDA 21
#define OLED_SCL 22
#define SERVO_PIN 18
#define GREEN_LED 25
#define YELLOW_LED 33
#define RED_LED 32
#define BUZZER_PIN 23
#define BUTTON_PIN 4
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
const int servoChannel = 0;
const int servoFrequency = 50;
const int servoResolution = 16;
bool systemRunning = false;
bool lastButtonState = HIGH;
unsigned long lastButtonTime = 0;
const unsigned long debounceDelay = 250;
bool slot1Occupied = false, slot2Occupied = false, slot3Occupied = false, slot4Occupied = false;
int availableSlots = 4;

float readDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW); delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 30000);
  if (duration == 0) return 999;
  return duration * 0.0343 / 2.0;
}

void setGateAngle(int angle) {
  int duty = map(angle, 0, 180, 1638, 8192);
  ledcWrite(servoChannel, duty);
}

void allLEDOff() {
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);
}

void updateSlots() {
  const float occupiedLimit = 20.0;
  slot1Occupied = readDistance(S1_TRIG, S1_ECHO) <= occupiedLimit;
  slot2Occupied = readDistance(S2_TRIG, S2_ECHO) <= occupiedLimit;
  slot3Occupied = readDistance(S3_TRIG, S3_ECHO) <= occupiedLimit;
  slot4Occupied = readDistance(S4_TRIG, S4_ECHO) <= occupiedLimit;
  availableSlots = (!slot1Occupied) + (!slot2Occupied) + (!slot3Occupied) + (!slot4Occupied);
}

void updateParkingStatus() {
  allLEDOff();
  if (availableSlots == 0) digitalWrite(RED_LED, HIGH);
  else if (availableSlots <= 2) digitalWrite(YELLOW_LED, HIGH);
  else digitalWrite(GREEN_LED, HIGH);
}

void showParkingDisplay() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("SMART PARKING");
  display.println("----------------");
  display.print("S1: "); display.println(slot1Occupied ? "OCCUPIED" : "FREE");
  display.print("S2: "); display.println(slot2Occupied ? "OCCUPIED" : "FREE");
  display.print("S3: "); display.println(slot3Occupied ? "OCCUPIED" : "FREE");
  display.print("S4: "); display.println(slot4Occupied ? "OCCUPIED" : "FREE");
  display.print("AVAILABLE: "); display.println(availableSlots);
  display.print("STATUS: ");
  if (availableSlots == 0) display.println("FULL");
  else if (availableSlots <= 2) display.println("LIMITED");
  else display.println("AVAILABLE");
  display.display();
}

void setup() {
  Serial.begin(115200);
  pinMode(S1_TRIG, OUTPUT); pinMode(S1_ECHO, INPUT);
  pinMode(S2_TRIG, OUTPUT); pinMode(S2_ECHO, INPUT);
  pinMode(S3_TRIG, OUTPUT); pinMode(S3_ECHO, INPUT);
  pinMode(S4_TRIG, OUTPUT); pinMode(S4_ECHO, INPUT);
  pinMode(ENTRY_PIR, INPUT); pinMode(EXIT_PIR, INPUT);
  pinMode(GREEN_LED, OUTPUT); pinMode(YELLOW_LED, OUTPUT); pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT); pinMode(BUTTON_PIN, INPUT_PULLUP);
  ledcSetup(servoChannel, servoFrequency, servoResolution);
  ledcAttachPin(SERVO_PIN, servoChannel);
  setGateAngle(0);
  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) Serial.println("OLED ERROR");
  else Serial.println("OLED OK");
  allLEDOff(); digitalWrite(GREEN_LED, HIGH); noTone(BUZZER_PIN);
  display.clearDisplay(); display.setTextColor(SSD1306_WHITE); display.setTextSize(1);
  display.setCursor(0, 8); display.println("SMART PARKING");
  display.setCursor(0, 25); display.println("MANAGEMENT SYSTEM");
  display.setCursor(0, 45); display.println("PRESS BUTTON");
  display.display();
  Serial.println("==============================");
  Serial.println("SMART PARKING SYSTEM");
  Serial.println("SYSTEM READY");
  Serial.println("PRESS BUTTON TO START");
  Serial.println("==============================");
  delay(2000);
}

void loop() {
  bool buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == LOW && lastButtonState == HIGH && millis() - lastButtonTime > debounceDelay) {
    systemRunning = !systemRunning;
    lastButtonTime = millis();
    if (systemRunning) Serial.println("SYSTEM: RUNNING");
    else { Serial.println("SYSTEM: STOPPED"); setGateAngle(0); noTone(BUZZER_PIN); }
  }
  lastButtonState = buttonState;

  if (!systemRunning) {
    allLEDOff(); digitalWrite(GREEN_LED, HIGH); noTone(BUZZER_PIN); setGateAngle(0);
    display.clearDisplay(); display.setTextColor(SSD1306_WHITE); display.setTextSize(1); display.setCursor(0, 5);
    display.println("SMART PARKING"); display.println("----------------"); display.println("SYSTEM: STOPPED"); display.println(); display.println("PRESS BUTTON"); display.println("TO START"); display.display();
    delay(300); return;
  }

  updateSlots(); updateParkingStatus();
  bool entryDetected = digitalRead(ENTRY_PIR);
  bool exitDetected = digitalRead(EXIT_PIR);

  if (entryDetected) {
    Serial.println("ENTRY: VEHICLE DETECTED");
    if (availableSlots > 0) {
      Serial.println("ENTRY: GATE OPEN"); noTone(BUZZER_PIN); setGateAngle(90); delay(1500); setGateAngle(0); Serial.println("ENTRY: GATE CLOSED");
    } else {
      Serial.println("PARKING FULL - ENTRY DENIED"); tone(BUZZER_PIN, 2000, 800); setGateAngle(0);
    }
  }

  if (exitDetected) {
    Serial.println("EXIT: VEHICLE DETECTED"); setGateAngle(90); delay(1200); setGateAngle(0); Serial.println("EXIT: GATE CLOSED");
  }

  showParkingDisplay();
  Serial.print("AVAILABLE SLOTS: "); Serial.println(availableSlots);
  Serial.print("S1: "); Serial.print(slot1Occupied ? "OCCUPIED" : "FREE");
  Serial.print(" | S2: "); Serial.print(slot2Occupied ? "OCCUPIED" : "FREE");
  Serial.print(" | S3: "); Serial.print(slot3Occupied ? "OCCUPIED" : "FREE");
  Serial.print(" | S4: "); Serial.println(slot4Occupied ? "OCCUPIED" : "FREE");
  delay(500);
}
