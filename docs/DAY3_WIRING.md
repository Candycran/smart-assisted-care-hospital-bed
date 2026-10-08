# Day 3 Wiring

## Existing connections that stay
Keep the complete Day 2 patient-monitoring circuit and TFT.

## Toilet and safety inputs

### Toilet button
- one electrical side -> D22
- opposite electrical side -> GND
- firmware mode: INPUT_PULLUP

### Emergency button
- one electrical side -> D23
- opposite electrical side -> GND
- firmware mode: INPUT_PULLUP

For a four-pin tactile pushbutton, the two pins on each physical side are internally common.
Use opposite sides of the button.

## Four slide switches

Each Wokwi slide switch has three terminals.

For each switch:
- CENTER / COMMON terminal -> assigned Arduino pin
- one OUTER terminal -> 5V
- the other OUTER terminal -> GND

Assignments:
- Patient present -> D24
- Obstruction -> D25
- Open limit -> D26
- Closed limit -> D27

The code treats HIGH as ACTIVE.
If the physical left/right orientation in your Wokwi layout makes the labels feel reversed,
leave the wiring electrically correct and use the Serial Monitor to identify which switch position is HIGH.

### Required initial conditions
- Patient present = ACTIVE/HIGH
- Obstruction = INACTIVE/LOW
- Open limit = INACTIVE/LOW
- Closed limit = ACTIVE/HIGH

## Servo
- PWM/signal -> D6
- V+ -> 5V
- GND -> GND

The Wokwi servo is only a simulation of the real toilet-access actuator.
A real bed actuator would need an appropriately rated driver and power supply.

## LEDs

### Green READY LED
- D30 -> 220 ohm resistor -> LED anode (+)
- LED cathode (-) -> GND

### Red FAULT LED
- D31 -> 220 ohm resistor -> LED anode (+)
- LED cathode (-) -> GND

The resistor may be placed on either side of the LED electrically, but keep the layout consistent.

## Buzzer
- buzzer + -> D8
- buzzer - -> GND

This direct connection is acceptable for the Wokwi simulation.
A real buzzer/indicator may require a transistor driver depending on current requirements.
