#include "pch.h"
#include "LexicalAnalyzer.h"
#include <cctype>
#include <iomanip>


LexicalAnalyzer::LexicalAnalyzer()
{
	ifstream infile("../TestCase/bubble_sort_err.txt");
	string line;
	if (!infile.is_open())
		LogUtil::Error("Failed to read source code");
	else
	{
		while (getline(infile, line))
		{
			mOrignalSrcCode += line;
			mOrignalSrcCode += '\n';
		}
		LogUtil::Debug(mOrignalSrcCode);
	}
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

	mReservedWords["program"] = LexType::PROGRAM;
	mReservedWords["var"] = LexType::VAR;
	mReservedWords["integer"] = LexType::INTEGER;
	mReservedWords["array"] = LexType::ARRAY;
	mReservedWords["of"] = LexType::OF;
	mReservedWords["procedure"] = LexType::PROCEDURE;
	mReservedWords["var"] = LexType::VAR;
	mReservedWords["begin"] = LexType::BEGIN;
	mReservedWords["end"] = LexType::END;
	mReservedWords["while"] = LexType::WHILE;
	mReservedWords["do"] = LexType::DO;
	mReservedWords["endwh"] = LexType::ENDWHILE;
	mReservedWords["if"] = LexType::IF;
	mReservedWords["then"] = LexType::THEN;
	mReservedWords["else"] = LexType::ELSE;
	mReservedWords["fi"] = LexType::FI;
	mReservedWords["read"] = LexType::READ;
	mReservedWords["write"] = LexType::WRITE;
}


LexicalAnalyzer::~LexicalAnalyzer()
{
}


char LexicalAnalyzer::getNextChar()
{
	return mSrcPtr == mOrignalSrcCode.size() ? '\0' : mOrignalSrcCode[mSrcPtr++];
}

void LexicalAnalyzer::ungetNextChar()
{
	mSrcPtr--;
}

void LexicalAnalyzer::getTokenList()
{
	string str = "";
S0:
	char cur_char = getNextChar();
	if (isalpha(cur_char))
	{
		goto INID;
	}
	else if (isdigit(cur_char))
	{
		goto INNUM;
	}
	else if (isSingleDelimiter(cur_char))
	{
		goto INSINGLE;
	}
	else if (cur_char == ':')
	{
		goto INASSIGN;
	}
	else if (cur_char == '{')
	{
		goto INCOMMENT;
	}
	else if (cur_char == '.')
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
	if (isalnum(cur_char))
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
	if (isdigit(cur_char))
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
		if (cur_char == '\n')
		{
			mCurLine++;
			goto S0;
		}
		else if (cur_char == '+')
		{
			t.lex = LexType::PLUS;
		}
		else if (cur_char == '-')
		{
			t.lex = LexType::MINUS;
		}
		else if (cur_char == '*')
		{
			t.lex = LexType::MULTIPLY;
		}
		else if (cur_char == '/')
		{
			t.lex = LexType::DIVIDE;
		}
		else if (cur_char == '(')
		{
			t.lex = LexType::LPARENTHESIS;
		}
		else if (cur_char == ')')
		{
			t.lex = LexType::RPARENTHESIS;
		}
		else if (cur_char == ';')
		{
			t.lex = LexType::SEMICOLON;
		}
		else if (cur_char == '[')
		{
			t.lex = LexType::LSQUAREBRACKET;
		}
		else if (cur_char == ']')
		{
			t.lex = LexType::RSQUAREBRACKET;
		}
		else if (cur_char == '=')
		{
			t.lex = LexType::EQU;
		}
		else if (cur_char == '<')
		{
			t.lex = LexType::LT;
		}
		else if (cur_char == '\0')
		{
			t.lex = LexType::LEXEOF;
			mTokenList.push_back(t);
			return;
		}
		else if (cur_char == ' ' || cur_char == '\t' || cur_char == '\r')
		{
			//t.lex = LexType::SPACE;
			goto S0;
		}
		else if (cur_char == ',')
		{
			t.lex = LexType::COMMA;
		}

		mTokenList.push_back(t);

		goto S0;
	}


INASSIGN:
	str += cur_char;
	cur_char = getNextChar();
	if (cur_char == '=')
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
	if (cur_char == '.')
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
		string s = "Unexpected character '";
		LogUtil::Error(s + cur_char + "' in line " + Utils::int2str(mCurLine));
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
	ofstream outfile(LEXFILENAME, ios::trunc);
	if (!outfile.is_open())
	{
		LogUtil::Error("Unable to open " + LEXFILENAME);
		return;
	}
	for (int i = 0; i < mTokenList.size(); i++)
	{
		Token t = mTokenList[i];
		if (t.lex == LexType::LEXERR)
		{
			outfile << "[ERROR] Unexpected character \"" << t.sem << "\" in line " << t.line << endl;
			continue;
		}
		outfile << std::left << "LINE " << t.line << setw(5) << ": " << setw(20) << mLex2String[t.lex] << t.sem << endl;
	}
	outfile.close();
}


bool LexicalAnalyzer::isSingleDelimiter(char ch)
{

	return (
		ch == '+' || ch == '-' || ch == '*' || ch == '/'
		|| ch == '(' || ch == ')' || ch == ';' || ch == '['
		|| ch == ']' || ch == '=' || ch == '<' || ch == ' '
		|| ch == '\n' || ch == '\t' || ch == '\r' || ch == '\0'
		|| ch == ','
		);
}


bool LexicalAnalyzer::isReservedWord(string word)
{

	return mReservedWords.find(word) != mReservedWords.end();
}
