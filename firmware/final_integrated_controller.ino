#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Servo.h>

// Smart Assisted-Care Hospital Bed - Final Integrated Controller (Timing-Hardened)
// Engineering simulation only; not clinical data or validated sanitation.
// Final-day timing fix: actuator motion is now robust to TFT rendering load.

const int TFT_CS=53, TFT_DC=49, TFT_RST=48;
Adafruit_ILI9341 tft(TFT_CS,TFT_DC,TFT_RST);

const int HR_PIN=A0, SPO2_PIN=A1, TEMP_PIN=A2, RESP_PIN=A3, SYS_PIN=A4, DIA_PIN=A5;
const int PIN_TOILET_BUTTON=22, PIN_EMERGENCY=23, PIN_PATIENT=24, PIN_OBSTRUCTION=25;
const int PIN_OPEN_LIMIT=26, PIN_CLOSED_LIMIT=27;
const int PIN_WATER_AVAILABLE=28, PIN_DRAIN_AVAILABLE=29, PIN_LEAK_DETECTED=36;
const int PIN_CAREGIVER_RESET=37;
const int PIN_TOILET_SERVO=6, PIN_WASTE_SERVO=7, PIN_BUZZER=8;
const int PIN_READY_LED=30, PIN_FAULT_LED=31;
const int PIN_FLUSH_OUTPUT=32, PIN_RINSE_OUTPUT=33, PIN_DRAIN_OUTPUT=34, PIN_SANITATION_LED=35;

Servo toiletServo, wasteServo;
const int TOILET_CLOSED_ANGLE=0, TOILET_OPEN_ANGLE=90;
const int WASTE_CLOSED_ANGLE=0, WASTE_OPEN_ANGLE=90;
int toiletCommandedAngle=TOILET_CLOSED_ANGLE;

const unsigned long SERVO_STEP_MS=35, MOVEMENT_TIMEOUT_MS=7000;
const unsigned long DEBOUNCE_MS=35, DOUBLE_CLICK_MS=650;
const unsigned long REJECT_HOLD_MS=1500, READY_HOLD_MS=1200;
const unsigned long WASTE_EVAC_MS=3000, PRIMARY_FLUSH_MS=4000, DRAIN_1_MS=2500;
const unsigned long RINSE_1_MS=4000, DRAIN_2_MS=2500, FINAL_RINSE_MS=3000;
const unsigned long FINAL_DRAIN_MS=3000, VERIFY_MS=1500, COMPLETE_HOLD_MS=1500;

unsigned long lastServoStepMs=0, stateEnteredMs=0, lastVitalReadMs=0, lastDisplayMs=0, lastSerialMs=0;
bool lastToiletRaw=HIGH, lastEmergencyRaw=HIGH, lastResetRaw=HIGH, waitingForSecondClick=false;
unsigned long toiletDebounceMs=0, emergencyDebounceMs=0, resetDebounceMs=0, firstClickMs=0;

struct PatientVitals{int hr,spo2,resp,systolic,diastolic; float tempC; bool engineeringWarning;};
PatientVitals vitals;

enum class BedState{NORMAL,SAFETY_CHECK,REQUEST_REJECTED,OPENING,TOILET_READY,IN_USE,FINISH_RECEIVED,SECURING,SECURED,SANITATION_CHECK,WASTE_EVACUATION,PRIMARY_FLUSH,DRAIN_1,RINSE_1,DRAIN_2,FINAL_RINSE,FINAL_DRAIN,CYCLE_VERIFY,SANITATION_COMPLETE,EMERGENCY,FAULT};
BedState state=BedState::NORMAL;

const char* stateName(BedState s);
void enterState(BedState next);

bool patientPresent(){return digitalRead(PIN_PATIENT)==HIGH;}
bool obstructionDetected(){return digitalRead(PIN_OBSTRUCTION)==HIGH;}
bool openLimitActive(){return digitalRead(PIN_OPEN_LIMIT)==HIGH;}
bool closedLimitActive(){return digitalRead(PIN_CLOSED_LIMIT)==HIGH;}
bool waterAvailable(){return digitalRead(PIN_WATER_AVAILABLE)==HIGH;}
bool drainAvailable(){return digitalRead(PIN_DRAIN_AVAILABLE)==HIGH;}
bool leakDetected(){return digitalRead(PIN_LEAK_DETECTED)==HIGH;}
bool contradictoryLimits(){return openLimitActive()&&closedLimitActive();}

