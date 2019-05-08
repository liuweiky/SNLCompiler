#include "pch.h"
#include "LexicalAnalyzer.h"

LexicalAnalyzer::LexicalAnalyzer()
{
	CFile infile(_T("../TestCase/bubble_sort_err.txt"), CFile::modeRead, NULL);
	int len = infile.GetLength();
	char* buf = new char[len + 2];
	buf[len] = buf[len + 1] = '\0';
	infile.Read(buf, len);
	mOrignalSrcCode = buf;
	delete[] buf;
	infile.Close();
	

	mSrcPtr = 0;
	mCurLine = 1;

	mLex2String[LexType::PROGRAM] = "<PROGRAM>";
	mLex2String[LexType::IDENTIFIER] = "<IDENTIFIER>";
	mLex2String[LexType::TYPE] = "<TYPE>";
	mLex2String[LexType::UINTEGER] = "<UINTEGER>";
	mLex2String[LexType::EQU] = "<EQU>";
	mLex2String[LexType::LT] = "<LT>";
	mLex2String[LexType::LEXEOF] = "<LEXEOF>";
	mLex2String[LexType::SPACE] = "<SPACE>";
	mLex2String[LexType::INTEGER] = "<INTEGER>";
	mLex2String[LexType::ARRAY] = "<ARRAY>";
	mLex2String[LexType::OF] = "<OF>";
	mLex2String[LexType::PROCEDURE] = "<PROCEDURE>";
	mLex2String[LexType::SEMICOLON] = "<SEMICOLON>";
	mLex2String[LexType::VAR] = "<VAR>";
	mLex2String[LexType::BEGIN] = "<BEGIN>";
	mLex2String[LexType::END] = "<END>";
	mLex2String[LexType::WHILE] = "<WHILE>";
	mLex2String[LexType::DO] = "<DO>";
	mLex2String[LexType::ENDWHILE] = "<ENDWHILE>";
	mLex2String[LexType::IF] = "<IF>";
	mLex2String[LexType::THEN] = "<THEN>";
	mLex2String[LexType::ELSE] = "<ELSE>";
	mLex2String[LexType::FI] = "<FI>";
	mLex2String[LexType::READ] = "<READ>";
	mLex2String[LexType::WRITE] = "<WRITE>";
	mLex2String[LexType::LPARENTHESIS] = "<LPARENTHESIS>";
	mLex2String[LexType::RPARENTHESIS] = "<RPARENTHESIS>";
	mLex2String[LexType::LSQUAREBRACKET] = "<LSQUAREBRACKET>";
	mLex2String[LexType::RSQUAREBRACKET] = "<RSQUAREBRACKET>";
	mLex2String[LexType::ASSIGN] = "<ASSIGN>";
	mLex2String[LexType::PLUS] = "<PLUS>";
	mLex2String[LexType::MINUS] = "<MINUS>";
	mLex2String[LexType::MULTIPLY] = "<MULTIPLY>";
	mLex2String[LexType::DIVIDE] = "<DIVIDE>";
	mLex2String[LexType::COMMA] = "<COMMA>";
	mLex2String[LexType::UNDERANGE] = "<UNDERANGE>";
	mLex2String[LexType::DOT] = "<DOT>";

	mReservedWords[_T("program")] = LexType::PROGRAM;
	mReservedWords[_T("var")] = LexType::VAR;
	mReservedWords[_T("integer")] = LexType::INTEGER;
	mReservedWords[_T("array")] = LexType::ARRAY;
	mReservedWords[_T("of")] = LexType::OF;
	mReservedWords[_T("procedure")] = LexType::PROCEDURE;
	mReservedWords[_T("var")] = LexType::VAR;
	mReservedWords[_T("begin")] = LexType::BEGIN;
	mReservedWords[_T("end")] = LexType::END;
	mReservedWords[_T("while")] = LexType::WHILE;
	mReservedWords[_T("do")] = LexType::DO;
	mReservedWords[_T("endwh")] = LexType::ENDWHILE;
	mReservedWords[_T("if")] = LexType::IF;
	mReservedWords[_T("then")] = LexType::THEN;
	mReservedWords[_T("else")] = LexType::ELSE;
	mReservedWords[_T("fi")] = LexType::FI;
	mReservedWords[_T("read")] = LexType::READ;
	mReservedWords[_T("write")] = LexType::WRITE;
}


