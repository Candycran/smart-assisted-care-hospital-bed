# Day 3 Verification

Fill in Actual Result and PASS/FAIL.

| ID | Scenario | Expected Result | Actual Result | Pass/Fail |
|---|---|---|---|---|
| D3-01 | Correct initial switches | NORMAL; closed position confirmed | | |
| D3-02 | Patient absent + toilet request | REQUEST_REJECTED; no opening | | |
| D3-03 | Safe toilet request | SAFETY_CHECK -> OPENING | | |
| D3-04 | During opening: CLOSED OFF then OPEN ON | TOILET_READY -> IN_USE | | |
| D3-05 | One finish click | No securing action | | |
| D3-06 | Double finish click | FINISH_RECEIVED -> SECURING | | |
| D3-07 | During closing: OPEN OFF then CLOSED ON | SECURED | | |
| D3-08 | Obstruction before opening | FAULT | | |
| D3-09 | Obstruction during opening | movement stops; FAULT | | |
| D3-10 | Obstruction during securing | movement stops; FAULT | | |
| D3-11 | Never activate OPEN LIMIT | opening timeout; FAULT | | |
| D3-12 | Never activate CLOSED LIMIT | closing timeout; FAULT | | |
| D3-13 | Both limit switches active | FAULT | | |
| D3-14 | Emergency during motion | EMERGENCY latched | | |
