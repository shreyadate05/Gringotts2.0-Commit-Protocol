/*
 * Backend.h
 *
 *  Created on: Dec 10, 2019
 *      Author: shreya
 */

#ifndef BACKEND_H_
#define BACKEND_H_

#include "SocketDefs.h"
#include "StringUtilities.h"
#include "Socket.h"
#include <pthread.h>
#include <map>
#include <vector>


class Backend
{
	SocketData socketData;
	std::vector<std::string> vCommand;

	std::map<int, long long> mAccounts;
	std::string sCommand;
	std::string sLocalDecision;
	std::string sTransaction;
	std::vector<std::string> vCommands;
	int iAccountNumber;
	std::string sOutput;

	static pthread_mutex_t lock;
	public:

		bool setHostname(std::string sHostname);
		bool setPortNum(int iPortNum);
		bool initClient();

		bool create();
		bool update();
		bool query();
		bool exit();

		bool readCommand();
		bool parseCommand();
		bool getLocalDecision();
		bool runClient();


		bool getTransaction(SocketData& newSocket);
		bool isReady(SocketData& newSocket);
		bool finalCall(SocketData& newSocket);

		bool commit(SocketData& newSocket);
		bool abort(SocketData& newSocket);
		bool sendOutput();
};





#endif /* BACKEND_H_ */
