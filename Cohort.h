/*
 * Cohort.h
 *
 *  Created on: Dec 8, 2019
 *      Author: shreya
 */

#ifndef COHORT_H_
#define COHORT_H_

#include "SocketDefs.h"
#include "Socket.h"
#include "StringUtilities.h"
#include <map>
#include <pthread.h>
#include <vector>

class Cohort
{
	SocketData socketData;
	std::map<int, long long> mAccounts;
	std::string sCommand;
	std::string sTransaction;
	std::vector<std::string> vCommands;
	int iAccountNumber;
	std::string sOutput;

	static pthread_mutex_t lock;

	public:

		bool setPortNum(int iPortNum);
		bool parseCommand();

		bool create();
		bool update();
		bool query();
		bool exit();


		bool initServer();
		bool runServer();

		bool getTransaction(SocketData& newSocket);
		bool isReady(SocketData& newSocket);
		bool finalCall(SocketData& newSocket);
		bool commit(SocketData& newSocket);
		bool abort(SocketData& newSocket);
		bool sendOutput();


		static void connection_handler(void* args);
};



#endif /* COHORT_H_ */
