# Demonstration script

Roughly five minutes. Each row is one action and the one line worth saying
while it happens.

## Before the demo

```
build.bat                 :: must finish with "Build succeeded"
del patients.txt treatments.txt beds.txt
```

Starting from empty data avoids leftover test records appearing on screen.

## Run

```
build.bat run
```

| # | Input | What appears | What to say |
|---|---|---|---|
| 1 | `1` → 101, ravi kumar, 34, fever, severity **4**, 100000 | Patient registered | "A routine case arrives first." |
| 2 | `1` → 102, sita devi, 58, chest pain, severity **1**, 110000 | Patient registered | "A cardiac case arrives an hour later." |
| 3 | `2` | 102 at the top with `***EMERGENCY***`, then 101 | **"This is the whole point. The later arrival is treated first because severity 1 outranks severity 4."** |
| 4 | `1` → ID `101` again | Rejected | "IDs are unique, so a duplicate is refused before anything else is asked." |
| 5 | `1` → ID `abc` | "Invalid input. Enter a number." | "Invalid input is rejected instead of silently becoming zero." `Ctrl+C` or finish the entry. |
| 6 | `5` | 101 (34) then 102 (58) | "Listed by age." |
| 7 | `2` | 102 then 101, unchanged | **"The queue is still in severity order. The sort worked on a copy — re-ordering the real queue would push the emergency patient down the line."** |
| 8 | `4` → `1` → 101 | ravi's record | "Linear search, O(n)." |
| 9 | `4` → `3` → 101 | ravi's record | "Binary search, O(log n) — but it must sort a copy first, so linear search is actually faster here." |
| 10 | `9` → 101, new severity `1` | "Queue has been re-ordered" | "The patient deteriorates; the queue re-orders itself." |
| 11 | `2` | 101 has moved up | "Severity alone is not enough — the position must change too." |
| 12 | `8` | "severity back to 4" | **"Undo. The stack is LIFO, so the most recent action reverses first. It goes through updateSeverity(), never through the queue nodes directly."** |
| 13 | `6` → `1` → bed `0`, patient `102` | Bed 0 allocated | "Bed allocation is also recorded on the undo stack." |
| 14 | `3` | 102 is treated, then asks for treatment details | "The most urgent patient is treated first." |
| 15 | → 12-11-2026, BP 150/95, Aspirin 75mg, ECG done | Treatment recorded | "The record is linked by patient ID." |
| 16 | `7` → 102 | The treatment record | "One patient can have many visits, so history is a separate linked list." |
| 17 | `10` | 102 with status `treated` | "Treated patients are retained, not deleted." |
| 18 | `11` | "All data saved" | — |
| 19 | `build.bat run` again | "Previous patient data loaded." | **"Restart. The data survives, and the queue rebuilds in priority order because loading re-inserts through enqueue()."** |

## Dashboard

Open `dashboard/index.html`, load `patients.txt`, `treatments.txt` and
`beds.txt`.

> "Read-only view of the same files. Severity colours, emergency markers, bed
> occupancy, searchable treatment history. It never writes, so it cannot put
> the program's state out of step."

## Tests

```
build.bat test
```

> "28 assertions, all passing — ordering, the tie-break, undo, boundary cases
> such as an empty queue and an out-of-range bed number."

## Expected questions

**Why a priority queue and not a normal queue?**
FIFO is unsafe in triage; a cardiac case cannot wait behind a fever case.

**Why a linked list and not an array?**
Insertion in the middle is O(1) once the position is found, instead of
shifting every element, and the size does not need fixing in advance.

**Why is severity 1 the most urgent?**
Real triage scales work that way, and it reduces the comparison to `<`.

**Why not use inheritance for emergency patients?**
The queue stores `Patient` by value, so a derived object would be sliced.
Pointers would fix it but would change every module. A `visitType` field plus
an emergency marker achieves the same result.

**Where is the database?**
Storage is file-based with a record layout that mirrors a relational schema.
The schema, ER diagram and migration path are in `docs/SCHEMA.md`.

**Is there a memory leak?**
`dequeue()` and `updateSeverity()` delete nodes, and `~PriorityQueue()` frees
both the waiting and treated lists at exit.

**What would break at 10,000 patients?**
`enqueue()` is O(n). A binary heap would make it O(log n); the trade-off is
that a heap is array-based and would not demonstrate a linked list.

**What does not work yet?**
No database connection; symptom text cannot be clinically validated; sorting
is capped at 100 patients; single-user only. All recorded in
`docs/TEST_RESULTS.md`.
