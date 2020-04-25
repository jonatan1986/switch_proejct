#ifndef __PORT_H__
#define __PORT_H__

#include<vector>

#include"client.h"
//#include"client.cpp"

using std::vector;
using std::cout;
using std::endl;

class Port
{
	static const size_t BACK_LOG = 5;
	static const size_t LENGTH = 1024;		
	public:
	Port(const ConnectionData& _param);
	
	~Port();
	void Listen();
	private:
	void InitConnection();
	void InitFdSet();
	void Select();
	void Accept();
	void TreatCurrentClients();

	const string m_srcIp;
	const int m_srcPort;
	fd_set m_master;    
    fd_set m_traverser;
	struct sockaddr_in m_sin;
	vector<Client* >m_clients;
	int m_maxSd;
	int m_optval;
    int m_numOfClients;
	int m_activity;
	int m_slen;
	int m_sockfd;
	ConnectionData m_connectionsData;
};
/*============================================================================================================*/
Port::Port(const ConnectionData& _param):
m_connectionsData(_param),
m_srcIp(_param.m_srcIp),
m_srcPort(_param.m_srcPort),
m_optval(1),
m_numOfClients(0),
m_activity(-1),
m_slen(sizeof(m_sin))
{
	InitConnection();
	InitFdSet();
} 
/*============================================================================================================*/
void Port::InitConnection()
{
	m_sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(m_sockfd < 0) /* should bigger/equal than/to 0. */
	{
		perror("socket failed");
	} 
	memset(&m_sin,0,sizeof(m_sin));
	m_sin.sin_family = AF_INET;
	m_sin.sin_port = htons(m_srcPort);
	m_sin.sin_addr.s_addr  = inet_addr((m_srcIp.c_str()));
	if( setsockopt(m_sockfd,SOL_SOCKET,SO_REUSEADDR,&m_optval, sizeof(m_optval)) != 0)
	{
		perror("setsockopt");
	}

	if( bind(m_sockfd,(struct sockaddr*)&m_sin,sizeof(m_sin)) < 0)
	{
		perror("bind");
	}
	if ( listen(m_sockfd,BACK_LOG) == -1)
	{
		perror("listen");
	}
	cout<<" port " <<m_srcPort<<" inits the connetion " <<endl;

}
/*============================================================================================================*/
void Port::InitFdSet()
{
	m_maxSd = m_sockfd;
	FD_ZERO(&m_master); 
	FD_ZERO(&m_traverser); 
	FD_SET(m_sockfd,&m_master);
}
/*============================================================================================================*/
void Port::Accept()
{
	int newSd;	
	if( FD_ISSET(m_sockfd,&m_master) )
	{
			cout<<" Port::Accept() accepted new connection"<<endl;			
			newSd =  accept(m_sockfd,(struct sockaddr *)&m_sin,(socklen_t*)&m_slen);
			if( newSd > 0)
			{
				++m_numOfClients;
				Client* client = new Client(newSd,m_connectionsData);
				m_clients.push_back(client);
				client->Run();
			} 
	}
}
/*============================================================================================================*/
void Port::Select()
{
	m_traverser = m_master;
	m_activity = select(m_maxSd+1,&m_traverser,NULL,NULL,NULL); 
	if( m_activity < 0 && errno == EINTR)
	{
		perror("select");
	} 
}
/*============================================================================================================*/

void Port::Listen()
{
	int recvLen,sentBytes;	
	char buffer[LENGTH];
	while(true)
	{
		Select();
		Accept();
	} 
}


Port::~Port()
{

	//TODO - wait for threads	
	while(!m_clients.empty())
	{
		Client* clientToDelete = m_clients.back();
		m_clients.pop_back();
		delete clientToDelete;
	} 
}
#endif// __PORT_H__











