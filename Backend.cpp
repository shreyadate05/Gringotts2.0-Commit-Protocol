/*
 * Backend.cpp
 *
 *  Created on: Dec 10, 2019
 *      Author: shreya
 */

#include "Backend.h"

using std::cout;
using std::string;
using std::vector;
using std::map;
using std::cin;

bool Backend::setHostname(std::string sHostname)
{
	socketData.sHostname = sHostname;
	return false;
}

bool Backend::setPortNum(int iPortNum)
{
	socketData.iPortNum = iPortNum;
	return true;
}

bool Backend::parseCommand()
{
	bool res = true;
	string sReady;

	char cDeLim = ' ';
	vCommands.clear();
	vCommands = StringUtilities::StringSplitByDelim(sCommand, cDeLim);
	cout << "\n[ INFO ] Global Commit for transaction " << sCommand;
	if (vCommands.size() == 0)
	{
		cout << "\n[ERROR ] Invalid command " << sCommand.c_str();
		return false;
	}

	std::transform(vCommands[0].begin(), vCommands[0].end(), vCommands[0].begin(), ::toupper);
	for (int i = 0; i < vCommands.size(); i++)
	{
		//cout << "\n[ INFO ] " << vCommands[i];
	}

	if(strcmp(vCommands[0].c_str(), "CREATE") == 0)
		res = create();

	else if (strcmp(vCommands[0].c_str(), "UPDATE") == 0)
		res = update();

	else if (strcmp(vCommands[0].c_str(), "QUERY") == 0)
		res = query();

	else if (strcmp(vCommands[0].c_str(), "QUIT") == 0)
			res = exit();

	res &= sendOutput();

	return res;
}

bool Backend::sendOutput()
{
	bool res = true;

	res = Socket::sendData(socketData, sOutput.c_str());
	if (!res)
	{
		cout << "\n[ERROR ] Socket error occurred. Exiting client.";
	}

	return res;
}

bool Backend::create()
{
	bool res = true;

	if (vCommands.size() != 2)
	{
		cout << "\n[ERROR ] Invalid command " << sCommand.c_str();
		return false;
	}

	iAccountNumber++;
	cout << "\n[ INFO ] Account number for new account is: " << iAccountNumber;
	sOutput = "";

	int deposit = std::stoi(vCommands[1]);
	if (deposit < 0)
	{
		sOutput = "ERR";
	}

	try
	{
		mAccounts.insert(std::make_pair(iAccountNumber, deposit));
		sOutput = "OK " + std::to_string(iAccountNumber);
	}
	catch(...)
	{
		sOutput = "ERR";
	}

	return res;
}

bool Backend::update()
{
	bool res = true;

	if (vCommands.size() != 3)
	{
		cout << "\n[ERROR ] Invalid command " << sCommand.c_str();
		return false;
	}

	int iAccountId = stoi(vCommands[1]);
	long long llAmount = stoll(vCommands[2]);

	if (mAccounts.count(iAccountId) <= 0)
	{
		cout << "\n[ERROR ] Invalid Account ID provided";
		return false;
	}

	mAccounts[iAccountId] = llAmount;
	sOutput = "OK " + std::to_string(llAmount);

	return res;
}

bool Backend::query()
{
	bool res = true;

	if (vCommands.size() != 2)
	{
		cout << "\n[ERROR ] Invalid command " << sCommand.c_str();
		return false;
	}

	int iAccountId = stoi(vCommands[1]);
	if (mAccounts.count(iAccountId) <= 0)
	{
		cout << "\n[ERROR ] Invalid Account ID provided";
		return false;
	}

	sOutput = "OK " + std::to_string(mAccounts[iAccountId]);

	return res;
}

bool Backend::exit()
{
	bool res = true;

	res = Socket::closeSocket(socketData);

	return res;
}

