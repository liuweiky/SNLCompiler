/**
	DFA ´Ê·¨·ÖÎöÆ÷
*/
#pragma once

#include <string>
#include <fstream>
#include <vector>
#include "LogUtil.h"
#include "Utils.h"
#include <sstream>
#include <map>

using namespace std;

const string LEXFILENAME = "../TestCase/out/lex.txt";

enum LexType
{
	PROGRAM,
	IDENTIFIER,
	TYPE,
	UINTEGER,
	ARRAY,
	OF,
	PROCEDURE,
	EQU,
	LT,				// <
	LEXEOF,			// EOF
	SPACE,
	INTEGER,
	SEMICOLON,		// ;
	VAR,
	BEGIN,
	END,
	WHILE,
	DO,
	ENDWHILE,
	IF,
	THEN,
	ELSE,
	FI,
	CHARACTER,
	READ,
	WRITE,
	LPARENTHESIS,	// (
	RPARENTHESIS,	// )
	LSQUAREBRACKET,	// [
	RSQUAREBRACKET,	// ]
	ASSIGN,			// :=
	PLUS,
	MINUS,
	MULTIPLY,
	DIVIDE,
	COMMA,			// ,
	UNDERANGE,		// ..
	DOT,			// .
	LEXERR			// Not a lex element
};

struct Token
{
	Token(int li, LexType le, string se) : line(li), lex(le), sem(se)
	{
		/*string s = "";
		stringstream ss;
		ss << li;
		ss >> s;
		s = "line " + s;
		LogUtil::Debug(s + ' ' + se); */
	}
	Token() { line = -1; lex = LexType::LEXERR; sem = ""; }
	int line;
	LexType lex;
	string sem;
};

class LexicalAnalyzer
{
public:
	LexicalAnalyzer();
	~LexicalAnalyzer();
	char getNextChar();
	void ungetNextChar();
	string mOrignalSrcCode;
	void getTokenList();

	vector<Token> mTokenList;
	int mSrcPtr, mCurLine;

	map<LexType, string> mLex2String;
	map<string, LexType> mReservedWords;

	void Lex2File();
	bool isSingleDelimiter(char ch);
	bool isReservedWord(string word);
};

