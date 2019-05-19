#pragma once

#include <vector>
#include <map>
#include <set>
#include "SyntaxParserCommon.h"
#include "LexicalAnalyzer.h"

#define STR_EPS CString(_T("<EPSILON>"))
#define EPS_ITEM SyntaxItem(LexType::EPSILON)
#define PRODUCTION_FILENAME _T("../TestCase/syntax.txt")

using namespace std;

struct SyntaxItem
{
	SyntaxItem() 
	{
		nodeType = NodeType::Terminal;
		token = LexType::LEXERR;
	}
	SyntaxItem(NodeType n)
	{
		nodeType = n;
		token = LexType::LEXERR;
	}
	SyntaxItem(LexType t)
	{
		nodeType = NodeType::Terminal;
		token = t;
	}
	SyntaxItem(NodeType n, LexType t): nodeType(n), token(t){}
	bool operator < (const SyntaxItem& item) const
	{
		return nodeType * 100000 + token < item.nodeType * 100000 + item.token;
	}
	NodeType nodeType;
	LexType token;	// 当nodeType为Terminal（终极符）时该项有效，否则，该项应为LEXERR
	void SetNodeType(NodeType n) { nodeType = n; token = LexType::LEXERR; }
	void SetLexType(LexType t) { nodeType = Terminal; token = t; }
};

struct Production
{
	SyntaxItem mLeft;
	int no;
	vector<SyntaxItem> mRights;
};

class LL1SyntaxParser
{
public:
	LL1SyntaxParser();
	LL1SyntaxParser(vector<Token> tokens);
	~LL1SyntaxParser();

	void NextToken();

	void Parse();
	void InitMap();

	map<NodeType, map<LexType, vector<SyntaxItem>>> mLL1Map;
	int mTokenPtr;

	Token GetCurToken();
	vector<ParseLog> mParseLog;
	int mCurLine;
	map<NodeType, CString> mNodeType2Str;
	map<CString, NodeType> mStr2NodeType;

	LexicalAnalyzer mLexicalAnalyzer;
	vector<Token> mTokenList;

	vector<Production> mProductions;
	map<int, vector<SyntaxItem>> mId2Rights;
	void ReadProuctions();
	vector<CString> SplitString(CString str, CString pattern);

	map<SyntaxItem, set<SyntaxItem>> mFirstSet;
	map<SyntaxItem, set<SyntaxItem>> mFollowSet;
	map<int, set<SyntaxItem>> mPredictSet;
	
	void GetFirstSet();
	void GetFollowSet();
	void GetPredictSet();
	set<SyntaxItem> GetSyntaxListFirst(vector<SyntaxItem> items);
	
	set<SyntaxItem> mNonTerminals;
	set<SyntaxItem> mTerminals;
	SyntaxItem mBeginSyntax;
};

