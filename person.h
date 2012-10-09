#ifndef _PERSON_
#define _PERSON_

#include <cstdio>
#include <cstring>
#include <string>
#include <iostream>
using namespace std;

//restore a record using struct
typedef struct _PERSON_DATA
{
	char name[32];
	char phone[32];
	char addr[32];
}PDATA;

class Person
{
public:
	Person(string &name,string &phone,string &addr);
	Person(PDATA &pdata);
	PDATA getData();
	void print();

	string getName();
	string getAddr();
	string getPhone();
	
	string toString();

private:
	string name;
	string phone;
	string addr;
};

Person::Person(string &name,string &phone,string &addr)
{
	this->name = name;
	this->phone = phone;
	this->addr = addr;
}

Person::Person(PDATA &pdata)
{
	this->name = string(pdata.name);
	this->phone = string(pdata.phone);
	this->addr = string(pdata.addr);
}

PDATA Person::getData()
{
		PDATA p;
		memset(&p,0,sizeof(PDATA));
		string tt = name.substr(0,31);
		strcpy(p.name,tt.c_str());
		
	    tt = phone.substr(0,31);
		strcpy(p.phone,tt.c_str());
		
	    tt = addr.substr(0,31);
		strcpy(p.addr,tt.c_str());
		
		return p;
}

void Person::print()
{
	cout<<"Name :"<<name<<" mobile :"<<phone<<" Addr :"<<addr<<endl;
}

string Person::getName()
{
	return name;
}

string Person::getAddr()
{

	return addr;
}

string Person::getPhone()
{
	return phone;
}

string Person::toString()
{
	char buffer[1024] ={0};
	sprintf(buffer,"Name : %s mobile : %s Addr : %s\r\n",name.c_str(),phone.c_str(),addr.c_str());
	return string(buffer);
}
#endif
