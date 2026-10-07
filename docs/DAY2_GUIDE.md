# Day 2 Guide — Complete Patient Monitoring Subsystem

## Build rule
Do not paste a finished diagram.json. Add and wire every component manually in Wokwi.

## Stage A — Expand the analogue input bank
Keep the Day 1 heart-rate potentiometer on A0.

Add five more slide potentiometers:

| Signal | Arduino Mega pin |
|---|---|
| Heart rate | A0 |
| SpO2 | A1 |
| Temperature | A2 |
| Respiratory rate | A3 |
| Systolic pressure | A4 |
| Diastolic pressure | A5 |

Use a breadboard power rail to keep the layout clean:
- Mega 5V -> breadboard + rail
- Mega GND -> breadboard - rail
- each potentiometer VCC -> + rail
- each potentiometer GND -> - rail
- each potentiometer SIG -> its assigned analogue pin

## Stage B — Multi-vital Serial Monitor test
Load `firmware/day2_multivitals_serial.ino`.
Move one slider at a time and confirm only the intended value changes.
Do not move to Stage C until all six inputs are correct.

## Stage C — Add the bedside TFT
Add one ILI9341 2.8-inch TFT to the right of the Mega.

| TFT pin | Mega pin |
|---|---|
| VCC | 5V |
| GND | GND |
| CS | 53 |
| D/C | 49 |
| RST | 48 |
| MOSI | 51 |
| SCK | 52 |

MISO is not required for this display-only Day 2 build.

Add these Wokwi libraries:
- Adafruit GFX Library
- Adafruit ILI9341

Then load `firmware/day2_tft_patient_monitor.ino`.

## Stage D — Verify the monitor
The screen should show HR, SpO2, blood pressure, temperature, respiratory rate, and an overall engineering status.
Move each slider and verify the corresponding display changes.

## Stage E — MATLAB
Open `simulation/matlab/day2_patient_vitals_analysis.m` in MATLAB and run it.
It generates synthetic 60-second patient data, inserts demonstration abnormal segments, plots the signals, and exports a CSV.

## End-of-day evidence
Save a clean Wokwi circuit screenshot, Serial Monitor screenshot, TFT screenshot, MATLAB plot screenshot, and completed test sheet.
