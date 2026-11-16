#ifndef SEARCH_H
#define SEARCH_H
#include <iostream>
#include <string>
#include "patient.h"
#include "priorityqueue.h"
using namespace std;

void searchById(PriorityQueue &q, int pid)
{
    QueueNode* temp = q.head;

    while (temp != NULL)
    {
        if (temp->data.getid() == pid)
        {
            temp->data.display();
            return;
        }
        temp = temp->next;
    }

    cout << "Patient not found." << endl;
}

void searchByName(PriorityQueue &q, string name)
{
    QueueNode* temp = q.head;
    bool found = false;

    while (temp != NULL)
    {
        if (temp->data.getName() == name)
        {
            temp->data.display();
            found = true;
        }
        temp = temp->next;
    }

    if (found == false)
    {
        cout << "No patient with this name." << endl;
    }
}

void displaySortedByAge(PriorityQueue &q)
{
    int count = 0;
    QueueNode* temp = q.head;

    while (temp != NULL)
    {
        count++;
        temp = temp->next;
    }

    if (count == 0)
    {
        cout << "Queue is empty." << endl;
        return;
    }

    Patient arr[100];
    int i = 0;
    temp = q.head;

    while (temp != NULL)
    {
        arr[i] = temp->data;
        i++;
        temp = temp->next;
    }

    for (int a = 0; a < count - 1; a++)
    {
        for (int b = 0; b < count - 1 - a; b++)
        {
            if (arr[b].getAge() > arr[b + 1].getAge())
            {
                Patient t = arr[b];
                arr[b] = arr[b + 1];
                arr[b + 1] = t;
            }
        }
    }

    for (i = 0; i < count; i++)
    {
        arr[i].display();
    }
}

void binarySearchById(PriorityQueue &q, int pid)
{
    int count = 0;
    QueueNode* temp = q.head;

    while (temp != NULL)
    {
        count++;
        temp = temp->next;
    }

    if (count == 0)
    {
        cout << "Queue is empty." << endl;
        return;
    }

    Patient arr[100];
    int i = 0;
    temp = q.head;

    while (temp != NULL)
    {
        arr[i] = temp->data;
        i++;
        temp = temp->next;
    }

    for (int a = 0; a < count - 1; a++)
    {
        for (int b = 0; b < count - 1 - a; b++)
        {
            if (arr[b].getid() > arr[b + 1].getid())
            {
                Patient t = arr[b];
                arr[b] = arr[b + 1];
                arr[b + 1] = t;
            }
        }
    }

    int low = 0;
    int high = count - 1;

    while (low <= high)
    {
        int mid = (low + high) / 2;

        if (arr[mid].getid() == pid)
        {
            arr[mid].display();
            return;
        }
        else if (arr[mid].getid() < pid)
        {
            low = mid + 1;
        }
        else
        {
            high = mid - 1;
        }
    }

    cout << "Patient not found." << endl;
}

#endif