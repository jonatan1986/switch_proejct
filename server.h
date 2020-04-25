#ifndef __SERVER_H__
#define __SERVER_H__
#include<fstream>
#include<thread>
#include <vector>

using std::vector;
using std::string;
using std::ifstream;

struct ConnectionData
{
	string m_srcIp;
	int m_srcPort;	
	string m_destIp;
	int m_destPort;
};


const size_t LENGTH = 1024;	
const size_t BACK_LOG = 5;



class Server
{
	static const size_t LENGTH = 1024;	
	static const size_t NUM_OF_SOCKETS = 3;
	
	public:
	Server(const string& _configFileName); 
	void Parse(const string& _configFileName);
	~Server();
	private:
	ifstream m_file;
	size_t m_currentLine;
	size_t m_numOfSockets;
	const string m_configFileName;
	size_t m_totalNumOfClients;
	vector<ConnectionData*> m_ConnectionDatas;//[NUM_OF_SOCKETS];
	std::thread *m_threads;
//	SocketData Sockets[numOfSockets];
};

#endif //__SERVER_H__
