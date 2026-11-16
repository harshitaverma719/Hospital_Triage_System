#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H
#include"patient.h"
#include<fstream>

struct QueueNode
{
    Patient    data;
    QueueNode* next;

    QueueNode(Patient p)
    {
        data = p;
        next = NULL;
    }
};
struct PriorityQueue
{
   QueueNode *head;
   QueueNode *treatedHead;
   int size;
   PriorityQueue(){
    head=NULL;
    treatedHead=NULL;
    size=0;
   }

   ~PriorityQueue()
   {
       QueueNode* temp = head;
       while (temp != NULL)
       {
           QueueNode* nxt = temp->next;
           delete temp;
           temp = nxt;
       }
       head = NULL;

       temp = treatedHead;
       while (temp != NULL)
       {
           QueueNode* nxt = temp->next;
           delete temp;
           temp = nxt;
       }
       treatedHead = NULL;
   }

   void addTreated(Patient p)
   {
       p.setStatus("treated");
       QueueNode* n = new QueueNode(p);
       n->next = treatedHead;
       treatedHead = n;
   }

   void displayTreated()
   {
       if (treatedHead == NULL)
       {
           cout << "No treated patients yet." << endl;
           return;
       }
       cout << "\n----Treated Patients----" << endl;
       QueueNode* temp = treatedHead;
       while (temp != NULL)
       {
           temp->data.display();
           temp = temp->next;
       }
   }
    bool higherPriority(Patient a, Patient b)
    {
        if (a.getSeverity() != b.getSeverity())
            return a.getSeverity() < b.getSeverity();

        return a.getArrivalTime() < b.getArrivalTime();
    }

    void enqueue(Patient p)
    {
        QueueNode* newNode = new QueueNode(p);

      
        if (head == NULL || higherPriority(p, head->data))
        {
            newNode->next = head;
            head = newNode;
            size++;
            return;
        }

        
        QueueNode* temp = head;
        while (temp->next != NULL && !higherPriority(p, temp->next->data))
        {
            temp = temp->next;
        }

        newNode->next = temp->next;
        temp->next = newNode;
        size++;
    }
    void dequeue()
    {
        if (head == NULL)
        {
            cout << "Queue is empty!" << endl;
            return;
        }

        cout << "\nPatient treated:" << endl;
        head->data.display();

        QueueNode* temp = head;
        addTreated(temp->data);
        head = head->next;
        delete temp;
        size--;
    }

bool isEmpty()
{
    return head == NULL;
}
   void display(){
        if (head==NULL){
            cout<<"Queue is empty"<< endl;
            return;

        }
        cout<<"\n----Patients Priority Queue----"<<endl;
        QueueNode *temp=head;
        int i=1;
        while(temp!=NULL){
            cout<<"Patient"<<i<<":"<<endl;
            temp->data.display();
            temp=temp->next;
            i++;
        }
    }
    void peek()
{
    if (head == NULL)
    {
        cout << "Queue is empty!" << endl;
        return;
    }
    cout << "\nPatient with highest priority:" << endl;
    head->data.display();
}
bool contains(int pid)
    {
        QueueNode *temp = head;
        while (temp != NULL)
        {
            if (temp->data.getid() == pid)
                return true;
            temp = temp->next;
        }
        return false;
    }
    // Returns the current severity of a patient, or 0 if not present.
    // Used before a severity change so the old value can be pushed
    // onto the undo stack.
    int getSeverityOf(int pid)
    {
        QueueNode *temp = head;
        while (temp != NULL)
        {
            if (temp->data.getid() == pid)
                return temp->data.getSeverity();
            temp = temp->next;
        }
        return 0;
    }

    void updateSeverity(int pid, int newSev)
    {
        QueueNode* temp = head;
        QueueNode* prev = NULL;

        
        while (temp != NULL && temp->data.getid() != pid)
        {
            prev = temp;
            temp = temp->next;
        }

        if (temp == NULL)
        {
            cout << "Patient not found!" << endl;
            return;
        }

        Patient p = temp->data;          
        int oldSev = p.getSeverity();

        
        if (prev == NULL)
            head = temp->next;           
        else
            prev->next = temp->next;     
        delete temp;
        size--;

        p.setseverity(newSev);           
        enqueue(p);                      

        cout << "\nSeverity  updated from " << oldSev << " to " << newSev<< "  Queue has been re-ordered " << endl;
    }

    void saveToFile()
    {
        ofstream fout("patients.txt");

        QueueNode* temp = head;
        while (temp != NULL)
        {
            fout << temp->data.getid() << "|"
                 << temp->data.getName() << "|"
                 << temp->data.getAge() << "|"
                 << temp->data.getSymptoms() << "|"
                 << temp->data.getSeverity() << "|"
                 << temp->data.getArrivalTimeStr() << "|"
                 << temp->data.getvisitType() << "|"
                 << temp->data.getStatus() << endl;

            temp = temp->next;
        }

        temp = treatedHead;
        while (temp != NULL)
        {
            fout << temp->data.getid() << "|"
                 << temp->data.getName() << "|"
                 << temp->data.getAge() << "|"
                 << temp->data.getSymptoms() << "|"
                 << temp->data.getSeverity() << "|"
                 << temp->data.getArrivalTimeStr() << "|"
                 << temp->data.getvisitType() << "|"
                 << temp->data.getStatus() << endl;

            temp = temp->next;
        }

        fout.close();
        cout << "Data saved to patients.txt" << endl;
    }

    void loadFromFile()
    {
        ifstream fin("patients.txt");

        if (!fin)
            return;

        string sid, name, sage, symptoms, ssev, atime, vtype, stat;

        while (getline(fin, sid, '|'))
        {
            getline(fin, name, '|');
            getline(fin, sage, '|');
            getline(fin, symptoms, '|');
            getline(fin, ssev, '|');
            getline(fin, atime, '|');
            getline(fin, vtype, '|');
            getline(fin, stat);

            Patient p(stoi(sid), name, stoi(sage), symptoms, stoi(ssev), atime);
            p.setvisitType(vtype);

            if (stat == "treated")
                addTreated(p);
            else
                enqueue(p);
        }

        fin.close();
        cout << "Previous patient data loaded." << endl;
    }
};

#endif