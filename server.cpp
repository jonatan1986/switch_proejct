#include<iostream>
#include<stdio.h>
#include<cstring>
#include<cstdlib>
#include<assert.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/select.h>
#include <unistd.h>
#include <errno.h>


#include "server.h"
#include "port.h"
using namespace std;

#define debugFile "debug.txt"

void threadfunc(ConnectionData *_connectionData,bool bDebug)
{
	Port  port(*_connectionData,bDebug);
	port.Listen();
}



Server::Server(const string& _configFileName)
{
	ParseDebug();
	Parse(_configFileName);
	m_threads = new thread[m_numOfSockets];
	for(int i = 0; i < m_numOfSockets; ++i)
	{
	   m_threads[i] = thread(threadfunc,ref(m_ConnectionDatas[i]),ref(m_bDebug));
	}
}

void Server::ParseDebug()
{
	ifstream file;
	string line;
	file.open(debugFile);
	assert(file);
	getline(file,line);
	m_bDebug = line == "yes" ? true : false;
}

bool isPortValid(const string& _port)
{
	if (_port.length() > 4)
	{
		return false;
	}
	for(int i = 0; i < _port.length(); ++i)
	{
		if( _port[i] < '0' ||  _port[i] > '9')
		{
			return false;
		}
	}
	return true;
}



int hostname_to_ip(const string& hostname , string& ip,bool bDebug)
{
    struct hostent *he;
    struct in_addr **addr_list;
    int i;

    if ( (he = gethostbyname( hostname.c_str() ) ) == NULL)
    {
        // get the host info
			if (bDebug)
			{
					cout<<"hostname.c_str()"<<hostname.c_str() <<endl;
			}
      herror("gethostbyname");
      return false;
    }

    addr_list = (struct in_addr **) he->h_addr_list;

    for(i = 0; addr_list[i] != NULL; i++)
    {
        //Return the first one;
		ip = inet_ntoa(*addr_list[i]);
     //   strcpy(ip , inet_ntoa(*addr_list[i]) );
        return 0;
    }

    return false;
}


void Server::Parse(const string& _configFileName)
{
	int numOfSockets = 0;
	string pipe = "|",endOfLine;
	size_t startPos = 0,pos;
	string line;
	ConnectionData *connectionData;

	m_file.open(_configFileName.c_str());
	assert(m_file);
	string hostname;
	while( m_file.eof() == false )
	{
		getline(m_file,line);
		if( !line.empty() )
		{
			connectionData = new ConnectionData;
			pos =  line.find_first_of(pipe,startPos);
			connectionData->m_srcIp = line.substr(startPos,pos-startPos);
			startPos = pos + 1;
			pos =  line.find_first_of(pipe,startPos);
			assert(isPortValid(line.substr(startPos,pos-startPos)));
			connectionData->m_srcPort = atoi((line.substr(startPos,pos-startPos)).c_str());
			startPos = pos + 1;
			pos =  line.find_first_of(pipe,startPos);
			if( line.substr(startPos,pos-startPos)[0] == 'w')
			{
				hostname = 	line.substr(startPos,pos-startPos);
				hostname_to_ip(hostname,connectionData->m_destIp,m_bDebug);
			}
			else
			{
				connectionData->m_destIp = line.substr(startPos,pos-startPos);
			}

			startPos = pos + 1;
			pos = line.find_first_of(endOfLine,startPos);
			assert(isPortValid(line.substr(startPos,pos-startPos)));
			connectionData->m_destPort = atoi((line.substr(startPos,pos-startPos)).c_str());
			if (m_bDebug)
			{
				 cout<<"connectionData->m_srcIp "<<connectionData->m_srcIp<<endl;
				 cout<<"connectionData->m_srcPort "<<connectionData->m_srcPort<<endl;
				 cout<<"connectionData->m_destIp "<<connectionData->m_destIp<<endl;
				 cout<<"connectionData->m_destPort "<<connectionData->m_destPort<<endl;
			}
			m_ConnectionDatas.push_back(connectionData);
			++numOfSockets;
		}
		startPos = 0;
	}
	m_file.close();
	m_numOfSockets  = numOfSockets;
}

Server::~Server()
{
	for(int i = 0; i < m_numOfSockets; ++i)
	{
		m_threads[i].join();
	}

	delete[] m_threads;
	while (m_ConnectionDatas.size())
	{
		ConnectionData *connectionData = m_ConnectionDatas.back();
		m_ConnectionDatas.pop_back();
		delete connectionData;
	}
}
