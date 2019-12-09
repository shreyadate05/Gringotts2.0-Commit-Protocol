/*
 * Cohort.cpp
 *
 *  Created on: Dec 8, 2019
 *      Author: shreya
 */

#include "Cohort.h"

using std::cout;
using std::string;
using std::vector;
using std::map;
using std::cin;


bool Cohort::setPortNum(int iPortNum)
{
	socketData.iPortNum = iPortNum;
	return true;
}

bool Cohort::parseCommand()
{
	bool res = true;
	string sReady;

	char cDeLim = ' ';
	vCommands.clear();
	vCommands = StringUtilities::StringSplitByDelim(sCommand, cDeLim);
	if (vCommands.size() == 0)
	{
		cout << "\n[ERROR ] Invalid command " << sCommand.c_str();
		return false;
	}

	std::transform(vCommands[0].begin(), vCommands[0].end(), vCommands[0].begin(), ::toupper);


	if(vCommands[0].c_str() == "CREATE")
		res = create();

	else if (vCommands[0].c_str() == "UPDATE")
		res = update();

	else if (vCommands[0].c_str() == "QUERY")
		res = query();

	else if (vCommands[0].c_str() == "QUIT")
			res = exit();

	res &= sendOutput();

	return res;
}

bool Cohort::sendOutput()
{
	bool res = true;

	res = Socket::sendData(socketData, sOutput.c_str());
	if (!res)
	{
		cout << "\n[ERROR ] Socket error occurred. Exiting client.";
	}

	return res;
}

bool Cohort::create()
{
	bool res = true;

	if (vCommands.size() != 2)
	{
		cout << "\n[ERROR ] Invalid command " << sCommand.c_str();
		return false;
	}

	iAccountNumber++;
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

bool Cohort::update()
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

bool Cohort::query()
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

bool Cohort::exit()
{
	bool res = true;

	res = Socket::closeSocket(socketData);

	return res;
}

bool Cohort::isReady(SocketData& newSocket)
{
	bool res = true;
	string sReady;

	// coordinator sends message in the format <client_request|READY?>
	res = Socket::recvData(newSocket, sReady);
	if (!res)
	{
		cout << "\n[ERROR ] Socket error occurred. Exiting client.";
	}
	cout << "\n[ INFO ] Received voting message " << sReady.c_str() << " from Coordinator";

	char  cDeLim = '|';
	vector<string> vMessage = StringUtilities::StringSplitByDelim(sReady, cDeLim);
	(vMessage.size() != 2) ? sReady = "FAIL" : sReady = "OK";

	sCommand = vMessage[0];
	res = Socket::sendData(socketData, sReady.c_str());
	if (!res)
	{
		cout << "\n[ERROR ] Socket error occurred. Exiting client.";
	}
	cout << "\n[ INFO ] Sent \'" << sReady << "\' back to Coordinator";


	return res;
}

bool Cohort::finalCall(SocketData& newSocket)
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

bool Cohort::commit(SocketData& newSocket)
{
	bool res = true;

	res = parseCommand();

	return res;
}

bool Cohort::abort(SocketData& newSocket)
{
	bool res = true;

	cout << "[ INFO ] Aborting transaction. ";

	return res;
}

bool Cohort::initServer()
{
	bool res = true;
	iAccountNumber = 100;

	if ( ! Socket::createSocket(socketData) )
	{
		cout << "\n[ERROR ] " << __func__;
		return false;
	}

	if ( ! Socket::bind(socketData) )
	{
		cout << "\n[ERROR ] " << __func__;
		return false;
	}

	if ( ! Socket::listen(socketData) )
	{
		cout << "\n[ERROR ] " << __func__;
		return false;
	}


	return res;
}

bool Cohort::runServer()
{
	bool res = true;

	int i = 0;
	while(true)
	{
		SocketData* newSocket = new SocketData();
		newSocket->iPortNum = socketData.iPortNum;
		newSocket->sHostname = socketData.sHostname;

		res = Socket::accept(socketData, *newSocket);
		res = isReady(*newSocket);
		res = finalCall(*newSocket);

	}

	return res;
}

