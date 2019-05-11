/**
	π§æﬂ¿‡
*/
#pragma once

class Utils
{
public:
	Utils();
	~Utils();
	static int cstr2int(CString str);
	static CString int2cstr(int s);
	template <typename ... Types>
	static CString FormatCString(CString format, const Types& ... args)
	{
		CString s;
		s.Format(format, args...);
		return s;
	}
};

