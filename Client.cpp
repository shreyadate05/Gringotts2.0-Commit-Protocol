/*
 * Client.cpp
 *
 *  Created on: Dec 8, 2019
 *      Author: shreya
 */


/*
 * GringottsClient.cpp
 *
 *  Created on: Oct 1, 2019
 *      Author: shreya
 */

#include "Client.h"

using std::cout;
using std::string;
using std::vector;
using std::map;
using std::cin;

bool Client::setHostname(std::string sHostname)
{
	socketData.sHostname = sHostname;
	return false;
}

bool Client::setPortNum(int iPortNum)
{
	socketData.iPortNum = iPortNum;
	return true;
}

bool Client::initClient()
{
	bool res = true;

	if (!Socket::createSocket(socketData))
	{
		cout << "\n[ERROR ] " << __func__;
		res =  false;
	}

	if (!Socket::connect(socketData))
	{
		cout << "\n[ERROR ] " << __func__;
		res =  false;
	}

	return res;
}

bool Client::runClient()
{
	bool res = true;
	bool bKeepAlive = true;

	while(bKeepAlive)
	{
		string sCommand, sResponse;

		cout << "\n[ INFO ] Enter command: ";
		getline(std::cin, sCommand, '\n');

		// Send command
		res = Socket::sendData(socketData, sCommand.c_str());
		if (!res)
		{
			cout << "\n[ERROR ] Socket error occurred. Exiting client.";
			bKeepAlive = false;
			break;
		}

		// Receive acknowledgement
		res = Socket::recvData(socketData, sResponse);
		if (!res)
		{
			cout << "\n[ERROR ] Socket error occurred. Exiting client.";
			bKeepAlive = false;
			break;
		}
		cout << "\n[ INFO ] " << sResponse.c_str();


	}

	return res;
}
