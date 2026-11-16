# Storage layer and schema

The system stores data in plain text files with a fixed record layout. The
layout is deliberately shaped like a relational schema, so that moving to
MySQL or SQLite later changes only the read/write code, not the data model.

**Delimiter:** `|` (pipe), one record per line.
Pipe is used rather than comma because a symptom field may itself contain a
comma — "chest pain, sweating" — which would break a comma-separated split.

---

## patients.txt  →  table `patients`

| # | Column | Type | Constraint | Example |
|---|---|---|---|---|
| 1 | patient_id | INT | PRIMARY KEY | 102 |
| 2 | name | VARCHAR(50) | NOT NULL | sita devi |
| 3 | age | INT | 0–120 | 58 |
| 4 | symptoms | VARCHAR(200) | NOT NULL | chest pain |
| 5 | severity | INT | 1–5, 1 = most critical | 1 |
| 6 | arrival_time | CHAR(6) | HHMMSS, 24-hour | 110000 |
| 7 | visit_type | VARCHAR(20) | Routine \| Emergency, derived from severity | Emergency |
| 8 | status | VARCHAR(10) | waiting \| treated | treated |

```
102|sita devi|58|chest pain|1|110000|Emergency|treated
```

`arrival_time` is stored as text, not as an integer, because an integer would
drop the leading zero in a time such as `090000` and turn six digits into
five, breaking the parser.

`status` exists so a treated patient stays in the record instead of being
deleted on discharge — the same reason a real system would soft-delete.

---

## treatments.txt  →  table `treatment_records`

| # | Column | Type | Constraint | Example |
|---|---|---|---|---|
| 1 | patient_id | INT | FOREIGN KEY → patients.patient_id | 102 |
| 2 | treatment_date | VARCHAR(12) | dd-mm-yyyy | 12-11-2026 |
| 3 | vitals | VARCHAR(100) | | BP 150/95, pulse 96 |
| 4 | medicine | VARCHAR(100) | | Aspirin 75mg |
| 5 | notes | VARCHAR(200) | | ECG done, admitted |

```
102|12-11-2026|BP 150/95, pulse 96|Aspirin 75mg|ECG done, admitted
```

One patient has many treatment records, so this is a one-to-many relation
keyed on `patient_id`. This is why treatment history is a separate class
rather than a field inside `Patient`.

---

## beds.txt  →  table `beds`

| # | Column | Type | Constraint | Example |
|---|---|---|---|---|
| 1 | bed_no | INT | PRIMARY KEY, 0 … n−1 | 0 |
| 2 | patient_id | INT | 0 = free, else FK → patients | 102 |

```
0|102
1|0
```

---

## Entity relationship

```
   patients                      treatment_records
┌────────────────┐            ┌─────────────────────┐
│ patient_id PK  │───────1:N──│ patient_id      FK  │
│ name           │            │ treatment_date      │
│ age            │            │ vitals              │
│ symptoms       │            │ medicine            │
│ severity       │            │ notes               │
│ arrival_time   │            └─────────────────────┘
│ visit_type     │
│ status         │                     beds
└────────┬───────┘            ┌─────────────────────┐
         └──────────0:1───────│ bed_no          PK  │
                              │ patient_id      FK  │
                              └─────────────────────┘
```

A patient occupies at most one bed; a bed holds at most one patient.

---

## Migration path

The in-memory structures do not change. Only four functions touch storage:

| Current | Would become |
|---|---|
| `PriorityQueue::saveToFile()` / `loadFromFile()` | `INSERT` / `SELECT` on `patients` |
| `TreatmentHistory::saveToFile()` / `loadFromFile()` | `INSERT` / `SELECT` on `treatment_records` |
| `BedManager::saveToFile()` / `loadFromFile()` | `UPDATE` / `SELECT` on `beds` |

The undo stack is intentionally not persisted: "the last action" has no
meaning across sessions.
