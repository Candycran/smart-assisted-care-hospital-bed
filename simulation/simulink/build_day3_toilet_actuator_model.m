%% Smart Assisted-Care Hospital Bed
% Day 3 - Toilet access actuator model
% Run in MATLAB with Simulink installed.

clear; clc;

model = 'day3_toilet_actuator_model';

if bdIsLoaded(model)
    close_system(model, 0);
end

new_system(model);
open_system(model);

add_block('simulink/Sources/Step', [model '/Open Command'], ...
    'Time', '1', 'Before', '0', 'After', '90', ...
    'Position', [60 85 100 115]);

add_block('simulink/Discontinuities/Rate Limiter', ...
    [model '/Actuator Rate Limit'], ...
    'RisingSlewLimit', '30', ...
    'FallingSlewLimit', '-30', ...
    'Position', [160 78 270 122]);

add_block('simulink/Discontinuities/Saturation', ...
    [model '/Mechanical Limits'], ...
    'UpperLimit', '90', ...
    'LowerLimit', '0', ...
    'Position', [330 80 425 120]);

add_block('simulink/Sinks/Scope', ...
    [model '/Position Scope'], ...
    'Position', [490 78 535 122]);

add_line(model, 'Open Command/1', 'Actuator Rate Limit/1');
add_line(model, 'Actuator Rate Limit/1', 'Mechanical Limits/1');
add_line(model, 'Mechanical Limits/1', 'Position Scope/1');

set_param(model, 'StopTime', '6');
save_system(model, [model '.slx']);

disp('Created day3_toilet_actuator_model.slx');
disp('Command: 0 -> 90 deg at t=1 s');
disp('Rate limit: 30 deg/s');
disp('Expected opening time after command: ~3 s');

open_system(model);
