#include "pch.h"
#include "Utils.h"

Utils::Utils()
{
}


Utils::~Utils()
{
}


int Utils::Cstr2Int(CString str)
{
	return _ttoi(str);
}

CString Utils::Int2Cstr(int s)
{
	CString str;
	str.Format(_T("%d"), s);
	return str;
}