const char* stateName(BedState s){
  switch(s){
    case BedState::NORMAL:return "NORMAL"; case BedState::SAFETY_CHECK:return "SAFETY_CHECK";
    case BedState::REQUEST_REJECTED:return "REQUEST_REJECTED"; case BedState::OPENING:return "OPENING";
    case BedState::TOILET_READY:return "TOILET_READY"; case BedState::IN_USE:return "IN_USE";
    case BedState::FINISH_RECEIVED:return "FINISH_RECEIVED"; case BedState::SECURING:return "SECURING";
    case BedState::SECURED:return "SECURED"; case BedState::SANITATION_CHECK:return "SANITATION_CHECK";
    case BedState::WASTE_EVACUATION:return "WASTE_EVAC"; case BedState::PRIMARY_FLUSH:return "PRIMARY_FLUSH";
    case BedState::DRAIN_1:return "DRAIN_1"; case BedState::RINSE_1:return "RINSE_1";
    case BedState::DRAIN_2:return "DRAIN_2"; case BedState::FINAL_RINSE:return "FINAL_RINSE";
    case BedState::FINAL_DRAIN:return "FINAL_DRAIN"; case BedState::CYCLE_VERIFY:return "CYCLE_VERIFY";
    case BedState::SANITATION_COMPLETE:return "CLEAN_COMPLETE"; case BedState::EMERGENCY:return "EMERGENCY";
    case BedState::FAULT:return "FAULT";
  } return "UNKNOWN";
}

bool vitalWarningCondition(){return vitals.hr<50||vitals.hr>120||vitals.spo2<92||vitals.tempC<35.5||vitals.tempC>38.0||vitals.resp<10||vitals.resp>24||vitals.systolic<90||vitals.systolic>140||vitals.diastolic<60||vitals.diastolic>90;}
void readVitals(){
  vitals.hr=map(analogRead(HR_PIN),0,1023,40,180); vitals.spo2=map(analogRead(SPO2_PIN),0,1023,80,100);
  vitals.tempC=map(analogRead(TEMP_PIN),0,1023,340,405)/10.0; vitals.resp=map(analogRead(RESP_PIN),0,1023,6,35);
  vitals.systolic=map(analogRead(SYS_PIN),0,1023,80,180); vitals.diastolic=map(analogRead(DIA_PIN),0,1023,40,120);
  vitals.engineeringWarning=vitalWarningCondition();
}

bool isSanitationState(){return state>=BedState::SANITATION_CHECK&&state<=BedState::SANITATION_COMPLETE;}
void sanitationOutputsOff(){digitalWrite(PIN_FLUSH_OUTPUT,LOW);digitalWrite(PIN_RINSE_OUTPUT,LOW);digitalWrite(PIN_DRAIN_OUTPUT,LOW);digitalWrite(PIN_SANITATION_LED,LOW);wasteServo.write(WASTE_CLOSED_ANGLE);}
void updateOutputsForState(){
  sanitationOutputsOff();
  bool ready=(state==BedState::TOILET_READY||state==BedState::IN_USE), danger=(state==BedState::FAULT||state==BedState::EMERGENCY);
  digitalWrite(PIN_READY_LED,ready); digitalWrite(PIN_FAULT_LED,danger); if(danger) tone(PIN_BUZZER,1000); else noTone(PIN_BUZZER);
  if(isSanitationState()) digitalWrite(PIN_SANITATION_LED,HIGH);
  switch(state){
    case BedState::WASTE_EVACUATION:wasteServo.write(90);digitalWrite(PIN_DRAIN_OUTPUT,HIGH);break;
    case BedState::PRIMARY_FLUSH:wasteServo.write(90);digitalWrite(PIN_FLUSH_OUTPUT,HIGH);break;
    case BedState::DRAIN_1:case BedState::DRAIN_2:case BedState::FINAL_DRAIN:wasteServo.write(90);digitalWrite(PIN_DRAIN_OUTPUT,HIGH);break;
    case BedState::RINSE_1:case BedState::FINAL_RINSE:wasteServo.write(90);digitalWrite(PIN_RINSE_OUTPUT,HIGH);break;
    default:break;
  }
}
void enterState(BedState next){
  state=next;
  stateEnteredMs=millis();

  // Reset the actuator timing reference whenever a movement state begins.
  // This prevents stale elapsed time from causing an instant position jump.
  if(state==BedState::OPENING || state==BedState::SECURING){
    lastServoStepMs=stateEnteredMs;
  }

  updateOutputsForState();
  Serial.print("[STATE] ");
  Serial.println(stateName(state));
}

