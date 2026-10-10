%% Smart Assisted-Care Hospital Bed
% Day 5 - Final Verification Coverage
% Fill FINAL_VALIDATION_RESULTS.csv with:
%   1 = PASS
%   0 = FAIL
%
% This version accepts numeric or text import types safely.

clear;
clc;
close all;

resultsFile = fullfile('..','..','tests','FINAL_VALIDATION_RESULTS.csv');

T = readtable(resultsFile);

if ~ismember('Passed', T.Properties.VariableNames)
    error('The CSV must contain a column named Passed.');
end

passedColumn = T.Passed;

%% Convert Passed column safely
if isnumeric(passedColumn) || islogical(passedColumn)

    passed = double(passedColumn);

    if all(isnan(passed))
        error('Fill the Passed column with 1 (PASS) or 0 (FAIL) before running.');
    end

    if any(isnan(passed))
        error('Some Passed cells are blank. Fill every row with 1 or 0.');
    end

elseif isstring(passedColumn) || ischar(passedColumn) || iscell(passedColumn)

    passedText = string(passedColumn);
    passedText = strtrim(passedText);

    if all(ismissing(passedText) | passedText == "")
        error('Fill the Passed column with 1 (PASS) or 0 (FAIL) before running.');
    end

    passed = nan(size(passedText));

    passMask = strcmpi(passedText, "1") | strcmpi(passedText, "PASS");
    failMask = strcmpi(passedText, "0") | strcmpi(passedText, "FAIL");

    passed(passMask) = 1;
    passed(failMask) = 0;

    if any(isnan(passed))
        error('Passed must contain only 1/0 or PASS/FAIL.');
    end

else
    error('Unsupported data type for Passed column.');
end

%% Validate values
if any(~ismember(passed, [0 1]))
    error('Passed must contain only 1 (PASS) or 0 (FAIL).');
end

%% Calculate results
totalTests = height(T);
passedTests = sum(passed == 1);
failedTests = sum(passed == 0);
passRatePct = 100 * passedTests / totalTests;

fprintf('\nFINAL VERIFICATION SUMMARY\n');
fprintf('--------------------------\n');
fprintf('Total tests : %d\n', totalTests);
fprintf('Passed      : %d\n', passedTests);
fprintf('Failed      : %d\n', failedTests);
fprintf('Pass rate   : %.1f %%\n\n', passRatePct);

%% Plot
figure('Name','Final Prototype Verification');
bar([passedTests failedTests]);
xticks([1 2]);
xticklabels({'PASS','FAIL'});
ylabel('Number of tests');
title(sprintf('Final Verification Results - %.1f%% Pass Rate', passRatePct));
grid on;

%% Save summary
summary = table( ...
    totalTests, ...
    passedTests, ...
    failedTests, ...
    passRatePct, ...
    'VariableNames', ...
    {'TotalTests','PassedTests','FailedTests','PassRate_pct'} ...
);

writetable(summary, 'day5_final_verification_summary.csv');

disp('Saved: day5_final_verification_summary.csv');

%% Final status message
if failedTests == 0
    disp('FINAL RESULT: All verification tests passed.');
else
    fprintf('FINAL RESULT: %d verification test(s) still require attention.\n', failedTests);
end
