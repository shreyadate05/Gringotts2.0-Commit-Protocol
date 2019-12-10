/*
 * GringottsBackend.cpp
 *
 *  Created on: Dec 8, 2019
 *      Author: shreya
 */

#include "Cohort.h"
#include <string.h>

using std::cout;


int main(int argc, char* argv[])
{
	Cohort c;

	if (argc != 2)
	{
		cout << "\n[ERROR ] Invalid command line arguments.";
		return -1;
	}

	c.setPortNum(std::stoi(argv[1]));



	std::cout << std::endl;
	return 0;
}
