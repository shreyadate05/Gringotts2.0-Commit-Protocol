/*
 * GringottsBackend.cpp
 *
 *  Created on: Dec 8, 2019
 *      Author: shreya
 */

#include "Backend.h"
#include <string.h>

using std::cout;


int main(int argc, char* argv[])
{
	Backend b;

	if (argc != 2)
	{
		cout << "\n[ERROR ] Invalid command line arguments.";
		return -1;
	}

	b.setPortNum(std::stoi(argv[1]));
	b.setHostname("localhost");

	if (!b.initClient())
	{
		cout << "\n[ERROR ] Failed to initialize backend server.";
		return -1;
	}

	if (!b.runClient())
	{
		cout << "\n[ERROR ] Something went wrong in the backend server.";
		return -1;
	}


	std::cout << std::endl;
	return 0;
}
