# Day 4B Normal Full-Cycle Test

Start with:
- Patient = YES
- Obstruction = NO
- Open Limit = OFF
- Closed Limit = ON
- Water = YES
- Drain = YES
- Leak = NO

Sequence:
1. Press TOILET once.
2. When opening starts, set Closed Limit OFF.
3. When toilet servo reaches 90 deg, set Open Limit ON.
4. Wait for IN_USE.
5. Double-press TOILET.
6. When closing starts, set Open Limit OFF.
7. When toilet servo reaches 0 deg, set Closed Limit ON.
8. Confirm the automatic states:
   SECURED -> SANITATION_CHECK -> WASTE_EVAC -> PRIMARY_FLUSH -> DRAIN_1 -> RINSE_1 -> DRAIN_2 -> FINAL_RINSE -> FINAL_DRAIN -> CYCLE_VERIFY -> CLEAN_COMPLETE -> NORMAL

Expected outputs:
- Waste servo opens during evacuation/flush/rinse/drain stages.
- FLUSH LED only during PRIMARY_FLUSH.
- RINSE LED during RINSE_1 and FINAL_RINSE.
- DRAIN LED during WASTE_EVAC, DRAIN_1, DRAIN_2 and FINAL_DRAIN.
- SANITATION LED remains on throughout sanitation states.
- Patient monitoring continues throughout the cycle.
