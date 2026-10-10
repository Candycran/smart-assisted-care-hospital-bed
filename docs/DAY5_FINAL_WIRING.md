# Day 5 Final Wiring Addition

Keep every Day 1–4 connection unchanged.

## Caregiver RESET / ACKNOWLEDGE button
Add one pushbutton:

- one electrical side -> Arduino Mega D37
- opposite electrical side -> GND
- firmware uses `INPUT_PULLUP`

Therefore:
- released = HIGH
- pressed = LOW

## Reset conditions
The reset button only returns the controller from FAULT/EMERGENCY to NORMAL if:

- obstruction = NO;
- leak = NO;
- CLOSED LIMIT = ON;
- OPEN LIMIT = OFF.

If these conditions are not satisfied, reset is rejected.

This prevents a caregiver reset from clearing a fault while the patient-access mechanism is still in an unsafe configuration.
