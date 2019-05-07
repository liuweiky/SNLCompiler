#include "pch.h"
#include "LogUtil.h"
#include <iostream>


LogUtil::LogUtil()
{
}


LogUtil::~LogUtil()
{
}


void LogUtil::info(string s)
{
	string ostring = "[INFO]\t" + s;
	ostring += '\n';
	size_t size = ostring.length();
	wchar_t* buffer = new wchar_t[size + 1];
	MultiByteToWideChar(CP_ACP, 0, ostring.c_str(), size, buffer, size * sizeof(wchar_t));
	buffer[size] = 0;
	OutputDebugString(buffer);
	delete[] buffer;
}


void LogUtil::Error(string s)
{
	string ostring = "[ERROR]\t" + s;
	ostring += '\n';
	size_t size = ostring.length();
	wchar_t* buffer = new wchar_t[size + 1];
	MultiByteToWideChar(CP_ACP, 0, ostring.c_str(), size, buffer, size * sizeof(wchar_t));
	buffer[size] = 0;
	OutputDebugString(buffer);
	delete[] buffer;
}


void LogUtil::Debug(string s)
{
	string ostring = "[DEBUG]\t" + s;
	ostring += '\n';
	size_t size = ostring.length();
	wchar_t* buffer = new wchar_t[size + 1];
	MultiByteToWideChar(CP_ACP, 0, ostring.c_str(), size, buffer, size * sizeof(wchar_t));
	buffer[size] = 0;
	OutputDebugString(buffer);
	delete[] buffer;
}
