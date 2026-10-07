#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

// Smart Assisted-Care Hospital Bed — Day 2
// Simulation only — NOT clinical measurement.

const int TFT_CS  = 53;
const int TFT_DC  = 49;
const int TFT_RST = 48;
Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RST);

const int HR_PIN   = A0;
const int SPO2_PIN = A1;
const int TEMP_PIN = A2;
const int RESP_PIN = A3;
const int SYS_PIN  = A4;
const int DIA_PIN  = A5;

int hr, spo2, resp, systolic, diastolic;
float tempC;
bool warningState;

unsigned long lastReadMs = 0;
unsigned long lastDrawMs = 0;

bool engineeringWarning() {
  return hr < 50 || hr > 120 || spo2 < 92 || tempC < 35.5 || tempC > 38.0 ||
         resp < 10 || resp > 24 || systolic < 90 || systolic > 140 ||
         diastolic < 60 || diastolic > 90;
}

void readVitals() {
  hr = map(analogRead(HR_PIN), 0, 1023, 40, 180);
  spo2 = map(analogRead(SPO2_PIN), 0, 1023, 80, 100);
  tempC = map(analogRead(TEMP_PIN), 0, 1023, 340, 405) / 10.0;
  resp = map(analogRead(RESP_PIN), 0, 1023, 6, 35);
  systolic = map(analogRead(SYS_PIN), 0, 1023, 80, 180);
  diastolic = map(analogRead(DIA_PIN), 0, 1023, 40, 120);
  warningState = engineeringWarning();
}

void drawMonitor() {
  tft.fillScreen(ILI9341_BLACK);
  tft.fillRect(0, 0, 320, 28, ILI9341_NAVY);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(8, 7);
  tft.print("PATIENT MONITOR");

  tft.setTextSize(1);
  tft.setTextColor(ILI9341_LIGHTGREY);
  tft.setCursor(10, 42); tft.print("HEART RATE");
  tft.setCursor(120, 42); tft.print("SpO2");
  tft.setCursor(220, 42); tft.print("BLOOD PRESSURE");

  tft.setTextSize(3);
  tft.setTextColor(ILI9341_GREEN);
  tft.setCursor(10, 56); tft.print(hr);
  tft.setTextSize(1); tft.print(" BPM");

  tft.setTextSize(3);
  tft.setTextColor(ILI9341_CYAN);
  tft.setCursor(120, 56); tft.print(spo2);
  tft.setTextSize(1); tft.print(" %");

  tft.setTextSize(2);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setCursor(220, 58); tft.print(systolic); tft.print("/"); tft.print(diastolic);
  tft.setTextSize(1); tft.setCursor(230, 80); tft.print("mmHg");

  tft.setTextColor(ILI9341_LIGHTGREY);
  tft.setCursor(10, 106); tft.print("TEMPERATURE");
  tft.setCursor(160, 106); tft.print("RESPIRATION");

  tft.setTextSize(2);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(10, 122); tft.print(tempC, 1); tft.print(" C");
  tft.setCursor(160, 122); tft.print(resp);
  tft.setTextSize(1); tft.print(" /min");

  if (warningState) {
    tft.fillRect(10, 165, 300, 32, ILI9341_RED);
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(2);
    tft.setCursor(43, 173);
    tft.print("ENGINEERING WARNING");
  } else {
    tft.fillRect(10, 165, 300, 32, ILI9341_DARKGREEN);
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(2);
    tft.setCursor(79, 173);
    tft.print("VALUES STABLE");
  }

  tft.setTextColor(ILI9341_LIGHTGREY);
  tft.setTextSize(1);
  tft.setCursor(40, 220);
  tft.print("SIMULATION ONLY - NOT CLINICAL DATA");
}

void printSerial() {
  Serial.print("HR="); Serial.print(hr);
  Serial.print(" | SpO2="); Serial.print(spo2);
  Serial.print(" | BP="); Serial.print(systolic); Serial.print("/"); Serial.print(diastolic);
  Serial.print(" | Temp="); Serial.print(tempC, 1);
  Serial.print(" | Resp="); Serial.print(resp);
  Serial.print(" | Warning="); Serial.println(warningState ? "YES" : "NO");
}

void setup() {
  Serial.begin(115200);
  tft.begin();
  tft.setRotation(1);
  readVitals();
  drawMonitor();
}

void loop() {
  unsigned long now = millis();

  if (now - lastReadMs >= 100) {
    lastReadMs = now;
    readVitals();
  }

  if (now - lastDrawMs >= 500) {
    lastDrawMs = now;
    drawMonitor();
    printSerial();
  }
}