bool Backend::getTransaction(SocketData& newSocket)
{
	bool res = true;
	cout << "\n[DEBUG ] Entry to " << __func__;

	sTransaction = "";
	res = Socket::recvData(newSocket, sTransaction);
	if (!res)
	{
		cout << "\n[ERROR ] Socket error occurred while receiving transaction.";
		res = false;
	}
	else
	{
		cout << "\n[ INFO ] Received transaction \"" << sTransaction.c_str() << "\" from Coordinator";
		cout << "\n[DEBUG ] Exit from " << __func__;
	}

	return res;
}

bool Backend::isReady(SocketData& newSocket)
{
	bool res = true;
	string sReady;
	cout << "\n[DEBUG ] Entry to " << __func__;

	res = Socket::recvData(newSocket, sReady);
	if (!res)
	{
		cout << "\n[ERROR ] Socket error occurred while receiving voting message from server.";
	}
	cout << "\n[ INFO ] Received voting message \"" << sReady.c_str() << "\" from Coordinator";

	res = Socket::sendData(socketData, "OK");
	if (!res)
	{
		cout << "\n[ERROR ] Socket error occurred. Exiting client.";
	}
	cout << "\n[ INFO ] Sent \'" << sReady << "\' back to Coordinator";

	cout << "\n[DEBUG ] Exit from " << __func__;
	return res;
}

bool Backend::finalCall(SocketData& newSocket)
{
	bool res = true;
	string sDecision;

	res = Socket::recvData(newSocket, sDecision);
	if (!res)
	{
		cout << "\n[ERROR ] Socket error occurred. Exiting client.";
	}
	std::transform(sDecision.begin(), sDecision.end(), sDecision.begin(), ::toupper);
	cout << "\n[ INFO ] Received " << sDecision.c_str() << " from Coordinator";

	res = (sDecision == "COMMIT") ? commit(newSocket) : abort(newSocket);

	return res;
}

bool Backend::commit(SocketData& newSocket)
{
	bool res = true;

	res = parseCommand();

	return res;
}

bool Backend::abort(SocketData& newSocket)
{
	bool res = true;

	cout << "[ INFO ] Aborting transaction. ";

	return res;
}

bool Backend::getLocalDecision()
{
	bool res = true;

	int iRand = (rand() % 9) + 1;
	if (iRand == 3 || iRand == 6 || iRand == 9)
	{
		sLocalDecision = "ABORT";
	}
	else
	{
		sLocalDecision = "COMMIT";
	}

	return res;
}

bool Backend::initClient()
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

bool Backend::runClient()
{
	bool res = true;
	bool bKeepAlive = true;

	string sReady;
	int i = 0;
	iAccountNumber = 100;

	while(true)
	{
		SocketData newSocket = socketData;
		string sClientData, sDecision, sTransaction;

		res = Socket::recvData(newSocket, sTransaction);
		cout << "\n[ INFO ] Received transaction \"" << sTransaction.c_str() << "\" from Coordinator";
		sCommand = sTransaction;

		res = Socket::recvData(newSocket, sReady);
		cout << "\n[ INFO ] Received voting message \"" << sReady.c_str() << "\" from Coordinator";

		res = getLocalDecision();
		res = Socket::sendData(socketData, sLocalDecision);
		cout << "\n[ INFO ] Sent local decision to "<< sLocalDecision << " back to Coordinator";


		res = Socket::recvData(newSocket, sDecision);
		std::transform(sDecision.begin(), sDecision.end(), sDecision.begin(), ::toupper);
		cout << "\n[ INFO ] Received " << sDecision.c_str() << " from Coordinator";

		res = (sDecision == "COMMIT") ? commit(newSocket) : abort(newSocket);

		sOutput = sOutput + "\r\n";
		res = Socket::sendData(socketData, sOutput);
		cout << "\n[ INFO ] Sent \'" << sOutput << "\' back to Coordinator";

		sleep(0.5);
		i++;
	}

	return res;
}
