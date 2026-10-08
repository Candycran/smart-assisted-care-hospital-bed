// Smart Assisted-Care Hospital Bed
// Day 3A - Input/output wiring test
// Run this BEFORE the toilet state-machine firmware.

#include <Servo.h>

const int PIN_TOILET_BUTTON = 22;
const int PIN_EMERGENCY     = 23;
const int PIN_PATIENT       = 24;
const int PIN_OBSTRUCTION   = 25;
const int PIN_OPEN_LIMIT    = 26;
const int PIN_CLOSED_LIMIT  = 27;

const int PIN_SERVO         = 6;
const int PIN_BUZZER        = 8;
const int PIN_READY_LED     = 30;
const int PIN_FAULT_LED     = 31;

Servo toiletServo;

void setup() {
  Serial.begin(115200);

  pinMode(PIN_TOILET_BUTTON, INPUT_PULLUP);
  pinMode(PIN_EMERGENCY, INPUT_PULLUP);

  pinMode(PIN_PATIENT, INPUT);
  pinMode(PIN_OBSTRUCTION, INPUT);
  pinMode(PIN_OPEN_LIMIT, INPUT);
  pinMode(PIN_CLOSED_LIMIT, INPUT);

  pinMode(PIN_READY_LED, OUTPUT);
  pinMode(PIN_FAULT_LED, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);

  toiletServo.attach(PIN_SERVO);
  toiletServo.write(0);

  digitalWrite(PIN_READY_LED, LOW);
  digitalWrite(PIN_FAULT_LED, LOW);

  Serial.println("DAY 3A - WIRING TEST");
  Serial.println("Buttons: released=HIGH, pressed=LOW");
  Serial.println("Switches: HIGH=active, LOW=inactive");
  Serial.println();
}

void loop() {
  bool toiletPressed = digitalRead(PIN_TOILET_BUTTON) == LOW;
  bool emergencyPressed = digitalRead(PIN_EMERGENCY) == LOW;
  bool patientPresent = digitalRead(PIN_PATIENT) == HIGH;
  bool obstruction = digitalRead(PIN_OBSTRUCTION) == HIGH;
  bool openLimit = digitalRead(PIN_OPEN_LIMIT) == HIGH;
  bool closedLimit = digitalRead(PIN_CLOSED_LIMIT) == HIGH;

  Serial.print("Toilet=");
  Serial.print(toiletPressed ? "PRESSED" : "released");
  Serial.print(" | Emergency=");
  Serial.print(emergencyPressed ? "PRESSED" : "released");
  Serial.print(" | Patient=");
  Serial.print(patientPresent ? "YES" : "NO");
  Serial.print(" | Obstruction=");
  Serial.print(obstruction ? "YES" : "NO");
  Serial.print(" | OpenLimit=");
  Serial.print(openLimit ? "ON" : "OFF");
  Serial.print(" | ClosedLimit=");
  Serial.println(closedLimit ? "ON" : "OFF");

  // Simple indicator check:
  digitalWrite(PIN_READY_LED, patientPresent && !obstruction);
  digitalWrite(PIN_FAULT_LED, obstruction);

  if (emergencyPressed) {
    tone(PIN_BUZZER, 1000);
  } else {
    noTone(PIN_BUZZER);
  }

  delay(300);
}
