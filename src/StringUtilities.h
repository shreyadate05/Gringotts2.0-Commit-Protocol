/*
 * StringUtilities.h
 *
 *  Created on: Dec 8, 2019
 *      Author: shreya
 */

#ifndef STRINGUTILITIES_H_
#define STRINGUTILITIES_H_

#include <string>
#include <vector>
#include <string.h>
#include <iostream>
#include <sstream>
#include <algorithm>

class StringUtilities
{
	public:

		static std::vector<std::string> StringSplitByDelim(std::string str, char& cDeLim);
		static std::string & ltrim(std::string & str);
		static std::string & rtrim(std::string & str);
		static std::string & trim(std::string & str);
};


#endif /* STRINGUTILITIES_H_ */
