# Test results

Run with `build.bat test` (Windows) or `./build.sh test`.
**28 assertions, 28 passing.**

## Suite 1 — `tests/test_queue.cpp` (13 assertions)

| ID | What it checks | Why it matters | Result |
|---|---|---|---|
| T1 | A new queue reports empty | Baseline | Pass |
| T2 | Severity 1 arriving later is placed at the head | The core triage rule | Pass |
| T3 | Equal severity falls back to arrival time | Fairness within a severity band | Pass |
| T4 | `dequeue()` removes the head only | Queue integrity | Pass |
| T5 | `dequeue()` on an empty queue does not crash | Boundary | Pass |
| T6a/b | `contains()` finds a present ID, rejects an absent one | Backs duplicate-ID rejection | Pass |
| T7 | `updateSeverity()` moves the patient, not just the field | Without this a newly critical patient stays at the back | Pass |
| T8a/b | A dequeued patient is retained with status `treated` | Records are not lost on discharge | Pass |
| T9a | `014530` converts to 6330 seconds | Time parsing | Pass |
| T9b | A 5-digit time returns 0 instead of crashing | Previously crashed on bad `substr` offsets | Pass |
| T10 | Save then reload rebuilds the queue in priority order | Load re-inserts via `enqueue()`, so order does not depend on file order | Pass |

## Suite 2 — `tests/test_modules.cpp` (15 assertions)

| ID | What it checks | Why it matters | Result |
|---|---|---|---|
| S1 | Sorting by age leaves the live queue untouched | Sorting the real queue would push an emergency patient down the line | Pass |
| S1b | Queue remains in severity order afterwards | Same | Pass |
| S2 | Sorting an empty queue is handled | Boundary | Pass |
| U1a | A severity change moves the patient up | Setup for the undo check | Pass |
| U1b | Undo restores the old severity | The undo deliverable | Pass |
| U1c | Undo restores the original queue order | Undo goes through `updateSeverity()`, never through raw nodes | Pass |
| U2 | Undo on an empty stack does not crash | Boundary | Pass |
| B1a–d | Bed allocate, release, and undo of each | Bed module + stack integration | Pass |
| B2 | Bed numbers −3 and 99 are rejected | A negative index would corrupt memory | Pass |
| T1a–c | Treatment history is keyed correctly per patient | One-to-many relation | Pass |

## Manual end-to-end test

Register two patients (severity 4, then severity 1) → the severity-1 patient
appears first with the emergency marker → sort by age → the queue is still in
severity order → change a severity → undo → the original severity and
position return → allocate a bed → treat the next patient → the treatment
record appears under that patient → save and exit → restart → all data
reloads in the correct order.

## Bugs found during development and fixed

| Bug | Symptom | Cause | Fix |
|---|---|---|---|
| Crash on display | Program terminated when showing a patient | Wrong `substr` offsets in arrival-time parsing | Correct offsets plus a length guard |
| Visit type always blank | Field was empty in every record | A hand-written copy constructor omitted it, and `enqueue` takes the patient by value | Removed the copy constructor so the compiler-generated one copies every field |
| Silent corruption on bad input | Typing text into a numeric field produced ID 0, skipped all later fields, and the bad record jumped ahead of real emergencies | `cin` entered a fail state with nothing to clear it | `readInt()` with `cin.clear()` and `cin.ignore()` |
| Multi-word names truncated | "ravi kumar" stored as "ravi" | `cin >>` stops at whitespace | `getline()` in `readName()` |
| Invalid times accepted | `146086` passed as a time | Only the length was checked | `readTime()` validates hours ≤ 23, minutes and seconds ≤ 59 |
| Date field rejected | "12-10-2026" refused as a date | `readName()` requires a letter and was being used for the date | Added `readLine()` for free-text fields |
| Treated patients disappeared | Records vanished from the file after treatment | Save wrote only from the live queue | Added a `status` column and a retained treated list |

## Known limitations

- **No database connection.** Storage is file-based; the schema and ER
  diagram are in `SCHEMA.md` and the migration path is documented there.
- **Symptom text cannot be clinically validated.** The program checks only
  that the field is non-empty.
- **Sorting is capped at 100 patients** by the fixed temporary array.
- **Single session, single user.** No concurrent access or locking.
- **The dashboard is read-only and not live.** It shows the last saved state.
