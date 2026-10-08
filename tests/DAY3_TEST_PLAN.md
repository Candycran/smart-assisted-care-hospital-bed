# Day 3 Verification

Fill in Actual Result and PASS/FAIL.

| ID | Scenario | Expected Result | Actual Result | Pass/Fail |
|---|---|---|---|---|
| D3-01 | Correct initial switches | NORMAL; closed position confirmed | NORMAL; closed position confirmed | Pass|
| D3-02 | Patient absent + toilet request | REQUEST_REJECTED; no opening | REQUEST_REJECTED; no opening | Pass |
| D3-03 | Safe toilet request | SAFETY_CHECK -> OPENING | SAFETY_CHECK -> OPENING| Pass |
| D3-04 | During opening: CLOSED OFF then OPEN ON | TOILET_READY -> IN_USE | TOILET_READY -> IN_USE | Pass |
| D3-05 | One finish click | No securing action | | Pass |
| D3-06 | Double finish click | FINISH_RECEIVED -> SECURING |FINISH_RECEIVED -> SECURING | Pass |
| D3-07 | During closing: OPEN OFF then CLOSED ON | SECURED | SECURED | Pass |
| D3-08 | Obstruction before opening | FAULT | FAULT | Pass |
| D3-09 | Obstruction during opening | movement stops; FAULT | movement stops; FAULT | Pass |
| D3-10 | Obstruction during securing | movement stops; FAULT | movement stops; FAULT | Pass |
| D3-11 | Never activate OPEN LIMIT | opening timeout; FAULT | opening timeout; FAULT | Pass |
| D3-12 | Never activate CLOSED LIMIT | closing timeout; FAULT | closing timeout; FAULT | Pass |
| D3-13 | Both limit switches active | FAULT | FAULT | Pass |
| D3-14 | Emergency during motion | EMERGENCY latched | EMERGENCY latched| Pass |
