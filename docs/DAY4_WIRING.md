# Day 4 Wokwi Wiring

Keep all Day 2 and Day 3 components. Add these manually.

## Inputs
- WATER AVAILABLE switch: centre/common -> D28, outer pins -> 5V and GND. HIGH = available.
- DRAIN AVAILABLE switch: centre/common -> D29, outer pins -> 5V and GND. HIGH = available.
- LEAK DETECTED switch: centre/common -> D36, outer pins -> 5V and GND. HIGH = leak detected.

## Waste outlet valve servo
- PWM -> D7
- V+ -> 5V
- GND -> GND
- 0 deg = closed, 90 deg = open

## Simulated pump/valve outputs
Use LEDs with 220 ohm resistors:
- FLUSH: D32 -> resistor -> LED anode; cathode -> GND
- RINSE: D33 -> resistor -> LED anode; cathode -> GND
- DRAIN: D34 -> resistor -> LED anode; cathode -> GND
- SANITATION ACTIVE: D35 -> resistor -> LED anode; cathode -> GND

## Starting conditions
Patient=HIGH, Obstruction=LOW, OpenLimit=LOW, ClosedLimit=HIGH, Water=HIGH, Drain=HIGH, Leak=LOW.
