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
	SocketData server;

	std::string sInputFile;
	int iNumBackendServers;
	int iTolerance;
	std::vector<int> vBackendPorts;
	std::vector<SocketData> vClientSockets;

	static pthread_mutex_t lock;


	public:

		bool setPortNum(int iPortNum);
		bool setInputFile(std::string sInputFile);
		bool parseInputFile();
		bool calculateTolerance();
		bool setupDistributedSystem();

		bool initServer();
		bool runServer();

		static void* connection_handler(void* args);

};




#endif /* COORDINATOR_H_ */