bool pressEvent(int pin,bool &lastRaw,unsigned long &lastDebounceMs){bool raw=digitalRead(pin);if(raw!=lastRaw&&millis()-lastDebounceMs>=DEBOUNCE_MS){lastDebounceMs=millis();lastRaw=raw;if(raw==LOW)return true;}return false;}
void processEmergency(){if(pressEvent(PIN_EMERGENCY,lastEmergencyRaw,emergencyDebounceMs)){Serial.println("[EMERGENCY] Emergency button pressed.");enterState(BedState::EMERGENCY);}}

void processCaregiverReset(){
  if(!pressEvent(PIN_CAREGIVER_RESET,lastResetRaw,resetDebounceMs)) return;

  if(state!=BedState::FAULT && state!=BedState::EMERGENCY){
    Serial.println("[RESET] Ignored: system is not in FAULT or EMERGENCY.");
    return;
  }

  bool safeToReset =
    !obstructionDetected() &&
    !leakDetected() &&
    closedLimitActive() &&
    !openLimitActive();

  if(!safeToReset){
    Serial.println("[RESET] Denied: restore safe closed condition first.");
    return;
  }

  sanitationOutputsOff();
  toiletCommandedAngle=TOILET_CLOSED_ANGLE;
  toiletServo.write(TOILET_CLOSED_ANGLE);
  wasteServo.write(WASTE_CLOSED_ANGLE);
  waitingForSecondClick=false;

  Serial.println("[RESET] Safe condition confirmed. Returning to NORMAL.");
  enterState(BedState::NORMAL);
}
void processToiletButton(){
  bool pressed=pressEvent(PIN_TOILET_BUTTON,lastToiletRaw,toiletDebounceMs);
  if(!pressed){if(waitingForSecondClick&&millis()-firstClickMs>DOUBLE_CLICK_MS){waitingForSecondClick=false;Serial.println("[INPUT] Finish single-click expired.");}return;}
  if(state==BedState::NORMAL){Serial.println("[INPUT] Toilet request received.");enterState(BedState::SAFETY_CHECK);return;}
  if(state==BedState::IN_USE){if(!waitingForSecondClick){waitingForSecondClick=true;firstClickMs=millis();Serial.println("[INPUT] First finish click received.");}else if(millis()-firstClickMs<=DOUBLE_CLICK_MS){waitingForSecondClick=false;Serial.println("[INPUT] Valid double-click: patient finished.");enterState(BedState::FINISH_RECEIVED);}}
}

void stepToiletServo(int targetAngle){
  unsigned long now=millis();

  if(now-lastServoStepMs<SERVO_STEP_MS) return;

  // Work out how many 35 ms movement intervals elapsed.
  // This makes actuator timing independent of slow TFT redraws.
  unsigned long elapsedSteps=(now-lastServoStepMs)/SERVO_STEP_MS;
  lastServoStepMs += elapsedSteps*SERVO_STEP_MS;

  int steps=(int)elapsedSteps;

  if(toiletCommandedAngle<targetAngle){
    toiletCommandedAngle += steps;
    if(toiletCommandedAngle>targetAngle) toiletCommandedAngle=targetAngle;
  }
  else if(toiletCommandedAngle>targetAngle){
    toiletCommandedAngle -= steps;
    if(toiletCommandedAngle<targetAngle) toiletCommandedAngle=targetAngle;
  }

  toiletServo.write(toiletCommandedAngle);
}