LexicalAnalyzer::~LexicalAnalyzer()
{
}


CString LexicalAnalyzer::getNextChar()
{
	CString ret = _T("");
	if (mSrcPtr >= mOrignalSrcCode.GetLength())
		ret = "\0";
	else
		ret = mOrignalSrcCode.GetAt(mSrcPtr++);
	return ret;
}

void LexicalAnalyzer::ungetNextChar()
{
	mSrcPtr--;
}

void LexicalAnalyzer::getTokenList()
{
	CString str = _T("");
S0:
	CString cur_char = getNextChar();
	if (isAlpha(cur_char))
	{
		goto INID;
	}
	else if (isDigit(cur_char))
	{
		goto INNUM;
	}
	else if (isSingleDelimiter(cur_char))
	{
		goto INSINGLE;
	}
	else if (cur_char == ":")
	{
		goto INASSIGN;
	}
	else if (cur_char == "{")
	{
		goto INCOMMENT;
	}
	else if (cur_char == ".")
	{
		goto INRANGE;
	}
	/*else if (cur_char == ',')
	{
		goto INCHAR;
	}*/
	else
	{
		goto INERROR;
	}

INID:
	str += cur_char;
	cur_char = getNextChar();
	if (isAlNum(cur_char))
	{
		goto INID;
	}
	else
	{
		Token t(mCurLine, LexType::IDENTIFIER, str);
		if (isReservedWord(str))
			t.lex = mReservedWords[str];
		mTokenList.push_back(t);
		str = "";
		ungetNextChar();
		goto S0;
	}

INNUM:
	str += cur_char;
	cur_char = getNextChar();
	if (isDigit(cur_char))
	{
		goto INNUM;
	}
	else
	{
		Token t(mCurLine, LexType::UINTEGER, str);
		mTokenList.push_back(t);
		str = "";
		ungetNextChar();
		goto S0;
	}

INSINGLE:
	{
		Token t;
		str += cur_char;
		t.line = mCurLine;
		t.sem = str;
		str = "";
		if (cur_char == "\n")
		{
			mCurLine++;
			goto S0;
		}
		else if (cur_char == "+")
		{
			t.lex = LexType::PLUS;
		}
		else if (cur_char == "-")
		{
			t.lex = LexType::MINUS;
		}
		else if (cur_char == "*")
		{
			t.lex = LexType::MULTIPLY;
		}
		else if (cur_char == "/")
		{
			t.lex = LexType::DIVIDE;
		}
		else if (cur_char == "(")
		{
			t.lex = LexType::LPARENTHESIS;
		}
		else if (cur_char == ")")
		{
			t.lex = LexType::RPARENTHESIS;
		}
		else if (cur_char == ";")
		{
			t.lex = LexType::SEMICOLON;
		}
		else if (cur_char == "[")
		{
			t.lex = LexType::LSQUAREBRACKET;
		}
		else if (cur_char == "]")
		{
			t.lex = LexType::RSQUAREBRACKET;
		}
		else if (cur_char == "=")
		{
			t.lex = LexType::EQU;
		}
		else if (cur_char == "<")
		{
			t.lex = LexType::LT;
		}
		else if (cur_char == "\0")
		{
			t.lex = LexType::LEXEOF;
			mTokenList.push_back(t);
			return;
		}
		else if (cur_char == " " || cur_char == "\t" || cur_char == "\r")
		{
			//t.lex = LexType::SPACE;
			goto S0;
		}
		else if (cur_char == ",")
		{
			t.lex = LexType::COMMA;
		}

		mTokenList.push_back(t);

		goto S0;
	}


INASSIGN:
	str += cur_char;
	cur_char = getNextChar();
	if (cur_char == "=")
	{
		str += cur_char;
		Token t(mCurLine, LexType::ASSIGN, str);
		mTokenList.push_back(t);
		str = "";
		goto S0;
	}
	else
	{
		goto INERROR;
	}

INCOMMENT:
	//str += cur_char;
	{
		str = "";
		goto S0;
	}

INRANGE:
	str += cur_char;
	cur_char = getNextChar();
	if (cur_char == ".")
	{
		str += cur_char;
		Token t(mCurLine, LexType::UNDERANGE, str);
		mTokenList.push_back(t);
		str = "";
		goto S0;
	}
	else
	{
		Token t(mCurLine, LexType::DOT, str);
		mTokenList.push_back(t);
		str = "";
		ungetNextChar();
		goto S0;
	}

	/*INCHAR:
		str += cur_char;
		cur_char = getNextChar();
		if (isalnum(cur_char))
		{
			goto DONE;
		}
		else
		{
			goto INERROR;
		}*/
INERROR:
	{
		Token t(mCurLine, LexType::LEXERR, str + cur_char);
		mTokenList.push_back(t);
		CString s = _T("Unexpected character \"");
		LogUtil::Error(s + cur_char + "\" in line " + Utils::int2cstr(mCurLine));
		/*cur_char = getNextChar();
		while (!isSingleDelimiter(cur_char))
			cur_char = getNextChar();
		ungetNextChar();*/
		goto S0;
	}
	return;
}




