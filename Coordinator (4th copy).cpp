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
using std::endl;

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

void* Coordinator::connection_handler(void* sockfd)
{

	char buffer[256], tmp_buffer[256];
	string req;
	long *ptr = (long *)sockfd;
	long newsockfd = *ptr;
	string sData = "OK";
	SocketData s;

	if(newsockfd < 0)
	{
		cout << "[ERROR ] Error on accept";
		return NULL;
	}

	s.iSocket = newsockfd;
	Socket::sendData(s, sData);

	while(true)
	{
		sData = " ";
		bzero(buffer,256);
		int rc = recv(newsockfd,buffer,sizeof(buffer),0);	//Read the request message from client
		if(rc < 0)
			cout << "[ERROR ] reading from socket";

		strcpy(buffer, sData.c_str());
		if(strcmp(buffer, "QUIT") == 0)
			break;

		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 10;

		for(int i = 0; i < 3; i++)
		{
			if (!Socket::setSocketOptTimeout(vClientSockets[i], 0, 10))
			{
				cout << "\n[ERROR ] Error occurred while setting socket options";
				break;
			}
		}

		pthread_mutex_lock(&lock);

		for(int i = 0; i < 3; i++)
		{
			int rc = send(vClientSockets[i].iSocket,buffer,strlen(buffer),MSG_NOSIGNAL);
			if (rc < 0)
			{
				cout << "\n[ERROR ] Send failed with rc: <" << rc << "> on server: " << i+1 << endl;
				continue;
			}
			else
				cout << "\n[ INFO ] Sent msg: <" << buffer << "> to server: " << i+1 << endl;
		}

		req = "READY";
		for(int i = 0; i < 3; i++)
		{
			int rc = send(vClientSockets[i].iSocket, &req,sizeof(int),MSG_NOSIGNAL);
			if (rc < 0)
			{
				cout << "[ INFO ] Send failed with rc: <" << rc << "> on server: " << i+1 << endl;
				continue;
			}
			else
				cout << "[ INFO ] Sent 'READY' message: <" << req << "> to server: " << i+1 << endl;
		}

		int flag = 1, cnt = 0;
		for(int i = 0; i < 3; i++)
		{
			int rc = recv(vClientSockets[i].iSocket,&req,sizeof(int),0);
			if (rc <= 0)
			{
				cout << "[ INFO ] Receive failed with rc: <" << rc << "> on server: " << i+1 << endl;
				continue;
			}
			else
			{
				cout << "[ INFO ] Received 'OK' msg: <" << req << "> from server: " << i+1 << endl;
				cnt++;
				cout << "[ INFO ] cnt = " << cnt << endl;
				if(req != "OK")
					flag = 0;
			}
		}

		cout << "[ INFO ] Out cnt = " << cnt << endl;
		if(flag == 1 && cnt >= 2)
		{
			req = "COMMIT";
		}
		else
		{
			req = "ABORT";
			if(cnt < 2)
				req = "FAIL";
		}

		for(int i = 0; i < 3; i++)
		{
			int rc = send(vClientSockets[i].iSocket,&req,sizeof(int),MSG_NOSIGNAL);
			if (rc < 0)
			{
				cout << "\n[ INFO ] Send failed with rc: <" << rc << "> on server: " << i+1 << endl;
				continue;
			}
			else
				cout << "\n[ INFO ] Sent msg: <" << req << "> to server: " << i+1 << endl;
		}

		for(int i = 0; i < 3; i++)
		{
			bzero(tmp_buffer,256);
			int rc = recv(vClientSockets[i].iSocket,tmp_buffer,sizeof(tmp_buffer),0);
			if (rc <= 0)
			{
				cout << "\n[ INFO ] Receive failed with rc: <" << rc << "> on server: " << i+1 << endl;
				continue;
			}
			else
			{
				strncpy(buffer, tmp_buffer, sizeof(tmp_buffer));
				cout << "\n[ INFO ] Received msg: <" << buffer << "> from server: " << i+1 << endl;
			}
		}

		// Two-Phase Commit Protocol Terminates

		pthread_mutex_unlock(&lock);							// Unlocking the Thread between front-end and back-end servers
		rc = send(newsockfd,buffer,strlen(buffer),0);			// Send the response received from beck-end servers to client
		if (rc < 0)
			cout << "\n[ERROR ] writing to socket";

		cout << "\n[ INFO ] Sent msg: <" << buffer << "> to client: " << newsockfd << endl;

	}

	string quit = "OK\nConnection closed by foreign host.";
	strcpy(buffer,quit.c_str());

	int rc = send(newsockfd,buffer,strlen(buffer),0);			// Send Connection Closing Message to Client
	if (rc < 0)
		cout << "\n[ERROR ] writing to socket";
	cout << "\n[ INFO ] Sent msg: <" << buffer << "> to client: " << newsockfd << endl;

	close(newsockfd);
	pthread_exit(NULL);

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