bool sanitationCommonFault(){if(leakDetected()){Serial.println("[SANITATION FAULT] Leak detected.");return true;}return false;}
bool requireWater(){if(!waterAvailable()){Serial.println("[SANITATION FAULT] Water unavailable.");return false;}return true;}
bool requireDrain(){if(!drainAvailable()){Serial.println("[SANITATION FAULT] Drain unavailable.");return false;}return true;}

void runStateMachine(){
  switch(state){
    case BedState::NORMAL:break;
    case BedState::SAFETY_CHECK:
      if(!patientPresent()){Serial.println("[SAFETY] No patient detected.");enterState(BedState::REQUEST_REJECTED);}else if(obstructionDetected()){Serial.println("[SAFETY] Obstruction present.");enterState(BedState::FAULT);}else if(contradictoryLimits()){Serial.println("[FAULT] Contradictory position feedback.");enterState(BedState::FAULT);}else if(!closedLimitActive()){Serial.println("[FAULT] Closed position not confirmed.");enterState(BedState::FAULT);}else{Serial.println("[SAFETY] Checks passed. Opening authorised.");enterState(BedState::OPENING);}break;
    case BedState::REQUEST_REJECTED:if(millis()-stateEnteredMs>=REJECT_HOLD_MS)enterState(BedState::NORMAL);break;
    case BedState::OPENING:
      if(obstructionDetected()){Serial.println("[FAULT] Obstruction during opening.");enterState(BedState::FAULT);break;}
      if(millis()-stateEnteredMs>MOVEMENT_TIMEOUT_MS){Serial.println("[FAULT] Opening timeout.");enterState(BedState::FAULT);break;}
      stepToiletServo(TOILET_OPEN_ANGLE); if(toiletCommandedAngle>=90&&openLimitActive()){Serial.println("[POSITION] Open confirmed.");enterState(BedState::TOILET_READY);}break;
    case BedState::TOILET_READY:if(millis()-stateEnteredMs>=READY_HOLD_MS)enterState(BedState::IN_USE);break;
    case BedState::IN_USE:break;
    case BedState::FINISH_RECEIVED:enterState(BedState::SECURING);break;
    case BedState::SECURING:
      if(obstructionDetected()){Serial.println("[FAULT] Obstruction during securing.");enterState(BedState::FAULT);break;}
      if(millis()-stateEnteredMs>MOVEMENT_TIMEOUT_MS){Serial.println("[FAULT] Closing timeout.");enterState(BedState::FAULT);break;}
      stepToiletServo(TOILET_CLOSED_ANGLE); if(toiletCommandedAngle<=0&&closedLimitActive()){Serial.println("[POSITION] Closed confirmed.");enterState(BedState::SECURED);}break;
    case BedState::SECURED:if(millis()-stateEnteredMs>=500)enterState(BedState::SANITATION_CHECK);break;
    case BedState::SANITATION_CHECK:
      if(!closedLimitActive()||openLimitActive()){Serial.println("[SANITATION FAULT] Access not secured.");enterState(BedState::FAULT);}else if(!waterAvailable()){Serial.println("[SANITATION FAULT] Water unavailable.");enterState(BedState::FAULT);}else if(!drainAvailable()){Serial.println("[SANITATION FAULT] Drain unavailable.");enterState(BedState::FAULT);}else if(leakDetected()){Serial.println("[SANITATION FAULT] Leak detected.");enterState(BedState::FAULT);}else{Serial.println("[SANITATION] Safety checks passed.");enterState(BedState::WASTE_EVACUATION);}break;
    case BedState::WASTE_EVACUATION:if(sanitationCommonFault()||!requireDrain())enterState(BedState::FAULT);else if(millis()-stateEnteredMs>=WASTE_EVAC_MS)enterState(BedState::PRIMARY_FLUSH);break;
    case BedState::PRIMARY_FLUSH:if(sanitationCommonFault()||!requireWater())enterState(BedState::FAULT);else if(millis()-stateEnteredMs>=PRIMARY_FLUSH_MS)enterState(BedState::DRAIN_1);break;
    case BedState::DRAIN_1:if(sanitationCommonFault()||!requireDrain())enterState(BedState::FAULT);else if(millis()-stateEnteredMs>=DRAIN_1_MS)enterState(BedState::RINSE_1);break;
    case BedState::RINSE_1:if(sanitationCommonFault()||!requireWater())enterState(BedState::FAULT);else if(millis()-stateEnteredMs>=RINSE_1_MS)enterState(BedState::DRAIN_2);break;
    case BedState::DRAIN_2:if(sanitationCommonFault()||!requireDrain())enterState(BedState::FAULT);else if(millis()-stateEnteredMs>=DRAIN_2_MS)enterState(BedState::FINAL_RINSE);break;
    case BedState::FINAL_RINSE:if(sanitationCommonFault()||!requireWater())enterState(BedState::FAULT);else if(millis()-stateEnteredMs>=FINAL_RINSE_MS)enterState(BedState::FINAL_DRAIN);break;
    case BedState::FINAL_DRAIN:if(sanitationCommonFault()||!requireDrain())enterState(BedState::FAULT);else if(millis()-stateEnteredMs>=FINAL_DRAIN_MS){wasteServo.write(0);enterState(BedState::CYCLE_VERIFY);}break;
    case BedState::CYCLE_VERIFY:
      if(leakDetected()||!closedLimitActive()||openLimitActive()){Serial.println("[VERIFY] Cycle verification failed.");enterState(BedState::FAULT);}else if(millis()-stateEnteredMs>=VERIFY_MS){Serial.println("[VERIFY] Programmed sanitation cycle completed safely.");enterState(BedState::SANITATION_COMPLETE);}break;
    case BedState::SANITATION_COMPLETE:if(millis()-stateEnteredMs>=COMPLETE_HOLD_MS){Serial.println("[SYSTEM] Sanitation complete. Returning to NORMAL.");enterState(BedState::NORMAL);}break;
    case BedState::EMERGENCY:case BedState::FAULT:sanitationOutputsOff();break;
  }
}

