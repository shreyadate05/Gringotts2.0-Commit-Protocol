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
#include <vector>


class Coordinator
{
	SocketData socketData;
	static pthread_mutex_t lock;

	public:

		bool setHostname(std::string sHostname);
		bool setPortNum(int iPortNum);
		bool initServerData();
		bool startServer();
		bool runServer();
		bool exitServer();

		static void* connection_handler(void* args);
		static bool transact(std::string sRecord);
		static bool withdraw(long long llAccountNumber, long long llAmount);
		static bool deposit(long long llAccountNumber, long long llAmount);
		static std::vector<std::string> StringSplitByDelim(std::string str, char& cDeLim);
		static bool printAccountData();
};




#endif /* COORDINATOR_H_ */
