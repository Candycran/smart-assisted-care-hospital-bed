// Smart Assisted-Care Hospital Bed
// Day 2A: Multi-vital Serial Monitor test
// Simulation only — NOT clinical measurement.

const int HR_PIN   = A0;
const int SPO2_PIN = A1;
const int TEMP_PIN = A2;
const int RESP_PIN = A3;
const int SYS_PIN  = A4;
const int DIA_PIN  = A5;

void setup() {
  Serial.begin(115200);
}

void loop() {
  int hr = map(analogRead(HR_PIN), 0, 1023, 40, 180);
  int spo2 = map(analogRead(SPO2_PIN), 0, 1023, 80, 100);
  float tempC = map(analogRead(TEMP_PIN), 0, 1023, 340, 405) / 10.0;
  int resp = map(analogRead(RESP_PIN), 0, 1023, 6, 35);
  int systolic = map(analogRead(SYS_PIN), 0, 1023, 80, 180);
  int diastolic = map(analogRead(DIA_PIN), 0, 1023, 40, 120);

  Serial.print("HR: "); Serial.print(hr); Serial.print(" BPM | ");
  Serial.print("SpO2: "); Serial.print(spo2); Serial.print("% | ");
  Serial.print("BP: "); Serial.print(systolic); Serial.print("/"); Serial.print(diastolic); Serial.print(" mmHg | ");
  Serial.print("Temp: "); Serial.print(tempC, 1); Serial.print(" C | ");
  Serial.print("Resp: "); Serial.print(resp); Serial.println(" /min");

  delay(500);
}
