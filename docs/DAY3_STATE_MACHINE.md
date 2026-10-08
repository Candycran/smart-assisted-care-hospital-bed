# Day 3 State Machine

```text
NORMAL
  |
  | single press
  v
SAFETY_CHECK
  |-- patient absent ---------> REQUEST_REJECTED -> NORMAL
  |-- obstruction -----------> FAULT
  |-- closed not confirmed --> FAULT
  |-- both limits active ----> FAULT
  |
  v
OPENING
  |-- obstruction -----------> FAULT
  |-- timeout ---------------> FAULT
  |-- emergency -------------> EMERGENCY
  |
  | open limit confirmed
  v
TOILET_READY
  |
  v
IN_USE
  |
  | deliberate double press
  v
FINISH_RECEIVED
  |
  v
SECURING
  |-- obstruction -----------> FAULT
  |-- timeout ---------------> FAULT
  |-- emergency -------------> EMERGENCY
  |
  | closed limit confirmed
  v
SECURED
```

Day 4 continues from SECURED.
