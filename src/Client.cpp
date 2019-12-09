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

bool Client::readCommand()
{
	bool res = true;

	cout << "\n[ INFO ] Enter command: ";
	cin >> sCommand;

	return res;
}

bool Client::parseCommand()
{
	bool res = true;

	char cDeLim = ' ';
	vCommand = StringUtilities::StringSplitByDelim(sCommand, cDeLim);

	return res;
}

bool Client::runClient()
{
	bool res = true;
	bool bKeepAlive = true;

	while(bKeepAlive && res)
	{
		res = readCommand();
		res = parseCommand();

		if(vCommand.size() == 0)
		{
			bKeepAlive = false;
			cout << "\n[ERROR ]Invalid Command";
		}

		std::transform(vCommand[0].begin(), vCommand[0].end(), vCommand[0].begin(), ::toupper);
		if (vCommand[0] == "QUIT")
		{
			bKeepAlive = false;
			cout << "\n[ INFO ] Exiting client";
		}

		res = Socket::sendData(socketData, sCommand.c_str());
		if (!res)
		{
			cout << "\n[ERROR ] Socket error occurred. Exiting client.";
		}

		string sStatus;
		res = Socket::recvData(socketData, sStatus);
		if (!res)
		{
			cout << "\n[ERROR ] Socket error occurred. Exiting client.";
		}

		cout << "\n[ INFO ]" << sStatus.c_str();
	}

	return res;
}
