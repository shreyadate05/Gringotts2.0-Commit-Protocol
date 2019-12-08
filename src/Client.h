/*
 * Client.h
 *
 *  Created on: Dec 8, 2019
 *      Author: shreya
 */

#ifndef CLIENT_H_
#define CLIENT_H_

#include "SocketDefs.h"
#include "StringUtilities.h"
#include "Socket.h"
#include <pthread.h>
#include <map>
#include <vector>


class Client
{
	SocketData socketData;
	std::string sCommand;

	public:

		bool setHostname(std::string sHostname);
		bool setPortNum(int iPortNum);
		bool initClient();

		bool readCommand();
		bool parseCommand(std::string sCommand, std::vector<std::string> vCommand);
		bool runClient();

};


#endif /* CLIENT_H_ */
