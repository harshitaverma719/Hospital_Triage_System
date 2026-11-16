#ifndef BED_H
#define BED_H
#include <iostream>
#include <string>
#include "priorityqueue.h"
#include "undo.h"
using namespace std;

class BedManager {
    int beds[50];
    int totalBeds;

public:
    BedManager(int n) {
        totalBeds = n;
        for (int i = 0; i < totalBeds; i++) beds[i] = 0;
    }

    void setBed(int b, int pid) { beds[b] = pid; }
    void clearBed(int b) { beds[b] = 0; }

    void allocateBed(UndoStack &st, int bedNumber, int patientId) {
        if (bedNumber >= 0 && bedNumber < totalBeds && beds[bedNumber] == 0) {
            beds[bedNumber] = patientId;
            cout << "Bed " << bedNumber << " allocated to patient " << patientId << endl;
            st.push("allocate", patientId, bedNumber, 0);
        } else {
            cout << "Bed " << bedNumber << " is not available!" << endl;
        }
    }

    void releaseBed(UndoStack &st, int bedNumber) {
        if (bedNumber >= 0 && bedNumber < totalBeds && beds[bedNumber] != 0) {
            int pid = beds[bedNumber];
            beds[bedNumber] = 0;
            cout << "Bed " << bedNumber << " released from patient " << pid << endl;
            st.push("release", pid, bedNumber, 0);
        } else {
            cout << "Bed " << bedNumber << " is already free!" << endl;
        }
    }

    void availableCount() {
        int count = 0;
        for (int i = 0; i < totalBeds; i++) {
            if (beds[i] == 0) count++;
        }
        cout << "Available beds: " << count << endl;
    }

    // True when the bed number is valid and currently assigned.
    bool isOccupied(int b)
    {
        if (b < 0 || b >= totalBeds) return false;
        return beds[b] != 0;
    }

    void displayBeds() {
        cout << "--- Bed Status ---" << endl;
        for (int i = 0; i < totalBeds; i++) {
            if (beds[i] == 0) cout << "Bed " << i << ": Free" << endl;
            else cout << "Bed " << i << ": Occupied by patient " << beds[i] << endl;
        }
    }
    void saveToFile()
    {
        ofstream fout("beds.txt");
        for (int i = 0; i < totalBeds; i++)
        {
            fout << i << "|" << beds[i] << endl;
        }
        fout.close();
        cout << "Bed data saved." << endl;
    }

    void loadFromFile()
    {
        ifstream fin("beds.txt");
        if (!fin) return;

        string sb, sp;
        while (getline(fin, sb, '|'))
        {
            getline(fin, sp);
            int b = stoi(sb);
            if (b >= 0 && b < totalBeds)
            {
                beds[b] = stoi(sp);
            }
        }
        fin.close();
        cout << "Bed data loaded." << endl;
    }
};

void undoLastAction(UndoStack &st, BedManager &bm, PriorityQueue &q)
{
    if (st.isEmpty()) {
        cout << "No actions to undo." << endl;
        return;
    }

    ActionRecord r = st.pop();

    if (r.actionType == "allocate") {
        bm.clearBed(r.bedNumber);
        cout << "Undo: Bed " << r.bedNumber << " freed." << endl;
    }
    else if (r.actionType == "release") {
        bm.setBed(r.bedNumber, r.patientId);
        cout << "Undo: Bed " << r.bedNumber << " given back to patient " << r.patientId << endl;
    }
    else if (r.actionType == "severity") {
        q.updateSeverity(r.patientId, r.oldSeverity);
        cout << "Undo: Patient " << r.patientId << " severity back to " << r.oldSeverity << endl;
    }
}

#endif