/*
 * StringUtilities.cpp
 *
 *  Created on: Dec 8, 2019
 *      Author: shreya
 */

#include "StringUtilities.h"

using std::vector;
using std::string;
using std::stringstream;


vector<string> StringUtilities::StringSplitByDelim(std::string str, char& cDeLim)
{
	vector<string> vTokens;
	stringstream ss(str);
	string item;

	while (std::getline(ss, item, cDeLim))
	{
		if(item != " " || item != "\n" || item!= "\r")
			vTokens.push_back (item);
	}

//	for (int i=0; i< vTokens.size(); i++)
//	{
//		cout << "\nvToken: " << vTokens[i];
//	}

	return vTokens;
}

std::string& StringUtilities::ltrim(std::string & str)
{
  auto it2 =  std::find_if( str.begin() , str.end() , [](char ch){ return !std::isspace<char>(ch , std::locale::classic() ) ; } );
  str.erase( str.begin() , it2);
  return str;
}

std::string& StringUtilities::rtrim(std::string & str)
{
  auto it1 =  std::find_if( str.rbegin() , str.rend() , [](char ch){ return !std::isspace<char>(ch , std::locale::classic() ) ; } );
  str.erase( it1.base() , str.end() );
  return str;
}

std::string& StringUtilities::trim(std::string & str)
{
   return ltrim(rtrim(str));
}
