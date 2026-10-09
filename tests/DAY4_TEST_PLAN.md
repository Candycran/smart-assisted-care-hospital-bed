# Day 4 Verification

| ID | Scenario | Expected result | Actual result | Pass/Fail |
|---|---|---|---|---|
| D4-01 | Sanitation I/O test | New switches, waste servo and four output LEDs respond correctly | | |
| D4-02 | Full normal cycle | SECURED -> sanitation sequence -> NORMAL | | |
| D4-03 | Access not secured | Sanitation blocked; FAULT | | |
| D4-04 | Water unavailable before cycle | Sanitation blocked; FAULT | | |
| D4-05 | Drain unavailable before cycle | Sanitation blocked; FAULT | | |
| D4-06 | Leak present before cycle | Sanitation blocked; FAULT | | |
| D4-07 | Water lost during flush/rinse | Outputs stop; FAULT | | |
| D4-08 | Drain lost during evacuation/drain | Outputs stop; FAULT | | |
| D4-09 | Leak introduced during sanitation | All sanitation outputs stop; FAULT | | |
| D4-10 | Emergency during sanitation | Outputs stop; EMERGENCY | | |
| D4-11 | Patient vitals during sanitation | TFT/Serial continue updating | | |
| D4-12 | End state | SANITATION_COMPLETE -> NORMAL | | |
