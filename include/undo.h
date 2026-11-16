#ifndef UNDO_H
#define UNDO_H
#include <iostream>
#include <string>
using namespace std;

struct ActionRecord {
    string actionType;
    int patientId;
    int bedNumber;
    int oldSeverity;
};

class UndoStack {
    ActionRecord stackArr[100];
    int top;

public:
    UndoStack() { top = -1; }

    void push(string type, int pid, int bed, int oldSev) {
        if (top < 99) {
            top++;
            stackArr[top].actionType = type;
            stackArr[top].patientId = pid;
            stackArr[top].bedNumber = bed;
            stackArr[top].oldSeverity = oldSev;
        } else {
            cout << "Undo stack full!" << endl;
        }
    }

    bool isEmpty() { return top == -1; }

    ActionRecord pop() {
        ActionRecord r = stackArr[top];
        top--;
        return r;
    }
};

#endif