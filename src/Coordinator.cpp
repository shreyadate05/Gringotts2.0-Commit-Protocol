/*
 * Coordinator.cpp
 *
 *  Created on: Dec 8, 2019
 *      Author: shreya
 */

#include "Coordinator.h"

using std::cout;
using std::string;
using std::vector;
using std::map;
using std::cin;


void* Coordinator::connection_handler(void* args)
{
	string* sTransaction = static_cast<string*>(args);
	pthread_mutex_lock(&lock);



	pthread_mutex_unlock(&lock);
	return NULL;
}

bool Coordinator::setupDistributedSystem()
{
	bool res = true;
	vClientSockets.clear();

	for (int i = 0; i < vBackendPorts.size(); i++)
	{
		SocketData socket;
		socket.iPortNum  = vBackendPorts[i];
		socket.sHostname = "localhost";
		vClientSockets.push_back(socket);

		if (!Socket::createSocket(vClientSockets[vClientSockets.size()-1]))
		{
			cout << "\n[ERROR ] " << __func__;
			res =  false;
		}

		if (!Socket::connect(vClientSockets[vClientSockets.size()-1]))
		{
			cout << "\n[ERROR ] " << __func__;
			res =  false;
		}
	}

	return res;
}

bool Coordinator::setPortNum(int iPortNum)
{
	server.iPortNum = iPortNum;
	return true;
}

bool Coordinator::setInputFile(string sInputFile)
{
	bool res = true;
	this->sInputFile = sInputFile;
	return res;
}

bool Coordinator::parseInputFile()
{
	bool res = true;

	/*
	 * Input file contains port numbers of backend servers.
	 * Format is:
	 *
	 * server1 4199
	 * server2 4000
	 * server3 5000
	 *
	 *Pushing port numbers in a vector
	 *
	 */

	std::ifstream file(sInputFile.c_str());
	std::string   line;
	try
	{
		while(std::getline(file, line))
		{
			std::vector<string> lineData;
			std::stringstream   lineStream(line.c_str());
			string value;

			// Read single row of space separated vector
			while(lineStream >> value)
			{
				lineData.push_back(value);
			}

			if (lineData.size() != 2)
				throw("ERROR");

			vBackendPorts.push_back(stoi(lineData[1]));
		}
		iNumBackendServers = vBackendPorts.size();
	}
	catch(...)
	{
		std::cout << "\n[ERROR ] Exception thrown while creating initial client records in " << __func__;
		res = false;
	}


	return res;
}

bool Coordinator::calculateTolerance()
{
	bool res = true;

	iTolerance = iNumBackendServers/3;
	cout << "\n[ INFO ] Total number of servers(including coordinator): " << iNumBackendServers + 1;
	cout << "\n[ INFO ] Number of faulty servers system can tolerate: " << iTolerance;

	return res;
}

bool Coordinator::initServer()
{
	bool res = true;

	if(!parseInputFile())
	{
		cout << "\n[ERROR ] Invalid command line arguments.";
		return -1;
	}

	res = calculateTolerance();
	if (!setupDistributedSystem())
	{
		cout << "\n[ERROR ] Failed to set up distributed system.";
		return -1;
	}

	if ( ! Socket::createSocket(server) )
	{
		cout << "\n[ERROR ] " << __func__;
		return false;
	}

	if ( ! Socket::bind(server) )
	{
		cout << "\n[ERROR ] " << __func__;
		return false;
	}

	if ( ! Socket::listen(server) )
	{
		cout << "\n[ERROR ] " << __func__;
		return false;
	}


	return res;
}

bool Coordinator::runServer()
{
	bool res = true;
	int i = 0;
	while(true)
	{
		SocketData* newSocket = new SocketData();
		newSocket->iPortNum = server.iPortNum;
		newSocket->sHostname = server.sHostname;
		pthread_t threadId;
		string sClientData;

		Socket::accept(server, *newSocket);
		Socket::recvData(*newSocket, sClientData);

		if(pthread_create(&threadId, NULL, Coordinator::connection_handler, (void*)(&sClientData)))
		{
			cout << "\n[ERROR ] " << __func__;
			return false;
		}

		i++;
	}
	return res;
}
