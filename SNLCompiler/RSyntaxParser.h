/**
	µÝ¹éÏÂ½µÓï·¨·ÖÎöÆ÷
*/
#pragma once

#include <vector>
#include <map>
#include "SyntaxParserCommon.h"
#include "LexicalAnalyzer.h"

using namespace std;

class RTreeNode
{
public:
	RTreeNode() 
	{
		//mSemName = _T("");
		mLine = -1;
		mNodeType = NodeType::Terminal;
		mChilds = vector<RTreeNode*>();
	}
	vector<RTreeNode*> mChilds;
	int mLine;
	NodeType mNodeType;
	Token mToken;
};

class RSyntaxParser
{
public:
	RSyntaxParser();
	RSyntaxParser(vector<Token> tokens);
	~RSyntaxParser();

	//vector<RTreeNode*> mChildNodes;
	vector<Token> mTokenList;
	vector<ParseLog> mParseLog;
	map<NodeType, CString> mNodeType2Str;
	int mTokenPtr;
	int mCurLine;
	LexicalAnalyzer mLexicalAnalyzer;

	RTreeNode* mSytaxTree;

	RTreeNode* Parse();
	RTreeNode* Program();
	RTreeNode* ProgramHead();
	RTreeNode* DeclarePart();
	RTreeNode* ProcDec();
	RTreeNode* ProcDeclaration();
	RTreeNode* ProcDecMore();
	RTreeNode* ProcDecPart();
	RTreeNode* ProcBody();
	RTreeNode* ParamList();
	RTreeNode* ParamDecList();
	RTreeNode* ParamMore();
	RTreeNode* Param();
	RTreeNode* FormList();
	RTreeNode* FidMore();
	RTreeNode* VarDec();
	RTreeNode* VarDeclaration();
	RTreeNode* VarDecList();
	RTreeNode* VarIdMore();
	RTreeNode* VarIdList();
	RTreeNode* TypeDecMore();
	RTreeNode* StmList();
	RTreeNode* StmMore();
	RTreeNode* Stm();
	RTreeNode* AssCall();
	RTreeNode* AssignmentRest();
	RTreeNode* ConditionalStm();
	RTreeNode* LoopStm();
	RTreeNode* InputStm();
	RTreeNode* OutputStm();
	RTreeNode* ReturnStm();
	RTreeNode* CallStmRest();
	RTreeNode* ActParamList();
	RTreeNode* ActParamMore();
	RTreeNode* RelExp();
	RTreeNode* OtherRelE();
	RTreeNode* Exp();
	RTreeNode* OtherTerm();
	RTreeNode* Term();
	RTreeNode* Factor();
	RTreeNode* OtherFactor();
	RTreeNode* Variable();
	RTreeNode* VariMore();
	RTreeNode* FieldVar();
	RTreeNode* FieldVarMore();
	RTreeNode* CmpOp();
	RTreeNode* AddOp();
	RTreeNode* MultOp();
	RTreeNode* ProgramBody();

	RTreeNode* ProgramName();
	void NextToken();
	Token GetCurToken();

	RTreeNode* VarDecMore();
	RTreeNode* TypeDec();
	RTreeNode* TypeDeclaration();
	RTreeNode* TypeDecList();
	RTreeNode* TypeId();
	RTreeNode* TypeDef();
	RTreeNode* BaseType();
	RTreeNode* StructureType();
	RTreeNode* ArrayType();
	RTreeNode* RecType();
	RTreeNode* FieldDecList();
	RTreeNode* IdList();
	RTreeNode* IdMore();
	RTreeNode* FieldDecMore();
	bool Match(LexType type);
	RTreeNode* GetMatchedTerminal(Token t);
	void ReleaseTree(RTreeNode* r);

	void RecordLog(LogType type, int line, CString log);
	void InitMap();
	CString GetSyntaxTreeStr(CString lftstr, CString append, RTreeNode* r);
	CString GetStrByLen(int len);
};