void drawDisplay(){
  tft.fillScreen(ILI9341_BLACK); tft.setRotation(1); tft.fillRect(0,0,320,28,ILI9341_NAVY); tft.setTextColor(ILI9341_WHITE); tft.setTextSize(2); tft.setCursor(8,7); tft.print("ASSISTED-CARE BED");
  tft.setTextSize(1); tft.setTextColor(ILI9341_LIGHTGREY); tft.setCursor(8,40);tft.print("HR");tft.setTextSize(2);tft.setTextColor(ILI9341_GREEN);tft.setCursor(8,53);tft.print(vitals.hr);tft.setTextSize(1);tft.print(" BPM");
  tft.setTextColor(ILI9341_LIGHTGREY);tft.setCursor(90,40);tft.print("SpO2");tft.setTextSize(2);tft.setTextColor(ILI9341_CYAN);tft.setCursor(90,53);tft.print(vitals.spo2);tft.setTextSize(1);tft.print("%");
  tft.setTextColor(ILI9341_LIGHTGREY);tft.setCursor(170,40);tft.print("BP");tft.setTextSize(2);tft.setTextColor(ILI9341_YELLOW);tft.setCursor(170,53);tft.print(vitals.systolic);tft.print("/");tft.print(vitals.diastolic);
  tft.setTextSize(1);tft.setTextColor(ILI9341_LIGHTGREY);tft.setCursor(8,85);tft.print("TEMP");tft.setTextSize(2);tft.setTextColor(ILI9341_WHITE);tft.setCursor(8,98);tft.print(vitals.tempC,1);tft.print(" C");
  tft.setTextSize(1);tft.setTextColor(ILI9341_LIGHTGREY);tft.setCursor(120,85);tft.print("RESP");tft.setTextSize(2);tft.setTextColor(ILI9341_WHITE);tft.setCursor(120,98);tft.print(vitals.resp);tft.setTextSize(1);tft.print("/min");
  tft.drawFastHLine(0,135,320,ILI9341_DARKGREY); tft.setTextSize(1);tft.setTextColor(ILI9341_LIGHTGREY);tft.setCursor(8,145);tft.print("STATE:");
  uint16_t c=ILI9341_CYAN; if(state==BedState::FAULT||state==BedState::EMERGENCY)c=ILI9341_RED; else if(isSanitationState())c=ILI9341_YELLOW; else if(state==BedState::TOILET_READY||state==BedState::IN_USE)c=ILI9341_GREEN;
  tft.setTextColor(c);tft.setTextSize(2);tft.setCursor(8,160);tft.print(stateName(state));
  tft.setTextSize(1);tft.setTextColor(ILI9341_LIGHTGREY);tft.setCursor(8,190);tft.print("Water:");tft.setTextColor(waterAvailable()?ILI9341_GREEN:ILI9341_RED);tft.print(waterAvailable()?"OK":"NO");
  tft.setTextColor(ILI9341_LIGHTGREY);tft.setCursor(90,190);tft.print("Drain:");tft.setTextColor(drainAvailable()?ILI9341_GREEN:ILI9341_RED);tft.print(drainAvailable()?"OK":"NO");
  tft.setTextColor(ILI9341_LIGHTGREY);tft.setCursor(180,190);tft.print("Leak:");tft.setTextColor(leakDetected()?ILI9341_RED:ILI9341_GREEN);tft.print(leakDetected()?"YES":"NO");
  tft.setCursor(8,215);tft.setTextColor(vitals.engineeringWarning?ILI9341_RED:ILI9341_GREEN);tft.print(vitals.engineeringWarning?"PATIENT MONITOR: ENGINEERING WARNING":"PATIENT MONITOR: VALUES STABLE");
}

