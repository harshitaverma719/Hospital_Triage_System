// ============================================================
//  Tests - Search / Sort, Undo stack, Bed manager, Treatment
//  Build: g++ tests/test_modules.cpp -I include -o test_modules
// ============================================================

#include "patient.h"
#include "priorityqueue.h"
#include "search.h"
#include "undo.h"
#include "bed.h"
#include "treatment.h"

int passed = 0;
int failed = 0;

void check(string label, bool condition)
{
    if (condition)
    {
        cout << "  PASS  " << label << endl;
        passed++;
    }
    else
    {
        cout << "  FAIL  " << label << endl;
        failed++;
    }
}

// Helper: walk the queue and return the Nth id (0-based), or -1.
int idAt(PriorityQueue &q, int n)
{
    QueueNode *t = q.head;
    while (t != NULL && n > 0)
    {
        t = t->next;
        n--;
    }
    return (t == NULL) ? -1 : t->data.getid();
}

int main()
{
    cout << "=== Search / Undo / Bed test suite ===" << endl << endl;

    // --------------------------------------------------------
    // S1  Sorting by age must NOT disturb the live queue.
    //     This is the single most important property of the
    //     sort module: the waiting queue stays in severity
    //     order, because re-ordering it by age would push an
    //     emergency patient down the line.
    // --------------------------------------------------------
    {
        PriorityQueue q;
        q.enqueue(Patient(101, "ravi", 30, "fever",      4, "100000"));
        q.enqueue(Patient(102, "sita", 45, "chest pain", 2, "090000"));
        q.enqueue(Patient(103, "amit", 25, "injury",     1, "080000"));

        int beforeHead   = idAt(q, 0);
        int beforeSecond = idAt(q, 1);
        int beforeThird  = idAt(q, 2);

        displaySortedByAge(q);

        check("S1  queue order is unchanged after sorting a copy",
              idAt(q, 0) == beforeHead &&
              idAt(q, 1) == beforeSecond &&
              idAt(q, 2) == beforeThird);

        check("S1b queue is still in severity order (103, 102, 101)",
              idAt(q, 0) == 103 && idAt(q, 1) == 102 && idAt(q, 2) == 101);
    }

    // --------------------------------------------------------
    // S2  Sorting an empty queue is handled without crashing
    // --------------------------------------------------------
    {
        PriorityQueue q;
        displaySortedByAge(q);
        check("S2  sorting an empty queue is handled", q.isEmpty());
    }

    // --------------------------------------------------------
    // U1  Undo of a severity change restores the old value
    //     AND the old queue position.
    // --------------------------------------------------------
    {
        PriorityQueue q;
        BedManager    bm(5);
        UndoStack     st;

        q.enqueue(Patient(201, "a", 30, "x", 4, "100000"));
        q.enqueue(Patient(202, "b", 30, "x", 2, "100000"));

        st.push("severity", 201, 0, 4);
        q.updateSeverity(201, 1);

        bool movedUp = (idAt(q, 0) == 201);

        undoLastAction(st, bm, q);

        check("U1a severity change moved the patient up", movedUp);
        check("U1b undo restores severity 4",
              q.getSeverityOf(201) == 4);
        check("U1c undo restores the original order (202 first)",
              idAt(q, 0) == 202);
    }

    // --------------------------------------------------------
    // U2  Undo on an empty stack must not crash
    // --------------------------------------------------------
    {
        PriorityQueue q;
        BedManager    bm(5);
        UndoStack     st;

        undoLastAction(st, bm, q);
        check("U2  undo on an empty stack is handled", st.isEmpty());
    }

    // --------------------------------------------------------
    // B1  Bed allocate / release, and undo of each
    // --------------------------------------------------------
    {
        PriorityQueue q;
        BedManager    bm(5);
        UndoStack     st;

        bm.allocateBed(st, 0, 301);
        check("B1a bed 0 is occupied after allocation",
              bm.isOccupied(0));

        undoLastAction(st, bm, q);
        check("B1b undo frees the bed", !bm.isOccupied(0));

        bm.allocateBed(st, 1, 302);
        bm.releaseBed(st, 1);
        check("B1c bed 1 is free after release", !bm.isOccupied(1));

        undoLastAction(st, bm, q);
        check("B1d undo of a release restores the patient",
              bm.isOccupied(1));
    }

    // --------------------------------------------------------
    // B2  Out-of-range bed numbers are rejected, not written
    //     A negative index would otherwise corrupt memory.
    // --------------------------------------------------------
    {
        PriorityQueue q;
        BedManager    bm(5);
        UndoStack     st;

        bm.allocateBed(st, -3, 401);
        bm.allocateBed(st, 99, 402);

        check("B2  invalid bed numbers are rejected safely",
              st.isEmpty());
    }

    // --------------------------------------------------------
    // T1  Treatment history is linked by patient ID and is
    //     kept in chronological order (appended at the end).
    // --------------------------------------------------------
    {
        TreatmentHistory th;
        th.addTreatment(501, "01-01-2026", "BP 120/80", "Paracetamol", "first visit");
        th.addTreatment(501, "05-01-2026", "BP 118/78", "Rest",        "follow up");
        th.addTreatment(502, "02-01-2026", "BP 140/90", "Aspirin",     "other patient");

        check("T1a history is stored for the right patient",
              th.countFor(501) == 2);
        check("T1b other patients are not mixed in",
              th.countFor(502) == 1);
        check("T1c a patient with no history reports zero",
              th.countFor(999) == 0);
    }

    cout << endl;
    cout << "Passed: " << passed << "   Failed: " << failed << endl;
    return failed == 0 ? 0 : 1;
}
