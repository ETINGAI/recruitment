#ifndef _SERVER_
#define _SERVER_

#include <pthread.h>


#include "addrbook.h"
#include <cstdlib>
#include <cstdio>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <cstring>
#include <string>
#include <iostream>
using namespace std;


typedef int SOCKET;
#define SOCKET_ERROR 0
#define MAX_PATH 1024

typedef struct _PARAMETER
{
	SOCKET		ClienSocket;
	AddrBook	*pBook;
}PARAMETER;

//Global exclusive semaphores
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

class Server
{
	public:
		Server();
		int start();

	private:
		AddrBook book;

};

//Receive information from client
int RecvInfo(SOCKET CientSocket,char *RecvBuffer)
{
	
	char buffer[MAX_PATH];
	memset(buffer,0x00,MAX_PATH);
	memset(RecvBuffer,0x00,MAX_PATH);
	int	Ret = recv(CientSocket, buffer, MAX_PATH, 0);
	strcpy(RecvBuffer,buffer);
	while(Ret !=0 && Ret != SOCKET_ERROR && strstr(RecvBuffer,"\n") == NULL)
	{
		Ret = recv(CientSocket, buffer, MAX_PATH, 0);
		strcat(RecvBuffer,buffer);
	}
	if(Ret == 0 || Ret == SOCKET_ERROR)
	{
			cout<<"客户端退出!"<<endl;
			return -1;
	}
	return 0;
}

//send informatino to client
int SendInfo(SOCKET CientSocket,const char *SendBuffer)
{
	int Ret = send(CientSocket, SendBuffer, (int)strlen(SendBuffer), 0);
	if ( Ret == SOCKET_ERROR )
	{
		cout<<"Send Info Error::"<<endl;
		return -1;
	}
	return 0;
}

//handle message according command entered
int HandleMsg(const char *cmdline,AddrBook *pBook,SOCKET CientSocket)
{


	if(cmdline == NULL)
		return -1;  //error exit,thread end.
	cout<<"cmdline:"<<cmdline<<"@"<<endl;
	if(strstr(cmdline,"quit") != NULL)
	{
		pBook->saveData();
		return 0;//normal termination,thread exit
	}
	else if(strstr(cmdline,"add") !=NULL)
	{
		char buffer[MAX_PATH];

		SendInfo(CientSocket,"name:");
		memset(buffer,0x00,MAX_PATH);
		RecvInfo(CientSocket,buffer);
		string name = string(buffer).substr(0,strlen(buffer) - 2);

		SendInfo(CientSocket,"mobile:");
		memset(buffer,0x00,MAX_PATH);
		RecvInfo(CientSocket,buffer);
		string mobile =  string(buffer).substr(0,strlen(buffer) - 2);

		SendInfo(CientSocket,"address:");
		memset(buffer,0x00,MAX_PATH);
		RecvInfo(CientSocket,buffer);
		string address =  string(buffer).substr(0,strlen(buffer) - 2);

		Person p(name,mobile,address);
		pBook->add(p);

		SendInfo(CientSocket,"address entry added\r\n");
		return 1;
	}
	else if(strstr(cmdline,"search") !=NULL)
	{
		char buffer[MAX_PATH];
		SendInfo(CientSocket,"by (name|mobile|address): ");
		memset(buffer,0x00,MAX_PATH);
		RecvInfo(CientSocket,buffer);
		string falg(buffer);

		memset(buffer,0x00,MAX_PATH);
		string temp  = falg.substr(0,falg.find('\r',0));
		sprintf(buffer,"%s :",temp.c_str());
		SendInfo(CientSocket,buffer);

		memset(buffer,0x00,MAX_PATH);
		RecvInfo(CientSocket,buffer);
		
		string key(buffer);
		key = key.substr(0,key.length()-2);
		
		list<Person> lp = pBook->search(key,falg);

		string rs;
		for(list<Person>::iterator it = lp.begin();it != lp.end();it++)
		{
			rs += it->toString();
		}
		SendInfo(CientSocket,(char *)rs.c_str());
		return 1;
	}
	else if(strstr(cmdline,"remove") !=NULL)
	{
		char buffer[MAX_PATH];
		SendInfo(CientSocket,"by (name|mobile|address): ");
		memset(buffer,0x00,MAX_PATH);
		RecvInfo(CientSocket,buffer);
		string falg(buffer);
		
		memset(buffer,0x00,MAX_PATH);
		string temp  = falg.substr(0,falg.find('\r',0));
		sprintf(buffer,"%s :",temp.c_str());
		SendInfo(CientSocket,buffer);
		
		memset(buffer,0x00,MAX_PATH);
		RecvInfo(CientSocket,buffer);
		
		string key = string(buffer);
		key = key.substr(0,key.length()-2);
		
		int count = pBook->del(key,falg);
		memset(buffer,0x00,MAX_PATH);
		sprintf(buffer,"\r\n%d address entries deleted\r\n",count);
		SendInfo(CientSocket,buffer);
		return 1;
	}
	else if(strstr(cmdline,"list") !=NULL)
	{
		for(list<Person>::iterator it = pBook->getBook()->begin(); it != pBook->getBook()->end();it++)
		{
			string rs = it->toString();
			SendInfo(CientSocket,rs.c_str());
		}
		return 1;
	}
	else if(strstr(cmdline,"help") !=NULL)
	{
			SendInfo(CientSocket,"Help Info....\r\n");
		return 1;
	}
	else
	{
			SendInfo(CientSocket,"Wrong Command!\r\n");
			return 1;
	}

}

