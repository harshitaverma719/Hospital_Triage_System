# Architecture notes

One file per module, header-only, each with an include guard. Modules
communicate only through public member functions.

---

## Module 1 — `patient.h`, `priorityqueue.h`  (Tanu)

### Patient

Eight private fields with public getters and setters. Private fields mean no
caller can write `p.severity = 99`; a change has to go through `setseverity()`
where a check can live.

`arrival_time` is a `string`, not an `int`. An integer drops the leading zero
in `090000`, turning six digits into five and breaking the parser.
`getArrivalTime()` converts `HHMMSS` into seconds so comparison is arithmetic
rather than string ordering, and returns 0 when the length is wrong instead of
calling `substr` out of range.

There is no hand-written copy constructor. An earlier version had one that
forgot `visitType`; since `enqueue()` takes the patient by value, the copy
constructor ran on every insertion and silently blanked the field. The
compiler-generated version copies every member.

`visitType` is derived from severity rather than asked from the user, so a
severity-5 patient can never be labelled "Emergency".

### PriorityQueue

A **sorted singly linked list**. The list is kept in order at insertion time,
so the head is always the most urgent patient.

`higherPriority(a, b)` holds the entire ordering rule:

```cpp
if (a.getSeverity() != b.getSeverity())
    return a.getSeverity() < b.getSeverity();   // lower number = more urgent
return a.getArrivalTime() < b.getArrivalTime(); // tie-break
```

`enqueue()` handles two cases — the new patient belongs at the head, or it
belongs after some node further along. The two pointer assignments must run
in this order:

```cpp
newNode->next = temp->next;   // attach forward first
temp->next    = newNode;      // then attach backward
```

Reversing them loses the rest of the list.

`dequeue()` is O(1): it removes the head, moves the treated patient to a
second list, and deletes the node. The array version this replaced was O(n)
because every element had to shift.

`updateSeverity()` does not simply call `setseverity()`. Changing the field
alone would leave the patient in the wrong position — a patient who becomes
critical would still be at the back. Instead the node is unlinked and
deleted, the severity is set, and the patient is re-inserted through
`enqueue()`, which finds the correct position. This is why the undo module is
required to call `updateSeverity()` rather than touching nodes directly.

`~PriorityQueue()` walks both lists and deletes every node, so the memory
taken by `new` in `enqueue()` is released at exit.

---

## Module 2 — `treatment.h`  (Harshita Verma)

A separate singly linked list keyed on `patientId`, not a field inside
`Patient`, because one patient can have many visits — a one-to-many relation.

`addTreatment()` appends at the end so history stays chronological; inserting
at the head would reverse it.

`displayHistory()` uses a `found` flag, so a patient with no history gets a
message rather than a blank screen.

The integration point with the queue is order-sensitive:

```cpp
int treatedId = q.head->data.getid();   // read the ID first
q.dequeue();                            // the node is deleted here
th.addTreatment(treatedId, ...);
```

Reading the ID after `dequeue()` would dereference freed memory.

---

## Module 3 — `undo.h`, `bed.h`  (Ishita Dhiman)

### UndoStack

An array-based stack with a `top` index; `top == -1` means empty. Push and
pop are O(1).

A stack is the right structure because undo always reverses the *most recent*
action — exactly LIFO. A queue would reverse the oldest action first.

One `ActionRecord` type covers all three action kinds:

```cpp
struct ActionRecord {
    string actionType;   // "allocate" | "release" | "severity"
    int patientId;
    int bedNumber;
    int oldSeverity;
};
```

A single shared stack matters: with separate stacks for beds and severity
there would be no way to know which action was genuinely last.

### undoLastAction()

Deliberately a free function rather than a member, because it needs both the
`BedManager` and the `PriorityQueue`. Putting it inside `BedManager` would
force the bed manager to know about the queue — an unnecessary coupling.

The severity branch calls `q.updateSeverity()`. That is what restores the
patient's queue position along with the value.

The push must happen *before* the change, or the old value is already gone.

### BedManager

An integer array where the index is the bed number and `0` means free.
Allocation and release are O(1).

Both operations bounds-check `bedNumber >= 0 && bedNumber < totalBeds`.
Without the lower bound, a negative index would write outside the array.

---

## Module 4 — `search.h`, `validation.h`, `main.cpp`  (Ayush)

### Search and sort

`searchById()` returns on the first match because IDs are unique.
`searchByName()` deliberately does not return early — names can repeat — and
uses a `found` flag to report "not found" after the full traversal.

`displaySortedByAge()` copies into an array and sorts the **copy**. Sorting
the live queue by age would move an emergency patient down the line and
defeat the purpose of the system. This is the single most important design
decision in the module.

`binarySearchById()` sorts the copy by **ID**, not age, because binary search
only works on the key the data is sorted by.

### Validation

| Function | Rejects |
|---|---|
| `readInt(prompt, min, max)` | Non-numeric input, and values outside the range |
| `readName(prompt)` | Empty input, and input with no letters |
| `readLine(prompt)` | Empty input only — for dates, vitals and notes, where digits and punctuation are legitimate |
| `readTime(prompt)` | Wrong length, non-digits, and impossible values such as `146086` |

`readInt()` needs both calls on failure:

```cpp
cin.clear();              // clear the fail flag
cin.ignore(1000, '\n');   // discard the offending text
```

With only `clear()` the bad text stays in the buffer and is re-read forever.
With only `ignore()` the stream is still in a fail state and reads nothing.

`readName()` uses `getline()` because `cin >>` stops at whitespace and would
truncate "ravi kumar" to "ravi", pushing "kumar" into the next field.

`readLine()` was added after testing: dates were being rejected because
`readName()` demands a letter and `12-10-2026` has none. One validation rule
does not fit every field.

### Integration

`main.cpp` owns the menu and the object lifetimes. Each menu branch delegates
to a module; no business logic lives here. Modules were merged one at a time
and compiled after each, rather than all at once.

---

## Storage

Each module owns its own file: `patients.txt`, `treatments.txt`, `beds.txt`.
Pipe-delimited, one record per line. Layout and migration path are in
`SCHEMA.md`.

`loadFromFile()` re-inserts each patient through `enqueue()` rather than
rebuilding the list directly, so the queue order is recomputed from the
ordering rule and does not depend on the order of lines in the file.

The undo stack is not persisted: "the last action" has no meaning across
sessions.

---

## Dashboard

`dashboard/index.html` is a single self-contained file — no build step, no
server, no internet. It parses the same pipe-delimited files and re-applies
the triage ordering rule in JavaScript rather than trusting the file's line
order, so it stays consistent with the C++ side.

It is read-only by design. A dashboard that could write would be a second
source of truth and could put the program's state out of step.
