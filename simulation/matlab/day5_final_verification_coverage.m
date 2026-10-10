%% Smart Assisted-Care Hospital Bed
% Day 5 - Final Verification Coverage
% Fill FINAL_VALIDATION_RESULTS.csv with Passed = 1 or 0 before running.

clear; clc; close all;

T = readtable(fullfile('..','..','tests','FINAL_VALIDATION_RESULTS.csv'), ...
              'TextType','string');

if all(ismissing(T.Passed) | strlength(T.Passed)==0)
    error('Fill the Passed column with 1 (PASS) or 0 (FAIL) before running.');
end

passed = str2double(T.Passed);
if any(isnan(passed))
    error('Passed must contain only 1 or 0.');
end

totalTests = height(T);
passedTests = sum(passed == 1);
failedTests = sum(passed == 0);
coveragePct = 100 * passedTests / totalTests;

fprintf('FINAL VERIFICATION SUMMARY\n');
fprintf('--------------------------\n');
fprintf('Total tests : %d\n', totalTests);
fprintf('Passed      : %d\n', passedTests);
fprintf('Failed      : %d\n', failedTests);
fprintf('Pass rate   : %.1f %%\n', coveragePct);

figure('Name','Final Prototype Verification');
bar([passedTests failedTests]);
xticklabels({'PASS','FAIL'});
ylabel('Number of tests');
title(sprintf('Final Verification Results - %.1f%% Pass Rate', coveragePct));
grid on;

summary = table(totalTests, passedTests, failedTests, coveragePct, ...
    'VariableNames', {'TotalTests','PassedTests','FailedTests','PassRate_pct'});

writetable(summary, 'day5_final_verification_summary.csv');
disp('Saved day5_final_verification_summary.csv');
