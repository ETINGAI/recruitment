#ifndef _ADDR_BOOK_
#define _ADDR_BOOK_

#include "person.h"
#include <unistd.h>
#include <iostream>
#include <string>
#include <list>

using namespace std;


string path = "addrbook.data";

class AddrBook
{
public:
	AddrBook();
	void add(Person &p);
	void print();
	int  del(string& key,string &flag);
	list<Person> search(string& key,string &flag);
	list<Person>::iterator find(string& key,string &flag);
	list<Person> *getBook();
	~AddrBook();

	void saveData();


private:
	list<Person> book;
	void readData();
};


AddrBook::AddrBook()
{
	if((int)access(path.c_str(),0) != -1) //file existed
	{
		//read data
		readData();
	}
}

//read data from file
void AddrBook::readData()
{
	FILE *fp = fopen(path.c_str(),"rb");
	PDATA p;
	if(fp !=NULL)
	{
		int temp = 0;
		while((temp = fread(&p,sizeof(PDATA),1,fp)) != 0)
			book.push_back(Person(p));
		fclose(fp);
	}
}

//save data to file
void AddrBook::saveData()
{
	FILE *fp = fopen(path.c_str(),"wb");
	PDATA p;
	if(fp !=NULL)
	{
		for(list<Person>::iterator it = book.begin();it != book.end();it++)
		{
			p = (*it).getData();
			fwrite(&p,sizeof(PDATA),1,fp);
		}
	}
	fclose(fp);

}

//add data to linklist
void AddrBook::add(Person &p)
{
	book.push_back(p);
}

//print data from linklist
void AddrBook::print()
{
	for(list<Person>::iterator it = book.begin();it != book.end();it++)
	{
		(*it).print();
		
	}
}

list<Person> *AddrBook::getBook()
{
	return &book;
}

AddrBook::~AddrBook()
{
	saveData();
}

list<Person>::iterator AddrBook::find(string& key,string &flag)
{
	list<Person>::iterator it ;
	for(it = book.begin();it != book.end();it++)
	{
		if (flag.find(string("name"),0) != string::npos)
		{
			if(key == it->getName())
			{
				return it;
			}
		}
		else if(flag.find(string("mobile"),0) != string::npos)
		{
			if(key == it->getPhone())
			{
				return it;
			}
		}
		else if(flag.find(string("address"),0) != string::npos)
		{
			if(key == it->getAddr())
			{
				return it;
			}
		}
	}
	return it;
}

//delete data from linklist
int AddrBook::del(string& key,string &flag)
{
	int count = 0;
	
	list<Person>::iterator it ;
	while((it = find(key,flag)) != book.end())
	{
		++count;
		book.erase(it);
	}

	cout<<count <<" records delete"<<endl;
	return count;
}

//search data from linklist according to key
list<Person> AddrBook::search(string& key,string &flag)
{
	int count = 0;
	list<Person>  lp;
	for(list<Person>::iterator it = book.begin();it != book.end();it++)
	{
		if (flag.find(string("name"),0) != string::npos)
		{
			if(key == it->getName())
			{
				lp.push_back(*it);
				it->print();
				count++;
			}
		}
		else if(flag.find(string("mobile"),0) != string::npos)
		{
			if(key == it->getPhone())
			{
				lp.push_back(*it);
				it->print();
				count++;
			}
		}
		else if(flag.find(string("address"),0) != string::npos)
		{
			if(key == it->getAddr())
			{
				lp.push_back(*it);
				it->print();
				count++;
			}
		}
	}
	cout<<count <<" records find"<<endl;
	return lp;

}

#endif
