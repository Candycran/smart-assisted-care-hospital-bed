%% Smart Assisted-Care Hospital Bed
% Day 2 — Synthetic Patient Vitals Analysis
% Engineering simulation only. Not clinical data.

clear; clc; close all;

t = (0:1:60)';

hr = 78 + 3*sin(2*pi*t/20);
spo2 = 97 + 0.5*sin(2*pi*t/30);
sys = 118 + 4*sin(2*pi*t/25);
dia = 76 + 3*sin(2*pi*t/25);
temp = 36.8 + 0.15*sin(2*pi*t/40);
resp = 16 + 1.5*sin(2*pi*t/18);

% Inject demonstration abnormal segments
hr(35:42) = hr(35:42) + 55;
spo2(45:50) = spo2(45:50) - 8;
temp(52:56) = temp(52:56) + 1.8;

% Engineering test thresholds only — NOT clinical limits
warnHR = hr < 50 | hr > 120;
warnSpO2 = spo2 < 92;
warnTemp = temp < 35.5 | temp > 38.0;
warnResp = resp < 10 | resp > 24;
warnBP = sys < 90 | sys > 140 | dia < 60 | dia > 90;
overallWarning = warnHR | warnSpO2 | warnTemp | warnResp | warnBP;

figure('Name','Day 2 - Simulated Patient Vitals');
subplot(3,2,1); plot(t, hr, 'LineWidth', 1.2); xlabel('Time (s)'); ylabel('BPM'); title('Heart Rate'); grid on;
subplot(3,2,2); plot(t, spo2, 'LineWidth', 1.2); xlabel('Time (s)'); ylabel('%'); title('SpO2'); grid on;
subplot(3,2,3); plot(t, sys, 'LineWidth', 1.2); hold on; plot(t, dia, 'LineWidth', 1.2); xlabel('Time (s)'); ylabel('mmHg'); title('Blood Pressure'); legend('Systolic','Diastolic'); grid on;
subplot(3,2,4); plot(t, temp, 'LineWidth', 1.2); xlabel('Time (s)'); ylabel('deg C'); title('Temperature'); grid on;
subplot(3,2,5); plot(t, resp, 'LineWidth', 1.2); xlabel('Time (s)'); ylabel('breaths/min'); title('Respiratory Rate'); grid on;
subplot(3,2,6); stairs(t, overallWarning, 'LineWidth', 1.2); xlabel('Time (s)'); ylabel('Warning'); title('Engineering Warning State'); ylim([-0.1 1.1]); grid on;

T = table(t, hr, spo2, sys, dia, temp, resp, overallWarning, ...
    'VariableNames', {'Time_s','HR_BPM','SpO2_pct','Sys_mmHg','Dia_mmHg','Temp_C','Resp_per_min','Warning'});
writetable(T, 'day2_simulated_patient_vitals.csv');

disp('Saved day2_simulated_patient_vitals.csv');
