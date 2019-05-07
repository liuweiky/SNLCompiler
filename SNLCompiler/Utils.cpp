#include "pch.h"
#include "Utils.h"

#include <sstream>

Utils::Utils()
{
}


Utils::~Utils()
{
}


int Utils::str2int(string str)
{
	int s;
	stringstream ss;
	ss << str;
	ss >> s;
	return s;
}

string Utils::int2str(int s)
{
	string str;
	stringstream ss;
	ss << s;
	ss >> str;
	return str;
}
