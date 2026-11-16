# Hospital Triage Management System

A console-based triage system written in C++ that orders patients by clinical
severity instead of arrival order, records their treatment history, manages
ward beds, and can undo the last staff action. A read-only HTML dashboard
displays the data the program saves.

**Team indudhiman.ddnuk** — B.Tech CSE, Graphic Era

| Member | Name | Module | Data structure |
|---|---|---|---|
| 1 | Tanu | Patient record + priority queue, file persistence | Sorted singly linked list |
| 2 | Harshita Verma | Treatment history | Singly linked list |
| 3 | Ishita Dhiman | Undo + bed management | Stack (LIFO) + array |
| 4 | Ayush | Search, sorting, input validation, integration | Array + linear/binary search |

> Educational college project. Severity is entered by hospital staff; the
> program performs no clinical assessment and is not intended for real
> medical use.

---

## 1. The problem

In an emergency department, treating patients in arrival order is unsafe: a
cardiac case that arrives at 11:00 would wait behind a fever case that arrived
at 10:00. Triage solves this by ordering patients on assessed severity. This
project implements that ordering, together with the record-keeping that goes
with it.

**Severity scale (frozen):** `1` = most critical … `5` = least urgent.
A smaller number means higher priority, so the comparison reduces to `<`.

**Tie-break:** equal severity falls back to earlier arrival time, so patients
of the same urgency are still treated fairly in the order they arrived.

---

## 2. Features

| # | Feature | Implemented by |
|---|---|---|
| 1 | Register a patient, with duplicate-ID rejection | M1 + M4 |
| 2 | View the waiting queue in triage order | M1 |
| 3 | Treat the next patient; the record moves to treatment history | M1 + M2 |
| 4 | Search by ID (linear), by name (linear), by ID (binary on a sorted copy) | M4 |
| 5 | List patients by age without disturbing the live queue | M4 |
| 6 | Allocate / release beds, show occupancy and free count | M3 |
| 7 | View a patient's full treatment history | M2 |
| 8 | Undo the last action — severity change, bed allocation or release | M3 |
| 9 | Update severity; the queue re-orders itself automatically | M1 |
| 10 | View treated patients (retained, not deleted) | M1 |
| 11 | Save all state to disk and reload it on the next run | M1, M2, M3 |

Input validation runs on every numeric, name and time field, so invalid input
is rejected with a message rather than silently accepted.

---

## 3. Architecture

```
                    ┌──────────────────────────┐
                    │      src/main.cpp        │   menu, 11 options
                    │  (integration layer)     │
                    └────────────┬─────────────┘
                                 │
        ┌──────────────┬─────────┴──────┬──────────────┬─────────────┐
        │              │                │              │             │
  ┌─────▼─────┐  ┌─────▼──────┐  ┌──────▼─────┐  ┌─────▼────┐  ┌─────▼──────┐
  │ patient.h │  │ treatment.h│  │  undo.h    │  │ search.h │  │validation.h│
  │priorityq..│  │            │  │  bed.h     │  │          │  │            │
  │   M1      │  │     M2     │  │     M3     │  │    M4    │  │     M4     │
  └─────┬─────┘  └─────┬──────┘  └──────┬─────┘  └──────────┘  └────────────┘
        │              │                │
        ▼              ▼                ▼
  patients.txt   treatments.txt     beds.txt         ← storage layer
        │              │                │
        └──────────────┴────────────────┘
                       │
             ┌─────────▼──────────┐
             │ dashboard/index.html│   read-only view
             └─────────────────────┘
```

Every header is self-contained and guarded. Modules never reach into each
other's internals: for example the undo module changes a patient's severity by
calling `q.updateSeverity()`, never by editing queue nodes directly. That
single rule is what keeps the queue ordering correct after an undo.

### Why these data structures

| Structure | Where | Why this one |
|---|---|---|
| Sorted singly linked list | Waiting queue | Insertion happens in the correct position, so the head is always the most urgent patient and `dequeue()` is O(1). An array would need every element shifted on removal. |
| Singly linked list | Treatment history | One patient has many visits; records are appended so the history stays chronological. |
| Stack (array, LIFO) | Undo | "Undo" means reverse the *most recent* action, which is exactly what LIFO gives. A queue would reverse the oldest action first. |
| Array | Bed manager | The bed number is the index, so lookup and update are O(1), and the number of beds is fixed. |
| Array (temporary copy) | Sorting / binary search | Bubble sort and binary search need index access, which a linked list does not provide. |

### Complexity

