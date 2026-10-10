# Final Evidence Checklist

Capture clean, readable evidence — not every debug screen.

## Wokwi screenshots
- `final_system_overview.png` — full circuit with subsystem layout visible
- `patient_monitor_stable.png` — nominal vitals / values stable
- `toilet_ready.png` — TOILET_READY or IN_USE
- `sanitation_active.png` — one rinse/flush/drain state
- `normal_cycle_complete.png` — returned to NORMAL after sanitation
- `fault_obstruction.png` — safety fault example
- `fault_leak.png` — sanitation leak fault
- `safe_caregiver_reset.png` — recovery back to NORMAL

## MATLAB
- Day 2 patient-vitals analysis plot
- Day 4 sanitation timeline
- Day 4 output-activity plot

## Simulink
- Day 3 actuator model block diagram
- Day 3 actuator position response

## GIFs
Create three short GIFs:
1. `normal_assisted_toileting_cycle.gif`
   - request -> opening -> ready -> double press -> secure
2. `automatic_sanitation_cycle.gif`
   - waste evacuation -> flush -> rinse -> drain -> verification
3. `safety_fault_response.gif`
   - inject an obstruction or leak -> system enters FAULT

Keep GIFs short, crop to the Wokwi simulation area, and make labels/Serial output readable.
