   #ifndef VALIDATION_H
#define VALIDATION_H
#include <iostream>
#include <string>
using namespace std;

int readInt(string prompt, int minVal, int maxVal)
{
    int value;

    while (true)
    {
        cout << prompt;
        cin >> value;

        if (cin.fail())
        {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid input. Enter a number." << endl;
        }
        else if (value < minVal || value > maxVal)
        {
            cin.ignore(1000, '\n');
            cout << "Enter between " << minVal << " and " << maxVal << "." << endl;
        }
        else
        {
            cin.ignore(1000, '\n');
            return value;
        }
    }
}

string readName(string prompt)
{
    string name;

    while (true)
    {
        cout << prompt;
        getline(cin, name);

        if (name.length() == 0)
        {
            cout << "Name cannot be empty." << endl;
            continue;
        }

        bool hasLetter = false;
        for (int i = 0; i < name.length(); i++)
        {
            if ((name[i] >= 'a' && name[i] <= 'z') || (name[i] >= 'A' && name[i] <= 'Z'))
            {
                hasLetter = true;
            }
        }

        if (hasLetter == false)
        {
            cout << "Name must contain letters." << endl;
            continue;
        }

        return name;
    }
}

string readTime(string prompt)
{
    string t;

    while (true)
    {
        cout << prompt;
        getline(cin, t);

        if (t.length() != 6)
        {
            cout << "Enter 6 digits, format HHMMSS." << endl;
            continue;
        }

        bool allDigits = true;
        for (int i = 0; i < 6; i++)
        {
            if (t[i] < '0' || t[i] > '9')
            {
                allDigits = false;
            }
        }

        if (allDigits == false)
        {
            cout << "Only digits allowed." << endl;
            continue;
        }

        int hh = (t[0] - '0') * 10 + (t[1] - '0');
        int mm = (t[2] - '0') * 10 + (t[3] - '0');
        int ss = (t[4] - '0') * 10 + (t[5] - '0');

        if (hh > 23 || mm > 59 || ss > 59)
        {
            cout << "Invalid time value." << endl;
            continue;
        }

        return t;
    }
}
string readLine(string prompt)
{
    string s;

    while (true)
    {
        cout << prompt;
        getline(cin, s);

        if (s.length() == 0)
        {
            cout << "This field cannot be empty." << endl;
            continue;
        }

        return s;
    }
}

#endif