void LexicalAnalyzer::Lex2File()
{
	CFile outfile(LEXFILENAME, CFile::modeCreate | CFile::modeReadWrite);
	CString outstr = _T("");
	BYTE UH[] = { 0xff, 0xfe };
	outfile.Write(UH, 2);
	for (int i = 0; i < mTokenList.size(); i++)
	{
		Token t = mTokenList[i];
		if (t.lex == LexType::LEXERR)
		{
			outstr += _T("[ERROR] Unexpected character \"");
			outstr += t.sem;
			outstr += _T("\" in line ");
			outstr += Utils::int2cstr(t.line);
			outstr += "\r\n";
			continue;
		}
		CString linestr = _T("");
		linestr += _T("LINE ");
		linestr += Utils::int2cstr(t.line);
		linestr += _T(": ");
		int k = 10 - linestr.GetLength();
		if (k > 0)
			while (k--)
				linestr += " ";
		linestr += mLex2String[t.lex];
		k = 30 - linestr.GetLength();
		if (k > 0)
			while (k--)
				linestr += " ";
		linestr += t.sem;
		linestr += "\r\n";
		outstr += linestr;
	}
	outfile.Write((LPCTSTR)outstr.GetBuffer(outstr.GetLength()), outstr.GetLength() * sizeof(TCHAR));
	outfile.Close();
	AfxMessageBox(outstr);
}


bool LexicalAnalyzer::isSingleDelimiter(CString ch)
{

	return (
		ch == "+" || ch == "-" || ch == "*" || ch == "/"
		|| ch == "(" || ch == ")" || ch == ";" || ch == "["
		|| ch == "]" || ch == "=" || ch == "<" || ch == " "
		|| ch == "\n" || ch == "\t" || ch == "\r" || ch == "\0"
		|| ch == ","
	);
}


bool LexicalAnalyzer::isReservedWord(CString word)
{

	return mReservedWords.find(word) != mReservedWords.end();
}


bool LexicalAnalyzer::isAlpha(CString ch)
{
	if (ch == "\0")
		return false;
	static CString alpha = _T("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
	return alpha.Find(ch) != -1;
}


bool LexicalAnalyzer::isDigit(CString ch)
{
	if (ch == "\0")
		return false;
	static CString digit = _T("0123456789");
	return digit.Find(ch) != -1;
}


bool LexicalAnalyzer::isAlNum(CString ch)
{
	return isAlpha(ch) || isDigit(ch);
}


