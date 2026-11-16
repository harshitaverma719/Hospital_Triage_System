// ============================================================
//  Tests - Patient class and PriorityQueue
//  Build: g++ tests/test_queue.cpp -I include -o test_queue
// ============================================================

#include "patient.h"
#include "priorityqueue.h"

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

int main()
{
    cout << "=== PriorityQueue test suite ===" << endl << endl;

    // --------------------------------------------------------
    // T1  An empty queue reports itself as empty
    // --------------------------------------------------------
    {
        PriorityQueue q;
        check("T1  new queue is empty", q.isEmpty());
    }

    // --------------------------------------------------------
    // T2  A more severe patient moves ahead of an earlier one
    //     101 arrives first with severity 4
    //     102 arrives later with severity 1  -> must be at head
    // --------------------------------------------------------
    {
        PriorityQueue q;
        q.enqueue(Patient(101, "ravi", 30, "fever", 4, "100000"));
        q.enqueue(Patient(102, "sita", 45, "chest pain", 1, "110000"));

        check("T2  severity 1 is placed at the head",
              q.head->data.getid() == 102);
    }

    // --------------------------------------------------------
    // T3  Equal severity falls back to arrival time
    // --------------------------------------------------------
    {
        PriorityQueue q;
        q.enqueue(Patient(201, "later",  30, "fever", 3, "120000"));
        q.enqueue(Patient(202, "earlier", 31, "fever", 3, "090000"));

        check("T3  equal severity breaks the tie on arrival time",
              q.head->data.getid() == 202);
    }

    // --------------------------------------------------------
    // T4  dequeue removes the head and nothing else
    // --------------------------------------------------------
    {
        PriorityQueue q;
        q.enqueue(Patient(301, "a", 20, "x", 2, "100000"));
        q.enqueue(Patient(302, "b", 20, "x", 5, "100000"));
        q.dequeue();

        check("T4  dequeue removes the highest-priority patient",
              q.head != NULL && q.head->data.getid() == 302);
    }

    // --------------------------------------------------------
    // T5  dequeue on an empty queue must not crash
    // --------------------------------------------------------
    {
        PriorityQueue q;
        q.dequeue();
        check("T5  dequeue on empty queue is handled", q.isEmpty());
    }

    // --------------------------------------------------------
    // T6  contains() finds a present ID and rejects an absent one
    // --------------------------------------------------------
    {
        PriorityQueue q;
        q.enqueue(Patient(401, "a", 20, "x", 3, "100000"));

        check("T6a contains() finds an existing ID", q.contains(401));
        check("T6b contains() rejects an unknown ID", !q.contains(999));
    }

    // --------------------------------------------------------
    // T7  updateSeverity re-orders the queue, not just the field
    //     This is the behaviour that makes the system correct:
    //     a patient who becomes critical must move to the front.
    // --------------------------------------------------------
    {
        PriorityQueue q;
        q.enqueue(Patient(501, "low",  30, "x", 4, "100000"));
        q.enqueue(Patient(502, "mid",  30, "x", 2, "100000"));

        q.updateSeverity(501, 1);

        check("T7  updateSeverity moves the patient to the head",
              q.head->data.getid() == 501 && q.head->data.getSeverity() == 1);
    }

    // --------------------------------------------------------
    // T8  Treated patients are retained, not discarded
    // --------------------------------------------------------
    {
        PriorityQueue q;
        q.enqueue(Patient(601, "a", 20, "x", 1, "100000"));
        q.dequeue();

        check("T8  dequeued patient is kept in the treated list",
              q.treatedHead != NULL && q.treatedHead->data.getid() == 601);
        check("T8b treated patient carries status 'treated'",
              q.treatedHead->data.getStatus() == "treated");
    }

    // --------------------------------------------------------
    // T9  Arrival time parsing, including the invalid-length guard
    // --------------------------------------------------------
    {
        Patient good(701, "a", 20, "x", 3, "014530");   // 01:45:30
        Patient bad (702, "b", 20, "x", 3, "12345");    // 5 digits

        check("T9a HHMMSS converts to seconds",
              good.getArrivalTime() == 1 * 3600 + 45 * 60 + 30);
        check("T9b wrong-length time returns 0 instead of crashing",
              bad.getArrivalTime() == 0);
    }

    // --------------------------------------------------------
    // T10 Save and reload rebuilds the queue in priority order
    // --------------------------------------------------------
    {
        PriorityQueue q;
        q.enqueue(Patient(801, "a", 30, "fever", 4, "100000"));
        q.enqueue(Patient(802, "b", 40, "chest pain", 1, "110000"));
        q.saveToFile();

        PriorityQueue r;
        r.loadFromFile();

        check("T10 reloaded queue keeps priority order",
              r.head != NULL && r.head->data.getid() == 802);
    }

    cout << endl;
    cout << "Passed: " << passed << "   Failed: " << failed << endl;
    return failed == 0 ? 0 : 1;
}
