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

SocketData Coordinator::server;
pthread_mutex_t Coordinator::lock;
vector<int> Coordinator::vBackendPorts;
vector<SocketData> Coordinator::vClientSockets;

bool Coordinator::sendTransaction(string sTransaction)
{
	bool res = true;

	for (int i = 0; i < vClientSockets.size(); i++)
	{
		res = Socket::sendData(vClientSockets[i], sTransaction);
		if (!res)
		{
			cout << "\n[ERROR ] Failed to send transaction to server on port number " << vBackendPorts[i];
			res = false;
			break;
		}
		cout << "\n[ INFO ] Sent transaction to server on port number " << vBackendPorts[i];
	}

	return res;
}

bool Coordinator::sendVotingMessage()
{
	bool res = true;

	for (int i = 0; i < vClientSockets.size(); i++)
	{
		res = Socket::sendData(vClientSockets[i], "READY?");
		if (!res)
		{
			cout << "\n[ERROR ] Failed to send voting message to server on port number " << vBackendPorts[i];
			res &= false;
			break;
		}
		cout << "\n[ INFO ] Sent voting message to server on port number " << vBackendPorts[i];
	}

	return res;
}

bool Coordinator::recvVote(std::vector<bool>& vVotes)
{
	bool res = true;

	for (int i = 0; i < vClientSockets.size(); i++)
	{
		string sVote;
		res = Socket::recvData(vClientSockets[i], sVote);
		if (!res)
		{
			cout << "\n[ERROR ] Failed to receive vote of server on port number " << vBackendPorts[i];
			res &= false;
			break;
		}
		cout << "\n[ INFO ] Received vote of server on port number " << vBackendPorts[i];


		if (sVote == "OK")
			vVotes.push_back(true);
		else
			vVotes.push_back(false);
	}

	return res;
}

bool Coordinator::decide(std::vector<bool> vVotes, string sDecision)
{
	bool res = true;
	int iCount = 0;

	for (int i = 0; i < vVotes.size(); i++)
	{
		if (!vVotes[i])
			iCount++;
	}

	sDecision = (iCount > 1) ? "ABORT" : "COMMIT";

	return res;
}

bool Coordinator::sendDecision(string sDecision)
{
	bool res = true;

	for (int i = 0; i < vClientSockets.size(); i++)
	{
		res = Socket::sendData(vClientSockets[i], sDecision);
		if (!res)
		{
			cout << "\n[ERROR ] Failed to send decision to server on port number " << vBackendPorts[i];
			res = false;
			break;
		}
		cout << "\n[ INFO ] Sent decision to server on port number " << vBackendPorts[i];
	}

	return res;
}

bool Coordinator::recvServerResponse(vector<string>& vServerResponses)
{
	bool res = true;

	for (int i = 0; i < vClientSockets.size(); i++)
	{
		string sAck;
		res = Socket::recvData(vClientSockets[i], sAck);
		if (!res)
		{
			cout << "\n[ERROR ] Failed to receive response from server on port number " << vBackendPorts[i];
			res = false;
			break;
		}
		vServerResponses.push_back(sAck);
		cout << "\n[ INFO ] Received response of server on port number " << vBackendPorts[i];

	}

	return res;
}

void* Coordinator::connection_handler(void* args)
{
	SocketData* newConn = static_cast<SocketData*>(args);
	bool res = true;

	cout << "\n[ INFO ]In thread handler for socket " << newConn->iSocket;

	while(true)
	{
//		for (int i = 0; i < vClientSockets.size(); i++)
//		{
//			res = Socket::setSocketOptTimeout(vClientSockets[i], 0, 10);
//		}

		vector<bool> vVotes;
		vector<string> vServerResponses;
		string sDecision = "";
		string sClientResponse;
		string sTransaction;

		pthread_mutex_lock(&lock);
		Socket::recvData(*newConn, sTransaction);
		cout << "[ INFO ] Transaction is: " << sTransaction.c_str();
		res &= sendTransaction(sTransaction);
		sleep(1);
		break;
		res &= sendVotingMessage();
		sleep(1);
		res &= recvVote(vVotes);
		sleep(1);
		res &= decide(vVotes, sDecision);
		sleep(1);
		res &= sendDecision(sDecision);
		sleep(1);
		res &= recvServerResponse(vServerResponses);
		sleep(1);

		// send the response to client
		string sData = vServerResponses[0];
		res &= Socket::sendData(server, sData);

		pthread_mutex_unlock(&lock);
	}
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

		if (!Socket::createSocket(vClientSockets[i]))
		{
			cout << "\n[ERROR ] " << __func__;
			res =  false;
			break;
		}

		if (!Socket::setSocketOptTimeout(vClientSockets[i], 1, 0))
		{
			cout << "\n[ERROR ] " << __func__;
			res =  false;
			break;
		}

		if (!Socket::connect(vClientSockets[i]))
		{
			cout << "\n[ERROR ] " << __func__;
			res =  false;
			break;
		}


		cout << "\n[ INFO ] Connected to server on port " << vClientSockets[i].iPortNum;
	}

	return res;
}

bool Coordinator::setPortNum(int iPortNum)
{
	server.iPortNum = iPortNum;
	return true;
}

bool Coordinator::getBackendServerPorts()
{
	bool res = true;
	int iPort = 0;

	cout << "\n[ INFO ] Please enter port numbers for the three backend servers.";

	cout << "\n[ INFO ] Port number for Server 1: ";
	cin >> iPort;
	vBackendPorts.push_back(iPort);

	cout << "\n[ INFO ] Port number for Server 2: ";
	cin >> iPort;
	vBackendPorts.push_back(iPort);

	cout << "\n[ INFO ] Port number for Server 3: ";
	cin >> iPort;
	vBackendPorts.push_back(iPort);

	return res;
}

bool Coordinator::initServer()
{
	bool res = true;

	if(!getBackendServerPorts())
	{
		cout << "\n[ERROR ] Error occurred while getting Backend server Port numbers.";
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

	if (!setupDistributedSystem())
	{
		cout << "\n[ERROR ] Failed to set up distributed system with backend servers.";
		return -1;
	}


	return res;
}

bool Coordinator::runServer()
{
	bool res = true;
	int i = 0;

//	pthread_t daemonThreadId;
//	if(pthread_create(&daemonThreadId, NULL, Coordinator::daemon_thread, NULL))
//	{
//		cout << "\n[ERROR ] Error while creating daemon thread." << __func__;
//		return false;
//	}

	vector<pthread_t> vThreadIds;

	while(true)
	{
		SocketData* newSocket = new SocketData();
		newSocket->iPortNum = server.iPortNum;
		newSocket->sHostname = server.sHostname;
		pthread_t threadId;
		string sClientData;

		Socket::accept(server, *newSocket);
		if (i == 0)
		{
			if(pthread_create(&threadId, NULL, Coordinator::connection_handler, (void*)(newSocket)))
			{
				cout << "\n[ERROR ] " << __func__;
				return false;
			}
		}
		if(pthread_create(&threadId, NULL, Coordinator::connection_handler, (void*)(newSocket)))
		{
			cout << "\n[ERROR ] " << __func__;
			return false;
		}
		sleep(0.5);
		vThreadIds.push_back(threadId);
		i++;
	}

	return res;
}
