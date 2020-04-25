#ifndef __CLIENT_H__
#define __CLIENT_H__

#include<iostream>
#include<cstring>
#include<fcntl.h>
using std::cout;
using std::endl;



class Client
{
	static const size_t LENGTH = 1024;
	friend void* Connect(void *_param);
public:
	Client(int _incSocket,const ConnectionData& _param);
	inline int GetSocket() { return m_incSocket; }
	void Run();
	void Wait();
private:
	void InitClientConnection();
	void InitFdSet();

	const int m_incSocket;
	int m_OutGoingSocket;
	const string m_destIp;
	const int m_destPort;
	pthread_t m_clientThread;
	char  m_currentmessage[LENGTH];
	struct sockaddr_in m_sin;

	int m_maxSd;
	fd_set m_master;
	int m_flags;
};



Client::Client(int _incSocket,const ConnectionData& _param):
m_incSocket(_incSocket),
m_destIp(_param.m_destIp),
m_destPort(_param.m_destPort),
m_flags(-1)
{
	InitClientConnection();
	InitFdSet();
}

void Client::InitClientConnection()
{
	cout<<"client m_incsokcet "<<m_incSocket<<endl;
	cout<<"client m_destIp "<<m_destIp<<endl;
	cout<<"client m_destPort "<<m_destPort<<endl;
	int optval = 1;
	memset(m_currentmessage,0,sizeof(m_currentmessage));
	m_OutGoingSocket = socket(AF_INET,SOCK_STREAM,0);
	if(m_OutGoingSocket  < 0) /* should bigger/equal than/to 0. */
	{
		perror("socket failed");
	}
	memset(&m_sin,0,sizeof(m_sin));
	m_sin.sin_family = AF_INET;
	m_sin.sin_addr.s_addr  = inet_addr(m_destIp.c_str());
	m_sin.sin_port = htons(m_destPort);
	if( setsockopt(m_OutGoingSocket,SOL_SOCKET,SO_REUSEADDR,&optval, sizeof(optval)) != 0)
	{
		perror("setsockopt");
	}

	if ( connect(m_OutGoingSocket,(struct sockaddr*)&(m_sin),sizeof(m_sin)) == -1)
	{
		perror("connect");
	}
	else
	{
		cout<<"connection succeeded"<<endl;
	}
}

void Client::InitFdSet()
{
	m_maxSd = m_OutGoingSocket;
	FD_ZERO(&m_master);
	FD_SET(m_incSocket,&m_master);
	FD_SET(m_OutGoingSocket,&m_master);
}

void* Connect(void *_param)
{
	int sentBytes,recvLen;
	char buffer[LENGTH],message[LENGTH];
	Client *tempClient = reinterpret_cast<Client*>(_param);
	memset(message,0,sizeof(message));
	memset(buffer,0,sizeof(buffer));
	cout<<"m_incSocket "<<tempClient->m_incSocket<<endl;
	cout<<"m_OutGoingSocket "<<tempClient->m_OutGoingSocket<<endl;
	while(true)
	{
		int activity = select((tempClient->m_maxSd)+1,&(tempClient->m_master),NULL,NULL,NULL);
		if( activity < 0 && errno == EINTR)
		{
			perror("select");
		}
		if( FD_ISSET(tempClient->m_OutGoingSocket,&(tempClient->m_master)) )
		{
			recvLen =  recv(tempClient->m_OutGoingSocket,message,sizeof(message),0);
			if( recvLen > 0)
			{
				sentBytes = send(tempClient->m_incSocket,message,sizeof(message),0);
				memset(message,0,sizeof(message));
			}
			else if (recvLen == 0)
			{
				close(tempClient->m_incSocket);
				break;
			}
		}
		if( FD_ISSET(tempClient->m_incSocket,&(tempClient->m_master)) )
		{
				recvLen =  recv(tempClient->m_incSocket,buffer,sizeof(buffer),0);
				if( recvLen > 0)
				{
					sentBytes = send(tempClient->m_OutGoingSocket,buffer,sizeof(buffer),0);
					memset(buffer,0,sizeof(buffer));
				}
		}
  	FD_CLR(tempClient->m_incSocket,&(tempClient->m_master)); 
		FD_CLR(tempClient->m_OutGoingSocket,&(tempClient->m_master));
		FD_SET(tempClient->m_incSocket,&(tempClient->m_master));
		FD_SET(tempClient->m_OutGoingSocket,&(tempClient->m_master));
	}
	return NULL;
}

void Client::Run()
{
	Connect(this);
}

void Client::Wait()
{
}
/*==================================================================================================================*/


#endif// __CLIENT_H__
