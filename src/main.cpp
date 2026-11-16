

#include "patient.h"
#include "priorityqueue.h"
#include "treatment.h"
#include "undo.h"
#include "bed.h"
#include "search.h"
#include "validation.h"

const int TOTAL_BEDS = 10;


void registerPatient(PriorityQueue &q)
{
    int id = readInt("Patient ID: ", 1, 99999);

    
    if (q.contains(id))
    {
        cout << "Patient ID " << id << " already exists. Registration cancelled." << endl;
        return;
    }

    string name     = readName("Name: ");
    int    age      = readInt("Age: ", 0, 120);
    string symptoms = readLine("Symptoms: ");
    int    severity = readInt("Severity (1 = most critical, 5 = least): ", 1, 5);
    string atime    = readTime("Arrival time (HHMMSS): ");

    Patient p(id, name, age, symptoms, severity, atime);


    if (severity == 1)
        p.setvisitType("Emergency");
    else
        p.setvisitType("Routine");

    q.enqueue(p);
    cout << "Patient registered." << endl;
}


void treatNext(PriorityQueue &q, TreatmentHistory &th)
{
    if (q.isEmpty())
    {
        cout << "No patient in queue." << endl;
        return;
    }

    
    int treatedId = q.head->data.getid();

    cout << "Now treating:" << endl;
    q.head->data.display();
    q.dequeue();

    string d = readLine("Date (dd-mm-yyyy): ");
    string v = readLine("Vitals: ");
    string m = readLine("Medicine: ");
    string n = readLine("Notes: ");

    th.addTreatment(treatedId, d, v, m, n);
    cout << "Treatment recorded." << endl;
}

// ------------------------------------------------------------
//  Option 4 - Search sub-menu
// ------------------------------------------------------------
void searchMenu(PriorityQueue &q)
{
    cout << "  1. By ID (linear)" << endl;
    cout << "  2. By name (linear)" << endl;
    cout << "  3. By ID (binary search on sorted copy)" << endl;

    int s = readInt("Choice: ", 1, 3);

    if (s == 1)
    {
        int pid = readInt("Patient ID: ", 1, 99999);
        searchById(q, pid);
    }
    else if (s == 2)
    {
        string nm = readName("Name: ");
        searchByName(q, nm);
    }
    else
    {
        int pid = readInt("Patient ID: ", 1, 99999);
        binarySearchById(q, pid);
    }
}


void bedMenu(BedManager &bm, UndoStack &st)
{
    cout << "  1. Allocate bed" << endl;
    cout << "  2. Release bed" << endl;
    cout << "  3. Show all beds" << endl;
    cout << "  4. Available count" << endl;

    int b = readInt("Choice: ", 1, 4);

    if (b == 1)
    {
        int bn  = readInt("Bed number (0-9): ", 0, TOTAL_BEDS - 1);
        int pid = readInt("Patient ID: ", 1, 99999);
        bm.allocateBed(st, bn, pid);
    }
    else if (b == 2)
    {
        int bn = readInt("Bed number (0-9): ", 0, TOTAL_BEDS - 1);
        bm.releaseBed(st, bn);
    }
    else if (b == 3)
    {
        bm.displayBeds();
    }
    else
    {
        bm.availableCount();
    }
}


void updateSeverityMenu(PriorityQueue &q, UndoStack &st)
{
    int pid = readInt("Patient ID: ", 1, 99999);

    if (!q.contains(pid))
    {
        cout << "Patient not found." << endl;
        return;
    }

    int oldSev = q.getSeverityOf(pid);
    int newSev = readInt("New severity (1-5): ", 1, 5);

    
    st.push("severity", pid, 0, oldSev);
    q.updateSeverity(pid, newSev);
}


int main()
{
    PriorityQueue    q;
    TreatmentHistory th;
    BedManager       bm(TOTAL_BEDS);
    UndoStack        st;


    q.loadFromFile();
    th.loadFromFile();
    bm.loadFromFile();

    int choice = 0;

    while (choice != 11)
    {
        cout << endl;
        cout << "===== HOSPITAL TRIAGE MANAGEMENT SYSTEM =====" << endl;
        cout << " 1. Register Patient"      << endl;
        cout << " 2. View Waiting Queue"    << endl;
        cout << " 3. Treat Next Patient"    << endl;
        cout << " 4. Search Patient"        << endl;
        cout << " 5. List Patients by Age"  << endl;
        cout << " 6. Bed Management"        << endl;
        cout << " 7. Treatment History"     << endl;
        cout << " 8. Undo Last Action"      << endl;
        cout << " 9. Update Severity"       << endl;
        cout << "10. View Treated Patients" << endl;
        cout << "11. Save and Exit"         << endl;

        choice = readInt("Enter choice: ", 1, 11);

        if (choice == 1)
        {
            registerPatient(q);
        }
        else if (choice == 2)
        {
            q.display();
        }
        else if (choice == 3)
        {
            treatNext(q, th);
        }
        else if (choice == 4)
        {
            searchMenu(q);
        }
        else if (choice == 5)
        {
            displaySortedByAge(q);
        }
        else if (choice == 6)
        {
            bedMenu(bm, st);
        }
        else if (choice == 7)
        {
            int pid = readInt("Patient ID: ", 1, 99999);
            th.displayHistory(pid);
        }
        else if (choice == 8)
        {
            undoLastAction(st, bm, q);
        }
        else if (choice == 9)
        {
            updateSeverityMenu(q, st);
        }
        else if (choice == 10)
        {
            q.displayTreated();
        }
        else
        {
            q.saveToFile();
            th.saveToFile();
            bm.saveToFile();
            cout << "All data saved. Exiting." << endl;
        }
    }

    return 0;
}