void printStatus(){Serial.print("State=");Serial.print(stateName(state));Serial.print(" | HR=");Serial.print(vitals.hr);Serial.print(" | SpO2=");Serial.print(vitals.spo2);Serial.print(" | BP=");Serial.print(vitals.systolic);Serial.print("/");Serial.print(vitals.diastolic);Serial.print(" | Water=");Serial.print(waterAvailable()?"YES":"NO");Serial.print(" | Drain=");Serial.print(drainAvailable()?"YES":"NO");Serial.print(" | Leak=");Serial.print(leakDetected()?"YES":"NO");Serial.print(" | ToiletServo=");Serial.print(toiletCommandedAngle);Serial.println(" deg");}

void setup(){
  Serial.begin(115200);
  pinMode(PIN_TOILET_BUTTON,INPUT_PULLUP);pinMode(PIN_EMERGENCY,INPUT_PULLUP);pinMode(PIN_CAREGIVER_RESET,INPUT_PULLUP);
  pinMode(PIN_PATIENT,INPUT);pinMode(PIN_OBSTRUCTION,INPUT);pinMode(PIN_OPEN_LIMIT,INPUT);pinMode(PIN_CLOSED_LIMIT,INPUT);
  pinMode(PIN_WATER_AVAILABLE,INPUT);pinMode(PIN_DRAIN_AVAILABLE,INPUT);pinMode(PIN_LEAK_DETECTED,INPUT);
  pinMode(PIN_READY_LED,OUTPUT);pinMode(PIN_FAULT_LED,OUTPUT);pinMode(PIN_FLUSH_OUTPUT,OUTPUT);pinMode(PIN_RINSE_OUTPUT,OUTPUT);pinMode(PIN_DRAIN_OUTPUT,OUTPUT);pinMode(PIN_SANITATION_LED,OUTPUT);pinMode(PIN_BUZZER,OUTPUT);
  toiletServo.attach(PIN_TOILET_SERVO);wasteServo.attach(PIN_WASTE_SERVO);toiletServo.write(0);wasteServo.write(0);sanitationOutputsOff();
  tft.begin();tft.setRotation(1);readVitals();drawDisplay();Serial.println("SMART ASSISTED-CARE BED - FINAL INTEGRATED CONTROLLER");
}

void loop(){
  unsigned long now=millis();
  if(now-lastVitalReadMs>=100){lastVitalReadMs=now;readVitals();}
  processEmergency();
  processCaregiverReset();
  if(state!=BedState::EMERGENCY&&state!=BedState::FAULT){processToiletButton();runStateMachine();}
  if(now-lastDisplayMs>=500){
    // drawDisplay() is relatively expensive in simulation because it
    // redraws the full TFT. Timestamp AFTER drawing so display work does
    // not immediately trigger another redraw and starve actuator control.
    drawDisplay();
    lastDisplayMs=millis();
  }
  if(now-lastSerialMs>=1000){
    printStatus();
    lastSerialMs=millis();
  }
}
