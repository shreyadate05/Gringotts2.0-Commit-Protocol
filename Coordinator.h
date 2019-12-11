/*
 * Coordinator.h
 *
 *  Created on: Dec 8, 2019
 *      Author: shreya
 */

#ifndef COORDINATOR_H_
#define COORDINATOR_H_

#include "SocketDefs.h"
#include "Socket.h"
#include <pthread.h>
#include <string.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>

class Coordinator
{
	static SocketData server;
	static std::vector<int> vBackendPorts;
	static std::vector<SocketData> vClientSockets;
	static pthread_mutex_t lock;

	public:

		bool setPortNum(int iPortNum);
		bool setInputFile(std::string sInputFile);
		bool getBackendServerPorts();
		bool calculateTolerance();
		bool setupDistributedSystem();

		bool initServer();
		bool runServer();

		static void* connection_handler(void* args);
		static void* daemon_thread(void* args);

		static bool sendTransaction(std::string sTransaction);

		static bool sendVotingMessage();
		static bool recvVote(std::vector<bool>& vVotes);

		static bool decide(std::vector<bool> vVotes, std::string sDecision);
		static bool sendDecision(std::string sDecision);
		static bool recvServerResponse(std::vector<std::string>& vServerResponses);
};




#endif /* COORDINATOR_H_ */
