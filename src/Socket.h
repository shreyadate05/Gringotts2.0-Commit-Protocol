/*
 * Socket.h
 *
 *  Created on: Dec 8, 2019
 *      Author: shreya
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#include <string.h>
#include <iostream>
#include <string>
#include "SocketDefs.h"

class Socket
{
	public:

		static bool createSocket(SocketData& s);
		static bool bind(SocketData& s);
		static bool listen(SocketData& s);
		static bool accept(SocketData& oldSock, SocketData& newSock);
		static bool setSocketOptions(SocketData& s);
		static bool getSocketOptions(SocketData& s);
		static bool setSocketOptTimeout(SocketData& s, int iTimeoutInMicroSeconds);

		static bool connect(SocketData& s);
		static bool closeSocket(SocketData& s);

		static bool sendData(SocketData& s, std::string sData);
		static bool recvData(SocketData& s, std::string& sData);

		static bool isValidSocket(int& sockFd);
};




#endif /* SOCKET_H_ */
