# Smart Assisted-Care Hospital Bed with Automated Toileting and Patient Monitoring

## Overview
This project is a virtual mechatronics engineering prototype designed to explore how an intelligent hospital bed could improve independence and hygiene support for severely mobility-impaired patients while retaining caregiver oversight.

## Problem
Patients with paralysis, major mobility limitations or temporary post-operative immobility may require repeated caregiver assistance for toileting and monitoring. The proposed system integrates continuous simulated physiological monitoring with patient-initiated defecation assistance and automated toilet-bowl sanitation.

## Implemented subsystems
- simulated heart rate, SpO2, blood pressure, temperature and respiratory monitoring;
- clinician-style TFT interface;
- bed occupancy input;
- accessible toilet request/finish control;
- toilet-access actuator simulation;
- open/closed position feedback;
- obstruction detection;
- emergency control;
- waste outlet actuator;
- flush/rinse/drain control;
- water/drain/leak interlocks;
- automated sanitation sequence;
- safe caregiver fault reset;
- MATLAB analysis;
- Simulink actuator modelling;
- formal verification matrix and preliminary FMEA.

## Toolchain
- Wokwi
- Arduino Mega 2560 / Arduino C++
- VS Code
- Git / GitHub
- MATLAB
- Simulink

## Engineering scope
The work is a virtual proof-of-concept. Physiological inputs are synthetic and the sanitation sequence is control-system simulation only. The project is not a clinically validated medical device.

## Future development
- physical mechanical/CAD design and load analysis;
- real clinical-grade physiological sensors;
- physical actuator and motor-driver selection;
- real water/waste plumbing;
- patient perineal cleansing and drying;
- urine-management subsystem;
- full-body assisted bathing;
- pressure-injury prevention/repositioning;
- nurse-station/network integration;
- battery backup and manual mechanical release;
- regulatory, electrical-safety, usability and clinical validation.
