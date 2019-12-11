/*
 * GringottsFrontend.cpp
 *
 *  Created on: Dec 8, 2019
 *      Author: shreya
 */


#include "Coordinator.h"
#include <string.h>

using std::cout;


int main(int argc, char* argv[])
{
	Coordinator c;

	if (argc != 2)
	{
		cout << "\n[ERROR ] Invalid command line arguments.";
		return -1;
	}

	c.setPortNum(std::stoi(argv[1]));

	if (!c.initServer())
	{
		cout << "\n[ERROR ] Failed to initialize server.";
		return -1;
	}

	if (!c.runServer())
	{
		cout << "\n[ERROR ] Something went wrong in the server.";
		return -1;
	}


	std::cout << std::endl;
	return 0;
}
