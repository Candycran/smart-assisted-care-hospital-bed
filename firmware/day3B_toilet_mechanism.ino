#include <Servo.h>

// Smart Assisted-Care Hospital Bed
// Day 3B - Assisted Toileting Mechanism
// Engineering simulation only.

// ---------------- Pin assignments ----------------
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

// ---------------- Actuator ----------------
Servo toiletServo;

const int CLOSED_ANGLE = 0;
const int OPEN_ANGLE   = 90;
int commandedAngle = CLOSED_ANGLE;

const unsigned long SERVO_STEP_MS       = 35;
const unsigned long MOVEMENT_TIMEOUT_MS = 7000;
unsigned long lastServoStepMs = 0;

// ---------------- Buttons ----------------
const unsigned long DEBOUNCE_MS = 35;
const unsigned long DOUBLE_CLICK_WINDOW_MS = 650;

bool lastToiletRaw = HIGH;
bool lastEmergencyRaw = HIGH;

unsigned long toiletDebounceMs = 0;
unsigned long emergencyDebounceMs = 0;

bool waitingForSecondClick = false;
unsigned long firstClickMs = 0;

// ---------------- State machine ----------------
enum class BedState {
  NORMAL,
  SAFETY_CHECK,
  REQUEST_REJECTED,
  OPENING,
  TOILET_READY,
  IN_USE,
  FINISH_RECEIVED,
  SECURING,
  SECURED,
  EMERGENCY,
  FAULT
};

BedState state = BedState::NORMAL;

// Manual prototypes avoid Arduino .ino preprocessing issues with enum classes.
const char* stateName(BedState s);
void enterState(BedState next);

unsigned long stateEnteredMs = 0;
const unsigned long REJECT_HOLD_MS = 1500;
const unsigned long READY_HOLD_MS  = 1200;

// ---------------- Input helpers ----------------
bool patientPresent() {
  return digitalRead(PIN_PATIENT) == HIGH;
}

bool obstructionDetected() {
  return digitalRead(PIN_OBSTRUCTION) == HIGH;
}

bool openLimitActive() {
  return digitalRead(PIN_OPEN_LIMIT) == HIGH;
}

bool closedLimitActive() {
  return digitalRead(PIN_CLOSED_LIMIT) == HIGH;
}

bool contradictoryLimits() {
  return openLimitActive() && closedLimitActive();
}

const char* stateName(BedState s) {
  switch (s) {
    case BedState::NORMAL:           return "NORMAL";
    case BedState::SAFETY_CHECK:     return "SAFETY_CHECK";
    case BedState::REQUEST_REJECTED: return "REQUEST_REJECTED";
    case BedState::OPENING:          return "OPENING";
    case BedState::TOILET_READY:     return "TOILET_READY";
    case BedState::IN_USE:           return "IN_USE";
    case BedState::FINISH_RECEIVED:  return "FINISH_RECEIVED";
    case BedState::SECURING:         return "SECURING";
    case BedState::SECURED:          return "SECURED";
    case BedState::EMERGENCY:        return "EMERGENCY";
    case BedState::FAULT:            return "FAULT";
  }
  return "UNKNOWN";
}

// ---------------- Indicators ----------------
void updateIndicators() {
  bool ready = (state == BedState::TOILET_READY ||
                state == BedState::IN_USE);

  bool danger = (state == BedState::FAULT ||
                 state == BedState::EMERGENCY);

  digitalWrite(PIN_READY_LED, ready ? HIGH : LOW);
  digitalWrite(PIN_FAULT_LED, danger ? HIGH : LOW);

  if (danger) {
    tone(PIN_BUZZER, 1000);
  } else {
    noTone(PIN_BUZZER);
  }
}

void enterState(BedState next) {
  state = next;
  stateEnteredMs = millis();
  updateIndicators();

  Serial.print("[STATE] ");
  Serial.println(stateName(state));
}

// ---------------- Debounced press event ----------------
bool pressEvent(int pin, bool &lastRaw, unsigned long &lastDebounceMs) {
  bool raw = digitalRead(pin);

  if (raw != lastRaw && (millis() - lastDebounceMs) >= DEBOUNCE_MS) {
    lastDebounceMs = millis();
    lastRaw = raw;

    if (raw == LOW) {
      return true;
    }
  }

  return false;
}

// ---------------- Emergency ----------------
void processEmergency() {
  if (pressEvent(PIN_EMERGENCY, lastEmergencyRaw, emergencyDebounceMs)) {
    Serial.println("[EMERGENCY] Emergency button pressed.");
    enterState(BedState::EMERGENCY);
  }
}

// ---------------- Toilet button ----------------
void processToiletButton() {
  bool pressed = pressEvent(
    PIN_TOILET_BUTTON,
    lastToiletRaw,
    toiletDebounceMs
  );

  if (!pressed) {
    if (waitingForSecondClick &&
        (millis() - firstClickMs > DOUBLE_CLICK_WINDOW_MS)) {
      waitingForSecondClick = false;
      Serial.println("[INPUT] Finish single-click expired; no action.");
    }
    return;
  }

  if (state == BedState::NORMAL) {
    Serial.println("[INPUT] Toilet request received.");
    enterState(BedState::SAFETY_CHECK);
    return;
  }

  if (state == BedState::IN_USE) {
    if (!waitingForSecondClick) {
      waitingForSecondClick = true;
      firstClickMs = millis();
      Serial.println("[INPUT] First finish click received.");
      return;
    }

    if (millis() - firstClickMs <= DOUBLE_CLICK_WINDOW_MS) {
      waitingForSecondClick = false;
      Serial.println("[INPUT] Valid double-click: patient finished.");
      enterState(BedState::FINISH_RECEIVED);
    }
  }
}

