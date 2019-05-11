#include "pch.h"
#include "Utils.h"

Utils::Utils()
{
}


Utils::~Utils()
{
}


int Utils::cstr2int(CString str)
{
	return _ttoi(str);
}

CString Utils::int2cstr(int s)
{
	CString str;
	str.Format(_T("%d"), s);
	return str;
}

//template <typename... Types>
//CString Utils::FormatCString(CString format, const Types& ... args)
//{
//	CString s;
//	s.Format(format, args...);
//	return s;
//}
