/*
 * GringottsClient.cpp
 *
 *  Created on: Dec 8, 2019
 *      Author: shreya
 */


#include "Client.h"

using std::cout;
using std::endl;


int main(int argc, char* argv[])
{
	Client c;

	if (argc != 3)
	{
		cout << "\n[ERROR ] Invalid command line arguments.";
	}

	c.setPortNum(std::stoi(argv[1]));
	c.setHostname(argv[2]);

	c.initClient();
	c.runClient();


	std::cout << std::endl;
	return 0;
}
