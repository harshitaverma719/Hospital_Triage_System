#include"patient.h"
#include"priorityqueue.h"
#include<string>
using namespace std;

void registerPatientModule(PriorityQueue &q)
{
    int id, age, severity;
    string name, symptoms, atime, vtype;

    cout << "Enter Patient ID: ";
    cin >> id;
    cin.ignore();

    cout << "Enter Name: ";
    getline(cin, name);

    cout << "Enter Age: ";
    cin >> age;
    cin.ignore();

    cout << "Enter Symptoms: ";
    getline(cin, symptoms);

    cout << "Enter Severity (1 = most critical, 5 = least): ";
    cin >> severity;
    cin.ignore();

    cout << "Enter Arrival Time (HHMMSS): ";
    getline(cin, atime);

    cout << "Enter Visit Type (Emergency / Followup / Routine): ";
    getline(cin, vtype);

    Patient p(id, name, age, symptoms, severity, atime);
    p.setvisitType(vtype);

    q.enqueue(p);
    cout << "Patient registered successfully." << endl;
}

int main()
{
    PriorityQueue q;
    q.loadFromFile();
    int choice;

    while (true)
    {
        cout << endl;
        cout << "===== HOSPITAL TRIAGE SYSTEM =====" << endl;
        cout << "1. Register Patient" << endl;
        cout << "2. View Queue" << endl;
        cout << "3. Treat Next Patient" << endl;
        cout << "4. Update Severity" << endl;
        cout << "5. View Treated Patients" << endl;
        cout << "6. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;
        cin.ignore();

        if (choice == 1)
        {
            registerPatientModule(q);
        }
        else if (choice == 2)
        {
            q.display();
        }
        else if (choice == 3)
        {
            q.peek();
            q.dequeue();
        }
        else if (choice == 4)
        {
            int pid, newSev;
            cout << "Enter Patient ID: ";
            cin >> pid;
            cout << "Enter New Severity (1-5): ";
            cin >> newSev;
            cin.ignore();
            q.updateSeverity(pid, newSev);
        }
        else if (choice == 5)
        {
            q.displayTreated();
        }
        else if (choice == 6)
        {
            q.saveToFile();
            cout << "Exiting..." << endl;
            break;
        }
        else
        {
            cout << "Invalid choice. Try again." << endl;
        }
    }

    return 0;
}