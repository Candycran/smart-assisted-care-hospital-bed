// Smart Assisted-Care Hospital Bed
// Day 1: first sensing path
// Simulated input only — not a medical measurement.

const int HEART_RATE_PIN = A0;

void setup() {
  Serial.begin(115200);
}

void loop() {
  int rawValue = analogRead(HEART_RATE_PIN);

  // Demonstration mapping only:
  // raw ADC 0..1023 -> simulated heart rate 40..180 BPM
  int heartRateBPM = map(rawValue, 0, 1023, 40, 180);

  Serial.print("Raw ADC: ");
  Serial.print(rawValue);
  Serial.print(" | Simulated Heart Rate: ");
  Serial.print(heartRateBPM);
  Serial.println(" BPM");

  delay(500);
}
