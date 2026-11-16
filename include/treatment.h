#ifndef TREATMENT_H
#define TREATMENT_H
#include "patient.h"
#include<fstream>

struct TreatmentNode
{
    int patientId;
    string date;
    string vitals;
    string medicine;
    string notes;
    TreatmentNode* next;
};

class TreatmentHistory
{
    TreatmentNode* head;

public:
    TreatmentHistory()
    {
        head = NULL;
    }

    void addTreatment(int pid, string d, string v, string m, string n)
    {
        TreatmentNode* newNode = new TreatmentNode;
        newNode->patientId = pid;
        newNode->date = d;
        newNode->vitals = v;
        newNode->medicine = m;
        newNode->notes = n;
        newNode->next = NULL;

        if (head == NULL)
        {
            head = newNode;
            return;
        }

        TreatmentNode* temp = head;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = newNode;
    }
    void deleteTreatment(int pid, string d)
    {
        TreatmentNode* temp = head;
        TreatmentNode* prev = NULL;

       
        while (temp != NULL)
        {
            if (temp->patientId == pid && temp->date == d)
                break;

            prev = temp;
            temp = temp->next;
        }

        if (temp == NULL)
        {
            cout << "Record not found." << endl;
            return;
        }

        if (prev == NULL)  
        head=temp->next;            
        else                         
            prev->next = temp->next;

        delete temp;
        cout << "Treatment record deleted." << endl;
    }
    // Number of treatment records stored for one patient.
    int countFor(int pid)
    {
        int n = 0;
        TreatmentNode* temp = head;
        while (temp != NULL)
        {
            if (temp->patientId == pid) n++;
            temp = temp->next;
        }
        return n;
    }

    void displayHistory(int pid)
    {
        TreatmentNode* temp = head;
        bool found = false;

        cout << "\n----Treatment History for Patient " << pid << "----" << endl;

        while (temp != NULL)
        {
            if (temp->patientId == pid)
            {
                cout << "Date: " << temp->date << endl;
                cout << "Vitals: " << temp->vitals << endl;
                cout << "Medicine: " << temp->medicine << endl;
                cout << "Notes: " << temp->notes << endl;
                cout << "-------------------" << endl;
                found = true;
            }
            temp = temp->next;
        }

        if (found == false)
            cout << "No treatment records found." << endl;
    }
    void saveToFile()
    {
        ofstream fout("treatments.txt");

        TreatmentNode* temp = head;
        while (temp != NULL)
        {
            fout << temp->patientId << "|"
                 << temp->date << "|"
                 << temp->vitals << "|"
                 << temp->medicine << "|"
                 << temp->notes << endl;

            temp = temp->next;
        }

        fout.close();
        cout << "Treatment data saved." << endl;
    }

    void loadFromFile()
    {
        ifstream fin("treatments.txt");

        if (!fin)                      
            return;

        string spid, d, v, m, n;

        while (getline(fin, spid, '|'))
        {
            getline(fin, d, '|');
            getline(fin, v, '|');
            getline(fin, m, '|');
            getline(fin, n);           

            addTreatment(stoi(spid), d, v, m, n);
        }

        fin.close();
        cout << "Previous treatment data loaded." << endl;
    }
};

#endif