| Operation | Complexity | Note |
|---|---|---|
| `enqueue()` | O(n) | Walks the list to find the insertion point |
| `dequeue()` | O(1) | Head removal only |
| `peek()` | O(1) | Head is always highest priority |
| `contains()` / `searchById()` | O(n) | Linear traversal, stops on match |
| `searchByName()` | O(n) | No early exit — names can repeat |
| `updateSeverity()` | O(n) | Find + re-insert |
| `displaySortedByAge()` | O(n²) | Bubble sort on a copy |
| `binarySearchById()` | O(log n) search, **O(n²) overall** | A sort is required first |
| Stack `push` / `pop` | O(1) | Index arithmetic only |
| Bed allocate / release | O(1) | Direct index access |

**On binary search:** a linked list has no random access, so the "middle
element" cannot be reached directly. Each binary search therefore has to copy
the data into an array and sort it first, which costs O(n²) and outweighs the
O(log n) saving. Linear search is the faster choice at this scale. Binary
search pays off only when the data is already sorted and searched repeatedly.
Both are implemented so the two can be compared.

---

## 4. Project layout

```
hospital-triage-system/
├── README.md
├── build.bat                 Windows build / run / test
├── build.sh                  Linux and macOS equivalent
├── include/                  all modules, header-only
│   ├── patient.h             Patient record            (M1)
│   ├── priorityqueue.h       Triage queue + persistence(M1)
│   ├── treatment.h           Treatment history         (M2)
│   ├── undo.h                Undo stack                (M3)
│   ├── bed.h                 Bed manager + undo action (M3)
│   ├── search.h              Search and sorting        (M4)
│   └── validation.h          Input validation          (M4)
├── src/
│   ├── main.cpp              Integrated application
│   └── triage_module.cpp     Member 1 standalone module
├── tests/
│   ├── test_queue.cpp        13 assertions on patient + queue
│   └── test_modules.cpp      15 assertions on search/undo/beds/history
├── dashboard/
│   └── index.html            Read-only dashboard (no install needed)
├── data/                     Sample data files for the dashboard
└── docs/
    ├── ARCHITECTURE.md       Module-by-module design notes
    ├── SCHEMA.md             Storage layer and table design
    ├── TEST_RESULTS.md       Test log and known limitations
    └── DEMO_SCRIPT.md        Step-by-step demonstration script
```

---

## 5. Build and run

Requires a C++ compiler (`g++`, MinGW on Windows). No libraries, no
installation, no internet access.

**Windows**

```bat
build.bat          :: compile everything
build.bat run      :: compile and start the program
build.bat test     :: compile and run both test suites
```

**Linux / macOS**

```sh
./build.sh
./build.sh run
./build.sh test
```

**Manually, if you prefer**

```sh
g++ src/main.cpp -I include -o triage
./triage
```

Data files (`patients.txt`, `treatments.txt`, `beds.txt`) are written next to
the executable when you choose **Save and Exit**, and reloaded on the next
start.

### The dashboard

Open `dashboard/index.html` in any browser, then use the three buttons to
select the data files. A browser cannot read files from disk on its own, so
the files are chosen explicitly — this is why the dashboard needs no server.

To see it populated immediately, load the sample files from `data/`.

The dashboard shows the waiting queue in triage order with severity colours
and emergency markers, treated patients, bed occupancy, and a searchable
treatment history. It is **read-only**: it never writes to the data files, so
it cannot put the program's state out of step.

---

## 6. Testing

```
build.bat test          (or ./build.sh test)
```

28 assertions across two suites, all passing. They cover severity ordering,
the arrival-time tie-break, O(1) dequeue, duplicate-ID rejection,
`updateSeverity()` re-ordering, undo of all three action types, undo on an
empty stack, out-of-range bed numbers, retention of treated patients,
save/reload round-trips, and the invalid-length guard in time parsing.

Full log and the list of known limitations: `docs/TEST_RESULTS.md`.

---

## 7. Known limitations

Stated openly rather than hidden:

- **No database.** Storage is file-based with a fixed record layout that
  mirrors a relational schema (see `docs/SCHEMA.md`); moving to MySQL or
  SQLite would change only the read/write layer, not the data model.
- **Symptom text is not clinically validated.** The program can only check
  that the field is non-empty; whether a symptom is medically coherent is
  outside what software can determine.
- **Single user, single session.** There is no concurrent access or locking.
- **The sorting helper copies into a fixed array of 100**, so sorting is
  capped at 100 patients.
- **The dashboard is read-only and not live.** It reflects the last saved
  state; re-select the file to refresh.

---

## 8. Scope deliberately excluded

Recorded in the Phase 1 proposal and unchanged: automatic doctor allocation,
nearby-hospital search, and any form of automatic diagnosis. Severity is a
clinical judgement and stays with qualified staff.
