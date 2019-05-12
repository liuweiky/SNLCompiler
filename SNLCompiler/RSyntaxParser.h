/**
	递归下降语法分析器
*/
#pragma once

#include <vector>
#include "LexicalAnalyzer.h"

using namespace std;

enum LogType
{
	LERROR,
	LINFO,
	LDEBUG
};

// 语句块类型
enum NodeType
{
	Program,		// Program 标志块，是整个程序块，相当于开始符
	ProgramHead,		// Program Head 标志块，例如 program bubble
	ProgramName,
	ProgramBody,
	StmList,
	Stm,
	StmMore,
	DeclarePart,
	TypeDec,		// Type Declare 标志块，类型声明
	EMPTY,
	TypeDecList,
	TypeId,
	TypeDef,
	TypeDecMore,
	BaseType,
	StructureType,
	ArrayType,
	RecType,
	FieldDecList,
	IdList,
	FieldDecMore,
	IdMore,
	VarDec,		// Var Declare 标志块，变量声明
	VarDeclaration,
	VarDecList,
	VarIdList,
	VarIdMore,
	VarDecMore,
	ProcDec,
	ProcDeclaration,
	ProcDecMore,
	ParamList,
	ParamDecList,
	Param,
	ParamMore,
	FormList,
	FidMore,
	ProcDecPart,
	ProcBody,
	AssCall,
	AssignmentRest,
	ConditionalStm,
	LoopStm,
	InputStm,
	OutputStm,
	ReturnStm,
	CallStmRest,
	ActParamList,
	ActParamMore,
	Exp,
	OtherTerm,
	Term,
	OtherFactor,
	Factor,
	Variable,
	VariMore,
	FieldVar,
	FieldVarMore,
	RelExp,
	OtherRelE,
	CmpOp,
	AddOp,
	MultOp,
	Terminal	// 终极符
};

struct ParseLog
{
	ParseLog(int li, LogType t, CString lg): line(li), type(t), log(lg){}
	int line;
	LogType type;
	CString log;
};

class RTreeNode
{
public:
	RTreeNode() 
	{
		mSemName = _T("");
		mLine = -1;
		mNodeType = NodeType::Terminal;
		mChilds = vector<RTreeNode*>();
	}
	vector<RTreeNode*> mChilds;
	int mLine;
	NodeType mNodeType;
	CString mSemName;
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
};