// Server Thread
void* ServerThread(void* lpParameter)
{

	PARAMETER pa = *((PARAMETER *)lpParameter);
	SOCKET CientSocket = pa.ClienSocket;
	AddrBook *pBook      = pa.pBook;

	int Ret = 0;
	char RecvBuffer[MAX_PATH];
	char SendBuffer[MAX_PATH];
	while (true)
	{
		memset(SendBuffer,0x00,sizeof(SendBuffer));
		strcpy(SendBuffer,"ab>");
		Ret = SendInfo(CientSocket,SendBuffer);
		if ( Ret == -1 )
		{
			break;
		}
		Ret = RecvInfo(CientSocket,RecvBuffer);
		if(Ret == -1)
		{
			break;
		}
		cout<<"received a msg is:"<<RecvBuffer<<endl;

	
		pthread_mutex_lock(&mutex);
		Ret = HandleMsg(RecvBuffer,pBook,CientSocket);
		pthread_mutex_unlock(&mutex);

		if(Ret != 1)
		{
			close( CientSocket );
			break;
		}

	}
	return NULL;
}

Server::Server()
{
}

int Server::start()
{
	int sockfd,client_fd;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;

	if((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1)
	{
		fprintf(stderr,"socket create failed..!");
		exit(-1);
	}
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port   = htons(8888);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(server_addr.sin_zero),8);

	if(bind(sockfd,(struct sockaddr*)&server_addr,sizeof(struct sockaddr)) == -1)
	{
		fprintf(stderr,"bind failed\n");
		exit(-1);
	}

	if(listen(sockfd,32) == -1)
	{
		fprintf(stderr,"listen failed\n");
		exit(-1);
	}

	while(1)
	{
		socklen_t size = sizeof(struct sockaddr_in);
		if((client_fd = accept(sockfd,(struct sockaddr*)&client_addr,&size))== -1)
		{
			fprintf(stderr,"accept failed!");
			continue;
		}
		printf("received a connection from %s \n",inet_ntoa(client_addr.sin_addr));

		pthread_t          server_thread;
		pthread_attr_t     server_thread_attr;
		pthread_attr_init(&server_thread_attr);

		PARAMETER pa;
		pa.ClienSocket = client_fd;
		pa.pBook       = &book;

		
		if(pthread_create(&server_thread,&server_thread_attr,ServerThread,(void*)&pa) <0)
		{
			printf("pthread_create failed!\n");
		}

		
		
	
	}
    close(sockfd);

	return 0;
}

#endif
