#ifndef PATIENT_H
#define PATIENT_H
#include<iostream>
#include<string>
using namespace std;
class Patient 
{  private:
    int id ;
    string name;
    int age;
    int severity;
    string symptoms;
    string arrival_time;
    string visitType;
    string status;

 public :
    Patient(){
        id=0;
        name="";
        age=0;
        severity=0;
        symptoms="";
        arrival_time ="";
        visitType="Routine";
        status="waiting";
    }
    Patient (int p_id,string n,int p_age ,string sy, int se,string  a_t){
        id = p_id;
        name= n;
        age = p_age;
        symptoms = sy;
        severity = se;
        arrival_time = a_t;
        visitType="Routine";
        status="waiting";
    }
    
     int getSeverity()
    {
        return severity;
    }
    int getid(){
        return id;
    }
    void setseverity(int s)
    {
        severity =s;
    }
    string getName()
    {
        return name;
    }

    int getAge()
    {
        return age;
    }

    string getSymptoms()
    {
        return symptoms;
    }

    string getArrivalTimeStr()
    {
        return arrival_time;
    }
    string getvisitType()
    {
        return visitType;
    }
    void setvisitType(string v){
        visitType=v;
    }
    string getStatus()
    {
        return status;
    }
    void setStatus(string s){
        status=s;
    }
     int getArrivalTime()
{ 
    if(arrival_time.length()!=6)
    return 0;

    int h = stoi(arrival_time.substr(0, 2));
    int m = stoi(arrival_time.substr(2, 2));
    int s = stoi(arrival_time.substr(4, 2));
      return h * 3600 + m * 60 + s;
}
    void display () {
        if(severity==1){
        cout<<"***EMERGENCY***"<<endl;}

        cout<<"-----Patient Detail-----"<< endl;
        cout<<"ID: "<< id<< endl;
        cout<<"Name of the patient: "<<name<<endl;
        cout<<"Age : "<<age << endl;
        cout<<"Symptoms  of the patient: "<<symptoms<<endl;
        cout<<"Severity : "<<severity <<endl;
        cout<<"Arrival time :"<<arrival_time<<endl;
        cout<<"Visit Type: "<<visitType<<endl;
        cout<<"Status: "<<status<<endl;
}
};

#endif