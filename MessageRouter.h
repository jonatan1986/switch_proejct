#ifndef __MESSAGEROUTER_H__
#define __MESSAGEROUTER_H__

#include<iostream>
#include<cstring>
#include<fcntl.h>
#include "debug.h"
#include "singletone.h"
using std::cout;
using std::endl;



class MessageRouter
{
	static const size_t LENGTH = 1024;
	friend void* Connect(MessageRouter *_messageRouter);
public:
	MessageRouter(int _incSocket,const ConnectionData& _param);
	inline int GetSocket() { return m_incSocket; }
	void Run();
	void Wait();
private:
	void InitMessageRouterConnection();
	void InitFdSet();

	const int m_incSocket;
	int m_OutGoingSocket;
	const string m_destIp;
	const int m_destPort;
	pthread_t m_MessageRouterThread;
	char  m_currentmessage[LENGTH];
	struct sockaddr_in m_sin;

	int m_maxSd;
	fd_set m_master;
	int m_flags;
	bool m_bDebug;
};



MessageRouter::MessageRouter(int _incSocket,const ConnectionData& _param):
m_incSocket(_incSocket),
m_destIp(_param.m_destIp),
m_destPort(_param.m_destPort),
m_flags(-1)
{
	InitMessageRouterConnection();
	InitFdSet();
}

void MessageRouter::InitMessageRouterConnection()
{
	if (singletone<debug>::getinstance()->IsDebug())
	{
		cout<<"MessageRouter m_incsokcet "<<m_incSocket<<endl;
		cout<<"MessageRouter m_destIp "<<m_destIp<<endl;
		cout<<"MessageRouter m_destPort "<<m_destPort<<endl;
	}
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
	else if (singletone<debug>::getinstance()->IsDebug())
	{
		cout<<"connection succeeded"<<endl;
	}
}

void MessageRouter::InitFdSet()
{
	m_maxSd = m_OutGoingSocket;
	FD_ZERO(&m_master);
	FD_SET(m_incSocket,&m_master);
	FD_SET(m_OutGoingSocket,&m_master);
}

void* Connect(MessageRouter *_messageRouter)
{
	int sentBytes,recvLen;
	char buffer[LENGTH],message[LENGTH];
	// MessageRouter *tempMessageRouter = reinterpret_cast<MessageRouter*>(_param);
	memset(message,0,sizeof(message));
	memset(buffer,0,sizeof(buffer));
	if (singletone<debug>::getinstance()->IsDebug())
	{
		cout<<"m_incSocket "<<_messageRouter->m_incSocket<<endl;
		cout<<"m_OutGoingSocket "<<_messageRouter->m_OutGoingSocket<<endl;
	}
	while(true)
	{
		int activity = select((_messageRouter->m_maxSd)+1,&(_messageRouter->m_master),NULL,NULL,NULL);
		if( activity < 0 && errno == EINTR)
		{
			perror("select");
		}
		if( FD_ISSET(_messageRouter->m_OutGoingSocket,&(_messageRouter->m_master)) )
		{
			recvLen =  recv(_messageRouter->m_OutGoingSocket,message,sizeof(message),0);
			if( recvLen > 0)
			{
				sentBytes = send(_messageRouter->m_incSocket,message,sizeof(message),0);
				memset(message,0,sizeof(message));
			}
			else if (recvLen == 0)
			{
				close(_messageRouter->m_incSocket);
				break;
			}
		}
		if( FD_ISSET(_messageRouter->m_incSocket,&(_messageRouter->m_master)) )
		{
				recvLen =  recv(_messageRouter->m_incSocket,buffer,sizeof(buffer),0);
				if( recvLen > 0)
				{
					sentBytes = send(_messageRouter->m_OutGoingSocket,buffer,sizeof(buffer),0);
					memset(buffer,0,sizeof(buffer));
				}
		}
  	FD_CLR(_messageRouter->m_incSocket,&(_messageRouter->m_master));
		FD_CLR(_messageRouter->m_OutGoingSocket,&(_messageRouter->m_master));
		FD_SET(_messageRouter->m_incSocket,&(_messageRouter->m_master));
		FD_SET(_messageRouter->m_OutGoingSocket,&(_messageRouter->m_master));
	}
	return NULL;
}

void MessageRouter::Run()
{
	Connect(this);
}

void MessageRouter::Wait()
{
}
/*==================================================================================================================*/


#endif// __MESSAGEROUTER_H__