// ---------------- Non-blocking servo movement ----------------
void stepServoToward(int targetAngle) {
  if (millis() - lastServoStepMs < SERVO_STEP_MS) {
    return;
  }

  lastServoStepMs = millis();

  if (commandedAngle < targetAngle) {
    commandedAngle++;
    toiletServo.write(commandedAngle);
  } else if (commandedAngle > targetAngle) {
    commandedAngle--;
    toiletServo.write(commandedAngle);
  }
}

// ---------------- State machine ----------------
void runStateMachine() {
  switch (state) {

    case BedState::NORMAL:
      break;

    case BedState::SAFETY_CHECK:
      if (!patientPresent()) {
        Serial.println("[SAFETY] Request rejected: no patient detected.");
        enterState(BedState::REQUEST_REJECTED);
      }
      else if (obstructionDetected()) {
        Serial.println("[SAFETY] Obstruction detected before opening.");
        enterState(BedState::FAULT);
      }
      else if (contradictoryLimits()) {
        Serial.println("[FAULT] Both position limits are active.");
        enterState(BedState::FAULT);
      }
      else if (!closedLimitActive()) {
        Serial.println("[FAULT] Closed position is not confirmed.");
        enterState(BedState::FAULT);
      }
      else {
        Serial.println("[SAFETY] Checks passed. Opening authorised.");
        enterState(BedState::OPENING);
      }
      break;

    case BedState::REQUEST_REJECTED:
      if (millis() - stateEnteredMs >= REJECT_HOLD_MS) {
        enterState(BedState::NORMAL);
      }
      break;

    case BedState::OPENING:
      if (obstructionDetected()) {
        Serial.println("[FAULT] Obstruction during opening.");
        enterState(BedState::FAULT);
        break;
      }

      if (contradictoryLimits()) {
        Serial.println("[FAULT] Contradictory position feedback.");
        enterState(BedState::FAULT);
        break;
      }

      if (millis() - stateEnteredMs > MOVEMENT_TIMEOUT_MS) {
        Serial.println("[FAULT] Opening timeout: OPEN LIMIT not confirmed.");
        enterState(BedState::FAULT);
        break;
      }

      stepServoToward(OPEN_ANGLE);

      if (commandedAngle >= OPEN_ANGLE && openLimitActive()) {
        Serial.println("[POSITION] Open position confirmed.");
        enterState(BedState::TOILET_READY);
      }
      break;

    case BedState::TOILET_READY:
      if (millis() - stateEnteredMs >= READY_HOLD_MS) {
        Serial.println("[PATIENT] Toilet ready. Double-press TOILET when finished.");
        enterState(BedState::IN_USE);
      }
      break;

    case BedState::IN_USE:
      break;

    case BedState::FINISH_RECEIVED:
      Serial.println("[CONTROL] Finish accepted. Securing toilet access.");
      enterState(BedState::SECURING);
      break;

    case BedState::SECURING:
      if (obstructionDetected()) {
        Serial.println("[FAULT] Obstruction during securing.");
        enterState(BedState::FAULT);
        break;
      }

      if (contradictoryLimits()) {
        Serial.println("[FAULT] Contradictory position feedback.");
        enterState(BedState::FAULT);
        break;
      }

      if (millis() - stateEnteredMs > MOVEMENT_TIMEOUT_MS) {
        Serial.println("[FAULT] Closing timeout: CLOSED LIMIT not confirmed.");
        enterState(BedState::FAULT);
        break;
      }

      stepServoToward(CLOSED_ANGLE);

      if (commandedAngle <= CLOSED_ANGLE && closedLimitActive()) {
        Serial.println("[POSITION] Closed position confirmed.");
        enterState(BedState::SECURED);
      }
      break;

    case BedState::SECURED:
      // Intentional Day 3 endpoint.
      // Day 4 begins here with waste evacuation and sanitation.
      break;

    case BedState::EMERGENCY:
      // Latched. Restart the simulation after correcting the condition.
      break;

    case BedState::FAULT:
      // Latched. Restart the simulation after diagnosing the fault.
      break;
  }
}

// ---------------- Diagnostic status ----------------
void printStatus() {
  static unsigned long lastPrintMs = 0;

  if (millis() - lastPrintMs < 1000) {
    return;
  }

  lastPrintMs = millis();

  Serial.print("State=");
  Serial.print(stateName(state));

  Serial.print(" | Patient=");
  Serial.print(patientPresent() ? "YES" : "NO");

  Serial.print(" | Obstruction=");
  Serial.print(obstructionDetected() ? "YES" : "NO");

  Serial.print(" | OpenLimit=");
  Serial.print(openLimitActive() ? "ON" : "OFF");

  Serial.print(" | ClosedLimit=");
  Serial.print(closedLimitActive() ? "ON" : "OFF");

  Serial.print(" | ServoCmd=");
  Serial.print(commandedAngle);

  Serial.println(" deg");
}

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
  toiletServo.write(CLOSED_ANGLE);

  updateIndicators();

  Serial.println();
  Serial.println("SMART ASSISTED-CARE BED - DAY 3");
  Serial.println("Assisted toileting mechanism and safety controller");
  Serial.println();
  Serial.println("Required initial switch states:");
  Serial.println("Patient=YES | Obstruction=NO | OpenLimit=OFF | ClosedLimit=ON");
  Serial.println();
}

void loop() {
  processEmergency();

  if (state != BedState::EMERGENCY &&
      state != BedState::FAULT) {
    processToiletButton();
    runStateMachine();
  }

  printStatus();
}
