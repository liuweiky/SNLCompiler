#include "pch.h"
#include "RSyntaxParser.h"
#include "LexicalAnalyzer.h"


RSyntaxParser::RSyntaxParser()
{
	mLexicalAnalyzer.getTokenList();
	mLexicalAnalyzer.Lex2File();
	for (int i = 0; i < mLexicalAnalyzer.mTokenList.size(); i++)
	{
		if (mLexicalAnalyzer.mTokenList[i].lex != LexType::LEXERR)
			mTokenList.push_back(mLexicalAnalyzer.mTokenList[i]);
	}

	mTokenPtr = 0;
	mCurLine = mTokenList.size() == 0 ? 0 : mTokenList[0].line;
	mSytaxTree = NULL;

	InitMap();
}

RSyntaxParser::RSyntaxParser(vector<Token> tokens)
{
	for (int i = 0; i < tokens.size(); i++)
	{
		if (tokens[i].lex != LexType::LEXERR)
			mTokenList.push_back(tokens[i]);
	}

	mTokenPtr = 0;
	mCurLine = mTokenList.size() == 0 ? 0 : mTokenList[0].line;
	mSytaxTree = NULL;

	InitMap();
}


RSyntaxParser::~RSyntaxParser()
{
	ReleaseTree(mSytaxTree);
}


// Token指针往后移一个
void RSyntaxParser::NextToken()
{
	if (mTokenPtr < mTokenList.size())
		mTokenPtr++;
}


Token RSyntaxParser::GetCurToken()
{
	if (mTokenPtr == mTokenList.size())
	{
		Token t;
		t.line = mCurLine;
		return t;
	}
	else
		return mTokenList[mTokenPtr];
}

// 该函数调用总程序处理函数，创建语法分析树。
// 同时处理文件的提前结束错误。
// 函数处理成功，则返回所生成的语句类型语法树节点。
// 否则，函数返回NULL。
RTreeNode* RSyntaxParser::Parse()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Now begin parsing. Good luck!")));
	LogUtil::Info(_T("Now begin parsing. Good luck!"));
	RTreeNode* r = Program();
	if (GetCurToken().lex != LexType::LEXEOF)
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, _T("The code ends too early!")));
		LogUtil::Error(_T("The code ends too early!"));
	}
	mSytaxTree = r;
	return r;
}


// Program ::= ProgramHead DeclarePart ProgramBody
RTreeNode* RSyntaxParser::Program()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [Program]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing Program in line %d"), mCurLine));
	RTreeNode* prog = new RTreeNode();
	prog->mNodeType = NodeType::Program;
	prog->mLine = mCurLine;

	RTreeNode* ph = ProgramHead();
	RTreeNode* dp = DeclarePart();
	RTreeNode* pb = ProgramBody();

	prog->mChilds.push_back(ph);
	prog->mChilds.push_back(dp);
	prog->mChilds.push_back(pb);

	if (ph == NULL)
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, _T("Missing [ProgramHead]")));
		LogUtil::Error(Utils::FormatCString(_T("Missing ProgramHead near line %d"), mCurLine));
	}

	if (dp == NULL)
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, _T("Missing [DeclarePart]")));
		LogUtil::Error(Utils::FormatCString(_T("Missing DeclarePart near line %d"), mCurLine));
	}

	if (pb == NULL)
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, _T("Missing [ProgramBody]")));
		LogUtil::Error(Utils::FormatCString(_T("Missing ProgramBody near line %d"), mCurLine));
	}
	Token t = GetCurToken();
	if (Match(LexType::DOT))
	{
		/*CString s;
		s.Format(_T("Missing <DOT> in line %d"), mCurLine);*/
		//LogUtil::Error(Utils::FormatCString(_T("Missing <DOT> near line %d"), mCurLine));
		prog->mChilds.push_back(GetMatchedTerminal(t));
	}
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[Program] Finished")));
	return prog;
}


// ProgramHead ::= T-<PROGRAM> ProgramName
RTreeNode* RSyntaxParser::ProgramHead()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [ProgramHead]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing ProgramHead in line %d"), mCurLine));
	RTreeNode* ph = new RTreeNode();
	ph->mNodeType = NodeType::ProgramHead;
	ph->mLine = mCurLine;

	Token t = GetCurToken();
	if (Match(LexType::PROGRAM))
	{
		ph->mChilds.push_back(GetMatchedTerminal(t));
		//LogUtil::Error(Utils::FormatCString(_T("Missing <PROGRAM> near line %d"), mCurLine));
	}

	RTreeNode* pn = ProgramName();
	ph->mChilds.push_back(pn);
	if (pn == NULL)
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, _T("Missing [ProgramName]")));
		LogUtil::Error(Utils::FormatCString(_T("Missing ProgramName near line %d"), mCurLine));
	}


	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[ProgramHead] Finished")));

	return ph;
}


// ProgramName ::= T-<IDENTIFIER>
RTreeNode* RSyntaxParser::ProgramName()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [ProgramName]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing ProgramName in line %d"), mCurLine));
	RTreeNode* pn = new RTreeNode();
	pn->mNodeType = NodeType::ProgramName;
	pn->mLine = mCurLine;
	
	Token t = GetCurToken();

	if (Match(LexType::IDENTIFIER))
	{
		pn->mChilds.push_back(GetMatchedTerminal(t));
	}
	else
	{
		//LogUtil::Error(Utils::FormatCString(_T("Missing <IDENTIFIER> near line %d"), mCurLine));
	}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[ProgramName] Finished")));
	return pn;
}

// DeclarePart ::= TypeDec VarDec ProcDec
RTreeNode* RSyntaxParser::DeclarePart()
{
	// 在 DeclarePart 中可以声明自定义类型（数组等）、变量、过程
	// DeclarePart 本身只是一个标记
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [DeclarePart]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing DeclarePart in line %d"), mCurLine));
	RTreeNode* dp = new RTreeNode();
	dp->mNodeType = NodeType::DeclarePart;
	dp->mLine = mCurLine;

	RTreeNode* td = TypeDec();
	dp->mChilds.push_back(td);
	if (td == NULL)
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, _T("Missing [TypeDec]")));
		LogUtil::Error(Utils::FormatCString(_T("Missing TypeDec near line %d"), mCurLine));
	}

	RTreeNode* vd = VarDec();
	dp->mChilds.push_back(vd);
	if (vd == NULL)
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, _T("Missing [VarDec]")));
		LogUtil::Error(Utils::FormatCString(_T("Missing VarDec near line %d"), mCurLine));
	}
		

	RTreeNode* pd = ProcDec();
	dp->mChilds.push_back(pd);
	if (vd == NULL)
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, _T("Missing [ProcDec]")));
		LogUtil::Error(Utils::FormatCString(_T("Missing ProcDec near line %d"), mCurLine));
	}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[DeclarePart] Finished")));
	return dp;
}


// ProcDec ::= 
//				ε { BEGIN }
//				| ProcDeclaration { PROCEDURE }
RTreeNode* RSyntaxParser::ProcDec()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [ProcDec]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing ProcDec in line %d"), mCurLine));
	RTreeNode* pd = new RTreeNode();
	pd->mNodeType = NodeType::ProcDec;
	pd->mLine = mCurLine;


	if (GetCurToken().lex == LexType::BEGIN)	// 向前看 1 个
	{
		RTreeNode* n = GetMatchedTerminal(Token(LexType::EPSILON));
		pd->mChilds.push_back(n);
		mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[ProcDec] is EPSILON")));
		LogUtil::Info(Utils::FormatCString(_T("ProcDec is EPSILON near line %d"), mCurLine));
	}
	else if (GetCurToken().lex == LexType::PROCEDURE)
	{
		pd->mChilds.push_back(ProcDeclaration());
	}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[ProcDec] Finished")));
	return pd;
}

/* 书上算法26：少了 ProcDecMore */
// ProcDeclaration ::= 
//						T-<PROCEDURE>
//						ProcName(ParamList)
//						ProcDecPart
//						ProcBody
//						ProcDecMore
RTreeNode* RSyntaxParser::ProcDeclaration()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [ProcDeclaration]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing ProcDeclaration in line %d"), mCurLine));
	RTreeNode* pd = new RTreeNode();
	pd->mNodeType = NodeType::ProcDeclaration;
	pd->mLine = mCurLine;

	Token t = GetCurToken();

	if (Match(LexType::PROCEDURE))
	{
		pd->mChilds.push_back(GetMatchedTerminal(t));
	}
	
	if (GetCurToken().lex == LexType::IDENTIFIER)
	{
		t = GetCurToken();

		if (Match(LexType::IDENTIFIER))
		{
			pd->mChilds.push_back(GetMatchedTerminal(t));
		}
	}

	t = GetCurToken();

	if (Match(LexType::LPARENTHESIS))
	{
		pd->mChilds.push_back(GetMatchedTerminal(t));
	}

	pd->mChilds.push_back(ParamList());

	t = GetCurToken();

	if (Match(LexType::RPARENTHESIS))
	{
		pd->mChilds.push_back(GetMatchedTerminal(t));
	}

	t = GetCurToken();

	if (Match(LexType::SEMICOLON))
	{
		pd->mChilds.push_back(GetMatchedTerminal(t));
	}
	pd->mChilds.push_back(ProcDecPart());
	pd->mChilds.push_back(ProcBody());
	pd->mChilds.push_back(ProcDecMore());
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[ProcDeclaration] Finished")));
	return pd;
}


/* 书上算法少了此条产生式 */
// ProcDecMore ::=
//					ε { BEGIN }
//					| ProcDeclaration {PROCEDURE}
RTreeNode* RSyntaxParser::ProcDecMore()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [ProcDecMore]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing ProcDecMore in line %d"), mCurLine));
	RTreeNode* pdm = new RTreeNode();
	pdm->mNodeType = NodeType::ProcDecMore;
	pdm->mLine = mCurLine;

	if (GetCurToken().lex == LexType::BEGIN)
	{
		RTreeNode* n = GetMatchedTerminal(Token(LexType::EPSILON));
		pdm->mChilds.push_back(n);
		mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[ProcDecMore] is EPSILON")));
		LogUtil::Info(Utils::FormatCString(_T("ProcDecMore is EPSILON near line %d"), mCurLine));
	}
	else if (GetCurToken().lex == LexType::PROCEDURE)
	{
		pdm->mChilds.push_back(ProcDeclaration());
	}
	else
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[ProcDecMore] Finished")));
	return pdm;
}

// ProcDecPart ::= DeclarePart
RTreeNode* RSyntaxParser::ProcDecPart()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [ProcDecPart]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing ProcDecPart in line %d"), mCurLine));
	RTreeNode* pdp = new RTreeNode();
	pdp->mNodeType = NodeType::ProcDecPart;
	pdp->mLine = mCurLine;
	
	pdp->mChilds.push_back(DeclarePart());

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[ProcDecPart] Finished")));
	return pdp;
}

// ProcBody ::= ProgramBody
RTreeNode* RSyntaxParser::ProcBody()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [ProcBody]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing ProcBody in line %d"), mCurLine));
	RTreeNode* pb = new RTreeNode();
	pb->mNodeType = NodeType::ProcBody;
	pb->mLine = mCurLine;

	pb->mChilds.push_back(ProgramBody());

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[ProcBody] Finished")));
	return pb;
}

// ParamList :: = 
//				ε { RPAREN }
//				| ParamDecList { INTEGER, CHAR, ARRAY, RECORD, ID, VAR }
RTreeNode* RSyntaxParser::ParamList()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [ParamList]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing ParamList in line %d"), mCurLine));
	RTreeNode* pl = new RTreeNode();
	pl->mNodeType = NodeType::ParamList;
	pl->mLine = mCurLine;


	if (GetCurToken().lex == LexType::RPARENTHESIS)	// 向前看 1 个
	{
		RTreeNode* n = GetMatchedTerminal(Token(LexType::EPSILON));
		pl->mChilds.push_back(n);
		mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[ParamList] is EPSILON")));
		LogUtil::Info(Utils::FormatCString(_T("ParamList is EPSILON near line %d"), mCurLine));
	}
	else if (
		GetCurToken().lex == LexType::INTEGER || GetCurToken().lex == LexType::CHARACTER 
		|| GetCurToken().lex == LexType::ARRAY || GetCurToken().lex == LexType::RECORD
		|| GetCurToken().lex == LexType::IDENTIFIER || GetCurToken().lex == LexType::VAR
		)
	{
		pl->mChilds.push_back(ParamDecList());
	}
	else
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[ParamList] Finished")));
	return pl;
}


// ParamDecList ::= Param ParamMore
RTreeNode* RSyntaxParser::ParamDecList()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [ParamDecList]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing ParamDecList in line %d"), mCurLine));
	RTreeNode* vd = new RTreeNode();
	vd->mNodeType = NodeType::ParamDecList;
	vd->mLine = mCurLine;
	vd->mChilds.push_back(Param());
	vd->mChilds.push_back(ParamMore());

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[ParamDecList] Finished")));
	return vd;
}


// ParamMore ::= 
//				ε { RPAREN }
//				| T-<SEMICOLON> ParamDecList { SEMICOLON }
RTreeNode* RSyntaxParser::ParamMore()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [ParamMore]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing ParamMore in line %d"), mCurLine));
	RTreeNode* pm = new RTreeNode();
	pm->mNodeType = NodeType::ParamMore;
	pm->mLine = mCurLine;
	
	if (GetCurToken().lex == LexType::RPARENTHESIS)	// 向前看 1 个
	{
		RTreeNode* n = GetMatchedTerminal(Token(LexType::EPSILON));
		pm->mChilds.push_back(n);
		mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[ParamMore] is EPSILON")));
		LogUtil::Info(Utils::FormatCString(_T("ParamMore is EPSILON near line %d"), mCurLine));
	}
	else if (GetCurToken().lex == LexType::SEMICOLON)	// 向前看 1 个
	{
		Token t = GetCurToken();
		if (Match(LexType::SEMICOLON))
		{
			pm->mChilds.push_back(GetMatchedTerminal(t));
		}
		pm->mChilds.push_back(ParamDecList());
	}
	else
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[ParamMore] Finished")));
	return pm;
}

// Param ::= 
//			TypeDef FormList { INTEGER, CHARARCTER, RECORD, ARRAY, IDENTIFIER }
//			| T-<VAR> TypeDef FormList { VAR }
RTreeNode* RSyntaxParser::Param()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [Param]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing Param in line %d"), mCurLine));
	RTreeNode* p = new RTreeNode();
	p->mNodeType = NodeType::Param;
	p->mLine = mCurLine;
	
	if (
		GetCurToken().lex == LexType::INTEGER || GetCurToken().lex == LexType::CHARACTER
		|| GetCurToken().lex == LexType::RECORD || GetCurToken().lex == LexType::ARRAY
		|| GetCurToken().lex == LexType::IDENTIFIER)	// 向前看 1 个
	{
		p->mChilds.push_back(TypeDef());
		p->mChilds.push_back(FormList());
	}
	else if (GetCurToken().lex == LexType::VAR)
	{
		Token t = GetCurToken();

		if (Match(LexType::VAR))
		{
			p->mChilds.push_back(GetMatchedTerminal(t));
		}
		p->mChilds.push_back(TypeDef());
		p->mChilds.push_back(FormList());
	}
	else
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[Param] Finished")));
	return p;
}


// FormList ::= T-<IDENTIFIER> FidMore
RTreeNode* RSyntaxParser::FormList()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [FormList]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing FormList in line %d"), mCurLine));
	RTreeNode* fl = new RTreeNode();
	fl->mNodeType = NodeType::FormList;
	fl->mLine = mCurLine;

	if (GetCurToken().lex == LexType::IDENTIFIER)	// 向前看 1 个
	{
		Token t = GetCurToken();
		if (Match(LexType::IDENTIFIER))
		{
			fl->mChilds.push_back(GetMatchedTerminal(t));
		}
	}
	fl->mChilds.push_back(FidMore());

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[FormList] Finished")));
	return fl;
}


// FidMore ::= 
//				ε { SEMICOLON, RPARENTHESIS }
//				| T-<COMMA> FormList { COMMA }
RTreeNode* RSyntaxParser::FidMore()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [FidMore]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing FidMore in line %d"), mCurLine));
	RTreeNode* fm = new RTreeNode();
	fm->mNodeType = NodeType::FidMore;
	fm->mLine = mCurLine;

	if (GetCurToken().lex == LexType::SEMICOLON || GetCurToken().lex == LexType::RPARENTHESIS)	// 向前看 1 个
	{
		RTreeNode* n = GetMatchedTerminal(Token(LexType::EPSILON));
		fm->mChilds.push_back(n);
		mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[FidMore] is EPSILON")));
		LogUtil::Info(Utils::FormatCString(_T("FidMore is EPSILON near line %d"), mCurLine));
	}
	else if (GetCurToken().lex == LexType::COMMA)	// 向前看 1 个
	{
		Token t = GetCurToken();
		if (Match(LexType::COMMA))
		{
			fm->mChilds.push_back(GetMatchedTerminal(t));
		}
		fm->mChilds.push_back(FormList());
	}
	else
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[FidMore] Finished")));
	return fm;
}

// VarDec ::=
//				ε { PROCEDURE, BEGIN }
//				| VarDeclaration { VAR }
RTreeNode* RSyntaxParser::VarDec()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [VarDec]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing VarDec in line %d"), mCurLine));
	RTreeNode* vd = new RTreeNode();
	vd->mNodeType = NodeType::VarDec;
	vd->mLine = mCurLine;

	
	if (GetCurToken().lex == LexType::PROCEDURE || GetCurToken().lex == LexType::BEGIN)	// 向前看 1 个
	{
		RTreeNode* n = GetMatchedTerminal(Token(LexType::EPSILON));
		vd->mChilds.push_back(n);
		mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[VarDec] is EPSILON")));
		LogUtil::Info(Utils::FormatCString(_T("VarDec is EPSILON near line %d"), mCurLine));
	}
	else if (GetCurToken().lex == LexType::VAR)
	{
		vd->mChilds.push_back(VarDeclaration());
	}
	else
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[VarDec] Finished")));
	return vd;
}


// VarDeclaration ::= T-<VAR> VarDecList
RTreeNode* RSyntaxParser::VarDeclaration()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [VarDeclaration]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing VarDeclaration in line %d"), mCurLine));
	RTreeNode* vd = new RTreeNode();
	vd->mNodeType = NodeType::VarDeclaration;
	vd->mLine = mCurLine;

	Token t = GetCurToken();

	if (Match(LexType::VAR))
	{
		vd->mChilds.push_back(GetMatchedTerminal(t));

		RTreeNode* vdl = VarDecList();
		if (vdl == NULL)
			LogUtil::Error(Utils::FormatCString(_T("Missing VarDecList near line %d"), mCurLine));
		vd->mChilds.push_back(vdl);
	}
	else
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[VarDeclaration] Finished")));
	return vd;
}

// VarDecList ::= TypeDef VarIdList T-<SEMICOLON> VarDecMore
RTreeNode* RSyntaxParser::VarDecList()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [VarDecList]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing VarDecList in line %d"), mCurLine));
	RTreeNode* vdl = new RTreeNode();
	vdl->mNodeType = NodeType::VarDecList;
	vdl->mLine = mCurLine;

	vdl->mChilds.push_back(TypeDef());
	vdl->mChilds.push_back(VarIdList());

	Token t = GetCurToken();

	if (Match(LexType::SEMICOLON))
	{
		vdl->mChilds.push_back(GetMatchedTerminal(t));
		vdl->mChilds.push_back(VarDecMore());
	}
	else
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[VarDecList] Finished")));
	return vdl;
}

// VarIdMore :: = 
//				ε { SEMICOLON }
//				| T-<COMMA> VarIdList { COMMA }
RTreeNode* RSyntaxParser::VarIdMore()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [VarIdMore]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing VarIdMore in line %d"), mCurLine));
	RTreeNode* vim = new RTreeNode();
	vim->mNodeType = NodeType::VarIdMore;
	vim->mLine = mCurLine;

	if (GetCurToken().lex == LexType::SEMICOLON)	// 向前看 1 个
	{
		RTreeNode* n = GetMatchedTerminal(Token(LexType::EPSILON));
		vim->mChilds.push_back(n);
		mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[VarIdMore] is EPSILON")));
		LogUtil::Info(Utils::FormatCString(_T("VarIdMore is EPSILON near line %d"), mCurLine));
	}
	else if (GetCurToken().lex == LexType::COMMA)
	{
		Token t = GetCurToken();

		if (Match(LexType::COMMA))
		{
			vim->mChilds.push_back(GetMatchedTerminal(t));
			vim->mChilds.push_back(VarIdList());
		}
	}
	else
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[VarIdMore] Finished")));
	return vim;
}


// VarIdList ::= T-<IDENTIFIER> VarIdMore
RTreeNode* RSyntaxParser::VarIdList()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [VarIdList]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing VarIdList in line %d"), mCurLine));
	RTreeNode* vil = new RTreeNode();
	vil->mNodeType = NodeType::VarIdList;
	vil->mLine = mCurLine;

	Token t = GetCurToken();

	if (Match(LexType::IDENTIFIER))
	{
		vil->mChilds.push_back(GetMatchedTerminal(t));
	}

	vil->mChilds.push_back(VarIdMore());

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[VarIdList] Finished")));
	return vil;
}

// VarDecMore ::= 
//				ε { PROCEDURE, BEGIN }
//				| VarDecList { INTEGER, CHARACTER, RECORD, ARRAY, IDENTIFIER }
RTreeNode* RSyntaxParser::VarDecMore()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [VarDecMore]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing VarDecMore in line %d"), mCurLine));
	RTreeNode* vdm = new RTreeNode();
	vdm->mNodeType = NodeType::VarDecMore;
	vdm->mLine = mCurLine;

	if (GetCurToken().lex == LexType::PROCEDURE || GetCurToken().lex == LexType::BEGIN)	// 向前看 1 个
	{
		RTreeNode* n = GetMatchedTerminal(Token(LexType::EPSILON));
		vdm->mChilds.push_back(n);
		mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[VarDecMore] is EPSILON")));
		LogUtil::Info(Utils::FormatCString(_T("VarDecMore is EPSILON near line %d"), mCurLine));
	}
	else if (
		GetCurToken().lex == LexType::INTEGER || GetCurToken().lex == LexType::CHARACTER 
		|| GetCurToken().lex == LexType::RECORD || GetCurToken().lex == LexType::ARRAY
		|| GetCurToken().lex == LexType::IDENTIFIER)
	{
		vdm->mChilds.push_back(VarDecList());
	}
	else
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[VarDecMore] Finished")));
	return vdm;
}


// TypeDec ::= 
//			ε Expected {VAR, PROCEDURE, BEGIN}
//			| TypeDeclaration Expected { TYPE }
RTreeNode* RSyntaxParser::TypeDec()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [TypeDec]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing TypeDec in line %d"), mCurLine));
	RTreeNode* td = new RTreeNode();
	td->mNodeType = NodeType::TypeDec;
	td->mLine = mCurLine;

	if (GetCurToken().lex == LexType::TYPE)	// 向前看 1 个
	{
		td->mChilds.push_back(TypeDeclaration());
	}
	else if (GetCurToken().lex == LexType::VAR || GetCurToken().lex == LexType::PROCEDURE || GetCurToken().lex == LexType::BEGIN)
	{
		RTreeNode* n = GetMatchedTerminal(Token(LexType::EPSILON));
		td->mChilds.push_back(n);
		mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[TypeDec] is EPSILON")));
		LogUtil::Info(Utils::FormatCString(_T("TypeDec is EPSILON near line %d"), mCurLine));
	}
	else
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[TypeDec] Finished")));
	return td;
}


// TypeDeclaration ::= T-<TYPE> TypeDecList
RTreeNode* RSyntaxParser::TypeDeclaration()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [TypeDeclaration]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing TypeDeclaration in line %d"), mCurLine));
	RTreeNode* td = new RTreeNode();
	td->mNodeType = NodeType::TypeDeclaration;
	td->mLine = mCurLine;

	Token t = GetCurToken();

	if (Match(LexType::TYPE))
	{
		td->mChilds.push_back(GetMatchedTerminal(t));


		RTreeNode* tdl = TypeDecList();
		if (tdl == NULL)
			LogUtil::Error(Utils::FormatCString(_T("Missing TypeDecList near line %d"), mCurLine));
		td->mChilds.push_back(tdl);
	}
	else
	{
		//LogUtil::Error(Utils::FormatCString(_T("Missing <TYPE> near line %d"), mCurLine));
	}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[TypeDeclaration] Finished")));
	return td;
}


// 
// TypeDecList ::= TypeId T-<EQU> TypeDef T-<SEMICOLON> TypeDecMore
RTreeNode* RSyntaxParser::TypeDecList()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [TypeDecList]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing TypeDecList in line %d"), mCurLine));
	RTreeNode* tdl = new RTreeNode();
	tdl->mNodeType = NodeType::TypeDecList;
	tdl->mLine = mCurLine;

	RTreeNode* ti = TypeId();
	tdl->mChilds.push_back(ti);
	if (ti == NULL)
	{
		LogUtil::Error(Utils::FormatCString(_T("Missing TypeId near line %d"), mCurLine));
	}

	Token t = GetCurToken();

	if (Match(LexType::EQU))
	{
		tdl->mChilds.push_back(GetMatchedTerminal(t));
	}

	RTreeNode* tdf = TypeDef();
	tdl->mChilds.push_back(tdf);
	if (tdf == NULL)
	{
		LogUtil::Error(Utils::FormatCString(_T("Missing TypeDef near line %d"), mCurLine));
	}

	t = GetCurToken();

	if (Match(LexType::SEMICOLON))
	{
		tdl->mChilds.push_back(GetMatchedTerminal(t));
	}

	RTreeNode* tdm = TypeDecMore();
	tdl->mChilds.push_back(tdm);
	if (tdm == NULL)
	{
		LogUtil::Error(Utils::FormatCString(_T("Missing TypeDecMore near line %d"), mCurLine));
	}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[TypeDecList] Finished")));
	return tdl;
}


// TypeId :: = T-<IDENTIFIER>
RTreeNode* RSyntaxParser::TypeId()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [TypeId]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing TypeId in line %d"), mCurLine));
	RTreeNode* ti = new RTreeNode();
	ti->mNodeType = NodeType::TypeId;
	ti->mLine = mCurLine;
	Token t = GetCurToken();

	if (Match(LexType::IDENTIFIER))
	{
		ti->mChilds.push_back(GetMatchedTerminal(t));
	}
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[TypeId] Finished")));
	return ti;
}


// TypeDef ::=
//			BaseType { INTEGER, CHARACTER }
//			| StructureType { ARRAY, RECORD }
//			| T-<IDENTIFIER> { IDENTIFIER }
RTreeNode* RSyntaxParser::TypeDef()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [TypeDef]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing TypeDef in line %d"), mCurLine));
	RTreeNode* tdf = new RTreeNode();
	tdf->mNodeType = NodeType::TypeDef;
	tdf->mLine = mCurLine;
	
	if (GetCurToken().lex == LexType::INTEGER || GetCurToken().lex == LexType::CHARACTER)	// 向前看 1 个
	{
		tdf->mChilds.push_back(BaseType());
	}
	else if (GetCurToken().lex == LexType::ARRAY || GetCurToken().lex == LexType::RECORD)
	{
		tdf->mChilds.push_back(StructureType());
	}
	else if (GetCurToken().lex == LexType::IDENTIFIER)
	{
		Token t = GetCurToken();

		if (Match(LexType::IDENTIFIER))
		{
			tdf->mChilds.push_back(GetMatchedTerminal(t));
		}
	}
	else
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[TypeDef] Finished")));
	return tdf;
}


// BaseType ::=
//				T-<INTEGER> { INTEGER }
//				| T-<CHAR> { CHARACTER }
RTreeNode* RSyntaxParser::BaseType()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [BaseType]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing BaseType in line %d"), mCurLine));
	RTreeNode* bt = new RTreeNode();
	bt->mNodeType = NodeType::BaseType;
	bt->mLine = mCurLine;

	if (GetCurToken().lex == LexType::INTEGER)	// 向前看 1 个
	{
		Token t = GetCurToken();

		if (Match(LexType::INTEGER))
		{
			bt->mChilds.push_back(GetMatchedTerminal(t));
		}
	}
	else if (GetCurToken().lex == LexType::CHARACTER)
	{
		Token t = GetCurToken();

		if (Match(LexType::CHARACTER))
		{
			bt->mChilds.push_back(GetMatchedTerminal(t));
		}
	}
	else
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[BaseType] Finished")));
	return bt;
}


// StructureType ::=
//					ArrayType { ARRAY }
//					| RecType { RECORD }
RTreeNode* RSyntaxParser::StructureType()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [StructureType]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing StructureType in line %d"), mCurLine));
	RTreeNode* st = new RTreeNode();
	st->mNodeType = NodeType::StructureType;
	st->mLine = mCurLine;

	if (GetCurToken().lex == LexType::ARRAY)	// 向前看 1 个
	{
		st->mChilds.push_back(ArrayType());
	}
	else if (GetCurToken().lex == LexType::RECORD)
	{
		st->mChilds.push_back(RecType());
	}
	else
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[StructureType] Finished")));
	return st;
}


// ArrayType ::= T-<ARRAY> T-<[low..top]> T-<OF> BaseType
RTreeNode* RSyntaxParser::ArrayType()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [ArrayType]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing ArrayType in line %d"), mCurLine));
	RTreeNode* at = new RTreeNode();
	at->mNodeType = NodeType::ArrayType;
	at->mLine = mCurLine;

	Token t = GetCurToken();

	if (Match(LexType::ARRAY))
	{
		at->mChilds.push_back(GetMatchedTerminal(t));
	}

	t = GetCurToken();

	if (Match(LexType::LSQUAREBRACKET))
	{
		at->mChilds.push_back(GetMatchedTerminal(t));
	}
	
	// TODO: Distinct lower bound and upper bound
	t = GetCurToken();

	if (Match(LexType::UINTEGER))
	{
		at->mChilds.push_back(GetMatchedTerminal(t));
	}

	t = GetCurToken();

	if (Match(LexType::UNDERANGE))
	{
		at->mChilds.push_back(GetMatchedTerminal(t));
	}

	t = GetCurToken();

	if (Match(LexType::UINTEGER))
	{
		at->mChilds.push_back(GetMatchedTerminal(t));
	}

	t = GetCurToken();

	if (Match(LexType::RSQUAREBRACKET))
	{
		at->mChilds.push_back(GetMatchedTerminal(t));
	}

	t = GetCurToken();

	if (Match(LexType::OF))
	{
		at->mChilds.push_back(GetMatchedTerminal(t));
	}

	at->mChilds.push_back(BaseType());

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[ArrayType] Finished")));
	return at;
}


// RecType ::= T-<RECORD> FieldDecList T-<END>
RTreeNode* RSyntaxParser::RecType()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [RecType]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing RecType in line %d"), mCurLine));
	RTreeNode* rt = new RTreeNode();
	rt->mNodeType = NodeType::RecType;
	rt->mLine = mCurLine;

	Token t = GetCurToken();

	if (Match(LexType::RECORD))
	{
		rt->mChilds.push_back(GetMatchedTerminal(t));
	}

	rt->mChilds.push_back(FieldDecList());

	t = GetCurToken();

	if (Match(LexType::END))
	{
		rt->mChilds.push_back(GetMatchedTerminal(t));
	}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[RecType] Finished")));
	return rt;
}


// FieldDecList ::=
//				BaseType IdList T-<SEMICOLON> FieldDecMore { INTEGER, CHARACTER }
//				| ArrayType IdList T-<SEMICOLON> FieldDecMore { ARRAY }
RTreeNode* RSyntaxParser::FieldDecList()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [FieldDecList]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing FieldDecList in line %d"), mCurLine));
	RTreeNode* fd = new RTreeNode();
	fd->mNodeType = NodeType::FieldDecList;
	fd->mLine = mCurLine;

	if (GetCurToken().lex == LexType::INTEGER || GetCurToken().lex == LexType::CHARACTER)	// 向前看 1 个
	{
		fd->mChilds.push_back(BaseType());
		fd->mChilds.push_back(IdList());

		Token t = GetCurToken();

		if (Match(LexType::SEMICOLON))
		{
			fd->mChilds.push_back(GetMatchedTerminal(t));
		}

		fd->mChilds.push_back(FieldDecMore());
	}
	else if (GetCurToken().lex == LexType::ARRAY)
	{
		fd->mChilds.push_back(ArrayType());
		fd->mChilds.push_back(IdList());

		Token t = GetCurToken();

		if (Match(LexType::SEMICOLON))
		{
			fd->mChilds.push_back(GetMatchedTerminal(t));
		}

		fd->mChilds.push_back(FieldDecMore());
	}
	else
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[FieldDecList] Finished")));
	return fd;
}


// IdList ::= T-<IDENTIFIER> IdMore
RTreeNode* RSyntaxParser::IdList()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [IdList]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing IdList in line %d"), mCurLine));
	RTreeNode* idl = new RTreeNode();
	idl->mNodeType = NodeType::IdList;
	idl->mLine = mCurLine;

	Token t = GetCurToken();

	if (Match(LexType::IDENTIFIER))
	{
		idl->mChilds.push_back(GetMatchedTerminal(t));
	}

	idl->mChilds.push_back(IdMore());

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[IdList] Finished")));
	return idl;
}


// IdMore ::= 
//			ε { SEMICOLON }
//			| T-<COMMA> IdList { COMMA }
RTreeNode* RSyntaxParser::IdMore()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [IdMore]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing IdMore in line %d"), mCurLine));
	RTreeNode* idm = new RTreeNode();
	idm->mNodeType = NodeType::IdMore;
	idm->mLine = mCurLine;

	if (GetCurToken().lex == LexType::SEMICOLON)	// 向前看 1 个
	{
		RTreeNode* n = GetMatchedTerminal(Token(LexType::EPSILON));
		idm->mChilds.push_back(n);
		mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[IdMore] is EPSILON")));
		LogUtil::Info(Utils::FormatCString(_T("IdMore is EPSILON near line %d"), mCurLine));
	}
	else if (GetCurToken().lex == LexType::COMMA)
	{
		idm->mChilds.push_back(IdList());
	}
	else
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[IdMore] Finished")));
	return idm;
}


// FieldDecMore ::= 
//					ε { END }
//					| FieldDecList { INTEGER, CHARACTER, ARRAY }
RTreeNode* RSyntaxParser::FieldDecMore()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [FieldDecMore]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing FieldDecMore in line %d"), mCurLine));
	RTreeNode* fdm = new RTreeNode();
	fdm->mNodeType = NodeType::FieldDecMore;
	fdm->mLine = mCurLine;

	if (GetCurToken().lex == LexType::END)	// 向前看 1 个
	{
		RTreeNode* n = GetMatchedTerminal(Token(LexType::EPSILON));
		fdm->mChilds.push_back(n);
		mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[FieldDecMore] is EPSILON")));
		LogUtil::Info(Utils::FormatCString(_T("FieldDecMore is EPSILON near line %d"), mCurLine));
	}
	else if (GetCurToken().lex == LexType::INTEGER || GetCurToken().lex == LexType::CHARACTER || GetCurToken().lex == LexType::ARRAY)
	{
		fdm->mChilds.push_back(FieldDecList());
	}
	else
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[FieldDecMore] Finished")));
	return fdm;
}



// TypeDecMore ::= 
//					ε { VAR, PROCEDURE, BEGIN }
//					| TypeDecList { IDENTIFIER }
RTreeNode* RSyntaxParser::TypeDecMore()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [TypeDecMore]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing TypeDecMore in line %d"), mCurLine));
	RTreeNode* tdm = new RTreeNode();
	tdm->mNodeType = NodeType::TypeDecMore;
	tdm->mLine = mCurLine;

	if (GetCurToken().lex == LexType::VAR || GetCurToken().lex == LexType::PROCEDURE || GetCurToken().lex == LexType::BEGIN)	// 向前看 1 个
	{
		RTreeNode* n = GetMatchedTerminal(Token(LexType::EPSILON));
		tdm->mChilds.push_back(n);
		mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[TypeDecMore] is EPSILON")));
		LogUtil::Info(Utils::FormatCString(_T("TypeDecMore is EPSILON near line %d"), mCurLine));
	}
	else if (GetCurToken().lex == LexType::IDENTIFIER)
	{
		tdm->mChilds.push_back(TypeDecList());
	}
	else
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[TypeDecMore] Finished")));
	return tdm;
}


// StmList ::= Stm StmMore
RTreeNode* RSyntaxParser::StmList()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [StmList]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing StmList in line %d"), mCurLine));
	RTreeNode* sl = new RTreeNode();
	sl->mNodeType = NodeType::StmList;
	sl->mLine = mCurLine;

	sl->mChilds.push_back(Stm());
	sl->mChilds.push_back(StmMore());

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[StmList] Finished")));
	return sl;
}


/* 书上算法37：StmMore ::= ε 的 Preict 集少了 ELSE、FI */
// StmMore ::=
//				ε { ELSE, FI, END, ENDWHILE }
//				| T-<SEMICOLON> StmList { SEMICOLON }
RTreeNode* RSyntaxParser::StmMore()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [StmMore]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing StmMore in line %d"), mCurLine));
	RTreeNode* sm = new RTreeNode();
	sm->mNodeType = NodeType::StmMore;
	sm->mLine = mCurLine;

	if (
		GetCurToken().lex == LexType::END || GetCurToken().lex == LexType::ENDWHILE
		|| GetCurToken().lex == LexType::ELSE || GetCurToken().lex == LexType::FI)	// 向前看 1 个
	{
		RTreeNode* n = GetMatchedTerminal(Token(LexType::EPSILON));
		sm->mChilds.push_back(n);
		mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[StmMore] is EPSILON")));
		LogUtil::Info(Utils::FormatCString(_T("StmMore is EPSILON near line %d"), mCurLine));
	}
	else if (GetCurToken().lex == LexType::SEMICOLON)
	{
		Token t = GetCurToken();

		if (Match(LexType::SEMICOLON))
		{
			sm->mChilds.push_back(GetMatchedTerminal(t));
		}
		sm->mChilds.push_back(StmList());
	}
	else
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[StmMore] Finished")));
	return sm;
}

// Stm ::=
//			ConditionalStm { IF }
//			| LoopStm { WHILE }
//			| InputStm { READ }
//			| OutputStm { WRITE }
//			| ReturnStm { RETURN }
//			| T-<IDENTIFIER> AssCall { IDENTIFIER }
RTreeNode* RSyntaxParser::Stm()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [Stm]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing Stm in line %d"), mCurLine));
	RTreeNode* s = new RTreeNode();
	s->mNodeType = NodeType::Stm;
	s->mLine = mCurLine;

	if (GetCurToken().lex == LexType::IF)	// 向前看 1 个
	{
		s->mChilds.push_back(ConditionalStm());
	}
	else if (GetCurToken().lex == LexType::WHILE)
	{
		s->mChilds.push_back(LoopStm());
	}
	else if (GetCurToken().lex == LexType::READ)
	{
		s->mChilds.push_back(InputStm());
	}
	else if (GetCurToken().lex == LexType::WRITE)
	{
		s->mChilds.push_back(OutputStm());
	}
	else if (GetCurToken().lex == LexType::RETURN)
	{
		s->mChilds.push_back(ReturnStm());
	}
	else if (GetCurToken().lex == LexType::IDENTIFIER)
	{
		Token t = GetCurToken();

		if (Match(LexType::IDENTIFIER))
		{
			s->mChilds.push_back(GetMatchedTerminal(t));
		}
		s->mChilds.push_back(AssCall());
	}
	else
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[Stm] Finished")));
	return s;
}

/* 书上产生式67：AssCall ::= AssignmentRest 的 Predict 集少了 LSQUAREBRACKET、DOT */
/* 算法39：AssCall ::= CallStmRest Predict 集应为 { LPARENTHESIS } 而不是 { RPARENTHESIS } */
// AssCall ::= 
//				AssignmentRest { ASSIGN, LSQUAREBRACKET, DOT }
//				| CallStmRest { LPARENTHESIS }
RTreeNode* RSyntaxParser::AssCall()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [AssCall]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing AssCall in line %d"), mCurLine));
	RTreeNode* ac = new RTreeNode();
	ac->mNodeType = NodeType::AssCall;
	ac->mLine = mCurLine;

	if (GetCurToken().lex == LexType::ASSIGN || GetCurToken().lex == LexType::LSQUAREBRACKET || GetCurToken().lex == LexType::DOT)	// 向前看 1 个
	{
		ac->mChilds.push_back(AssignmentRest());
	}
	else if (GetCurToken().lex == LexType::LPARENTHESIS)
	{
		ac->mChilds.push_back(CallStmRest());
	}
	else
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[AssCall] Finished")));
	return ac;
}


/* 书上算法40：产生式少了 VariMore */
// AssignmentRest ::= VariMore T-<ASSIGN> Exp
RTreeNode* RSyntaxParser::AssignmentRest()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [AssignmentRest]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing AssignmentRest in line %d"), mCurLine));
	RTreeNode* ar = new RTreeNode();
	ar->mNodeType = NodeType::AssignmentRest;
	ar->mLine = mCurLine;

	ar->mChilds.push_back(VariMore());

	Token t = GetCurToken();

	if (Match(LexType::ASSIGN))
	{
		ar->mChilds.push_back(GetMatchedTerminal(t));
	}
	ar->mChilds.push_back(Exp());

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[AssignmentRest] Finished")));
	return ar;
}


/* 书上算法41：Exp 应改为 RelExp，StmL 应改为 StmList */
// ConditionalStm :: = T-<IF> RelExp T-<THEN> StmList T-<ELSE> StmList T-<FI>
RTreeNode* RSyntaxParser::ConditionalStm()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [ConditionalStm]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing ConditionalStm in line %d"), mCurLine));
	RTreeNode* cs = new RTreeNode();
	cs->mNodeType = NodeType::ConditionalStm;
	cs->mLine = mCurLine;

	Token t = GetCurToken();

	if (Match(LexType::IF))
	{
		cs->mChilds.push_back(GetMatchedTerminal(t));
	}

	cs->mChilds.push_back(RelExp());

	t = GetCurToken();

	if (Match(LexType::THEN))
	{
		cs->mChilds.push_back(GetMatchedTerminal(t));
	}

	cs->mChilds.push_back(StmList());

	t = GetCurToken();

	if (Match(LexType::ELSE))
	{
		cs->mChilds.push_back(GetMatchedTerminal(t));
	}

	cs->mChilds.push_back(StmList());

	t = GetCurToken();

	if (Match(LexType::FI))
	{
		cs->mChilds.push_back(GetMatchedTerminal(t));
	}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[ConditionalStm] Finished")));
	return cs;
}


/* 书上算法42：Exp 应改为 RelExp */
// LoopStm :: = T-<WHILE> RelExp T-<DO> StmList T-<ENDWHILE>
RTreeNode* RSyntaxParser::LoopStm()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [LoopStm]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing LoopStm in line %d"), mCurLine));
	RTreeNode* ls = new RTreeNode();
	ls->mNodeType = NodeType::LoopStm;
	ls->mLine = mCurLine;

	Token t = GetCurToken();

	if (Match(LexType::WHILE))
	{
		ls->mChilds.push_back(GetMatchedTerminal(t));
	}

	ls->mChilds.push_back(RelExp());

	t = GetCurToken();

	if (Match(LexType::DO))
	{
		ls->mChilds.push_back(GetMatchedTerminal(t));
	}

	ls->mChilds.push_back(StmList());

	t = GetCurToken();

	if (Match(LexType::ENDWHILE))
	{
		ls->mChilds.push_back(GetMatchedTerminal(t));
	}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[LoopStm] Finished")));
	return ls;
}

// InputStm ::= T-<READ> T-<LPARENTHESIS> T-<IDENTIFIER> T-<RPARENTHESIS>
RTreeNode* RSyntaxParser::InputStm()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [InputStm]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing InputStm in line %d"), mCurLine));
	RTreeNode* is = new RTreeNode();
	is->mNodeType = NodeType::InputStm;
	is->mLine = mCurLine;

	Token t = GetCurToken();

	if (Match(LexType::READ))
	{
		is->mChilds.push_back(GetMatchedTerminal(t));
	}

	t = GetCurToken();

	if (Match(LexType::LPARENTHESIS))
	{
		is->mChilds.push_back(GetMatchedTerminal(t));
	}

	t = GetCurToken();

	if (Match(LexType::IDENTIFIER))
	{
		is->mChilds.push_back(GetMatchedTerminal(t));
	}

	t = GetCurToken();

	if (Match(LexType::RPARENTHESIS))
	{
		is->mChilds.push_back(GetMatchedTerminal(t));
	}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[InputStm] Finished")));
	return is;
}


// OutputStm ::= T-<WRITE> T-<LPARENTHESIS> Exp T-<RPARENTHESIS>
RTreeNode* RSyntaxParser::OutputStm()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [OutputStm]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing OutputStm in line %d"), mCurLine));
	RTreeNode* os = new RTreeNode();
	os->mNodeType = NodeType::OutputStm;
	os->mLine = mCurLine;

	Token t = GetCurToken();

	if (Match(LexType::WRITE))
	{
		os->mChilds.push_back(GetMatchedTerminal(t));
	}

	t = GetCurToken();

	if (Match(LexType::LPARENTHESIS))
	{
		os->mChilds.push_back(GetMatchedTerminal(t));
	}

	os->mChilds.push_back(Exp());

	t = GetCurToken();

	if (Match(LexType::RPARENTHESIS))
	{
		os->mChilds.push_back(GetMatchedTerminal(t));
	}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[OutputStm] Finished")));
	return os;
}


// ReturnStm ::= T-<RETURN>
RTreeNode* RSyntaxParser::ReturnStm()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [ReturnStm]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing ReturnStm in line %d"), mCurLine));
	RTreeNode* rs = new RTreeNode();
	rs->mNodeType = NodeType::ReturnStm;
	rs->mLine = mCurLine;

	Token t = GetCurToken();

	if (Match(LexType::RETURN))
	{
		rs->mChilds.push_back(GetMatchedTerminal(t));
	}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[ReturnStm] Finished")));
	return rs;
}


// CallStmRest ::= T-<LPARENTHESIS> ActParamList T-<RPARENTHESIS>
RTreeNode* RSyntaxParser::CallStmRest()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [CallStmRest]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing CallStmRest in line %d"), mCurLine));
	RTreeNode* cs = new RTreeNode();
	cs->mNodeType = NodeType::CallStmRest;
	cs->mLine = mCurLine;

	Token t = GetCurToken();

	if (Match(LexType::LPARENTHESIS))
	{
		cs->mChilds.push_back(GetMatchedTerminal(t));
	}

	cs->mChilds.push_back(ActParamList());

	t = GetCurToken();

	if (Match(LexType::RPARENTHESIS))
	{
		cs->mChilds.push_back(GetMatchedTerminal(t));
	}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[CallStmRest] Finished")));
	return cs;
}


// ActParamList ::=
//				ε { RPARENTHESIS }
//				| Exp ActParamMore { IDENTIFIER, UINTEGER }
RTreeNode* RSyntaxParser::ActParamList()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [ActParamList]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing ActParamList in line %d"), mCurLine));
	RTreeNode* apl = new RTreeNode();
	apl->mNodeType = NodeType::ActParamList;
	apl->mLine = mCurLine;

	if (GetCurToken().lex == LexType::RPARENTHESIS)	// 向前看 1 个
	{
		RTreeNode* n = GetMatchedTerminal(Token(LexType::EPSILON));
		apl->mChilds.push_back(n);
		mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[ActParamList] is EPSILON")));
		LogUtil::Info(Utils::FormatCString(_T("ActParamList is EPSILON near line %d"), mCurLine));
	}
	else if (GetCurToken().lex == LexType::IDENTIFIER || GetCurToken().lex == LexType::UINTEGER)
	{
		apl->mChilds.push_back(Exp());
		apl->mChilds.push_back(ActParamMore());
	}
	else
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[ActParamList] Finished")));
	return apl;
}


// ActParamMore ::=
//				ε { RPARENTHESIS }
//				| T-<COMMA> ActParamList { COMMA }
RTreeNode* RSyntaxParser::ActParamMore()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [ActParamMore]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing ActParamMore in line %d"), mCurLine));
	RTreeNode* apm = new RTreeNode();
	apm->mNodeType = NodeType::ActParamMore;
	apm->mLine = mCurLine;

	if (GetCurToken().lex == LexType::RPARENTHESIS)	// 向前看 1 个
	{
		RTreeNode* n = GetMatchedTerminal(Token(LexType::EPSILON));
		apm->mChilds.push_back(n);
		mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[ActParamMore] is EPSILON")));
		LogUtil::Info(Utils::FormatCString(_T("ActParamMore is EPSILON near line %d"), mCurLine));
	}
	else if (GetCurToken().lex == LexType::COMMA)
	{
		Token t = GetCurToken();

		if (Match(LexType::COMMA))
		{
			apm->mChilds.push_back(GetMatchedTerminal(t));
		}
		apm->mChilds.push_back(ActParamList());
	}
	else
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[ActParamMore] Finished")));
	return apm;
}

/* 书上算法部分貌似少了该条产生式 */
// RelExp ::= Exp OtherRelE
RTreeNode* RSyntaxParser::RelExp()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [RelExp]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing RelExp in line %d"), mCurLine));
	RTreeNode* re = new RTreeNode();
	re->mNodeType = NodeType::RelExp;
	re->mLine = mCurLine;

	re->mChilds.push_back(Exp());
	re->mChilds.push_back(OtherRelE());

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[RelExp] Finished")));
	return re;
}

/* 书上算法部分貌似少了该条产生式 */
// OtherRelE ::= CmpOp Exp
RTreeNode* RSyntaxParser::OtherRelE()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [OtherRelE]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing OtherRelE in line %d"), mCurLine));
	RTreeNode* ore = new RTreeNode();
	ore->mNodeType = NodeType::OtherRelE;
	ore->mLine = mCurLine;

	ore->mChilds.push_back(CmpOp());
	ore->mChilds.push_back(Exp());

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[OtherRelE] Finished")));
	return ore;
}

// Exp ::= Term OtherTerm
RTreeNode* RSyntaxParser::Exp()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [Exp]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing Exp in line %d"), mCurLine));
	RTreeNode* exp = new RTreeNode();
	exp->mNodeType = NodeType::Exp;
	exp->mLine = mCurLine;

	exp->mChilds.push_back(Term());
	exp->mChilds.push_back(OtherTerm());

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[Exp] Finished")));
	return exp;
}

// OtherTerm ::= 
//				ε { LT , EQU, RSQUAREBRACKET, THEN, ELSE, FI, DO, ENDWH, RPARENTHESIS, END, SEMICOLON,COMMA }
//				| AddOp Exp { PLUS, MINUS }
RTreeNode* RSyntaxParser::OtherTerm()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [OtherTerm]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing OtherTerm in line %d"), mCurLine));
	RTreeNode* ot = new RTreeNode();
	ot->mNodeType = NodeType::OtherTerm;
	ot->mLine = mCurLine;

	if (GetCurToken().lex == LexType::LT || GetCurToken().lex == LexType::EQU
		|| GetCurToken().lex == LexType::RSQUAREBRACKET || GetCurToken().lex == LexType::THEN
		|| GetCurToken().lex == LexType::ELSE || GetCurToken().lex == LexType::FI
		|| GetCurToken().lex == LexType::DO || GetCurToken().lex == LexType::ENDWHILE
		|| GetCurToken().lex == LexType::RPARENTHESIS || GetCurToken().lex == LexType::END
		|| GetCurToken().lex == LexType::SEMICOLON || GetCurToken().lex == LexType::COMMA)	// 向前看 1 个
	{
		RTreeNode* n = GetMatchedTerminal(Token(LexType::EPSILON));
		ot->mChilds.push_back(n);
		mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[OtherTerm] is EPSILON")));
		LogUtil::Info(Utils::FormatCString(_T("OtherTerm is EPSILON near line %d"), mCurLine));
	}
	else if (GetCurToken().lex == LexType::PLUS || GetCurToken().lex == LexType::MINUS)
	{
		ot->mChilds.push_back(AddOp());
		ot->mChilds.push_back(Exp());
	}
	else
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[OtherTerm] Finished")));
	return ot;
}

// Term ::= Factor OtherFactor
RTreeNode* RSyntaxParser::Term()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [Term]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing Term in line %d"), mCurLine));
	RTreeNode* t = new RTreeNode();
	t->mNodeType = NodeType::Term;
	t->mLine = mCurLine;
	
	t->mChilds.push_back(Factor());
	t->mChilds.push_back(OtherFactor());
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[Term] Finished")));
	return t;
}


// Factor ::= 
//			T-<LPARENTHESIS> Exp T-<RPARENTHESIS>	{ LPARENTHESIS }
//			| T-<UINTEGER>	{ UINTEGER }
//			| Variable { IDENTIFIER }
RTreeNode* RSyntaxParser::Factor()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [Factor]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing Factor in line %d"), mCurLine));
	RTreeNode* f = new RTreeNode();
	f->mNodeType = NodeType::Factor;
	f->mLine = mCurLine;

	if (GetCurToken().lex == LexType::LPARENTHESIS)	// 向前看 1 个
	{
		Token t = GetCurToken();

		if (Match(LexType::LPARENTHESIS))
		{
			f->mChilds.push_back(GetMatchedTerminal(t));
		}
		f->mChilds.push_back(Exp());

		t = GetCurToken();

		if (Match(LexType::RPARENTHESIS))
		{
			f->mChilds.push_back(GetMatchedTerminal(t));
		}
	}
	else if (GetCurToken().lex == LexType::UINTEGER)
	{
		Token t = GetCurToken();

		if (Match(LexType::UINTEGER))
		{
			f->mChilds.push_back(GetMatchedTerminal(t));
		}
	}
	else if (GetCurToken().lex == LexType::IDENTIFIER)
	{
		f->mChilds.push_back(Variable());
	}
	else
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[Factor] Finished")));
	return f;
}


// OtherFactor ::= 
//				ε { PLUS, MINUS, LT, EQU, RSQUAREBRACKET, THEN , ELSE , FI, DO, ENDWH, RPARENTHESIS, END, SEMICOLON, COMMA }
//				| MultOp Term { MULTIPLY, DIVIDE}
RTreeNode* RSyntaxParser::OtherFactor()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [OtherFactor]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing OtherFactor in line %d"), mCurLine));
	RTreeNode* of = new RTreeNode();
	of->mNodeType = NodeType::OtherFactor;
	of->mLine = mCurLine;

	if (GetCurToken().lex == LexType::PLUS || GetCurToken().lex == LexType::MINUS
		|| GetCurToken().lex == LexType::LT || GetCurToken().lex == LexType::EQU
		|| GetCurToken().lex == LexType::RSQUAREBRACKET || GetCurToken().lex == LexType::THEN
		|| GetCurToken().lex == LexType::ELSE || GetCurToken().lex == LexType::FI
		|| GetCurToken().lex == LexType::DO || GetCurToken().lex == LexType::ENDWHILE
		|| GetCurToken().lex == LexType::RPARENTHESIS || GetCurToken().lex == LexType::END
		|| GetCurToken().lex == LexType::SEMICOLON || GetCurToken().lex == LexType::COMMA)	// 向前看 1 个
	{
		RTreeNode* n = GetMatchedTerminal(Token(LexType::EPSILON));
		of->mChilds.push_back(n);
		mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[OtherFactor] is EPSILON")));
		LogUtil::Info(Utils::FormatCString(_T("OtherFactor is EPSILON near line %d"), mCurLine));
	}
	else if (GetCurToken().lex == LexType::MULTIPLY || GetCurToken().lex == LexType::DIVIDE)
	{
		of->mChilds.push_back(MultOp());
		of->mChilds.push_back(Term());
	}
	else
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[OtherFactor] Finished")));
	return of;
}


// Variable ::= T-<IDENTIFIER> VariMore
RTreeNode* RSyntaxParser::Variable()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [Variable]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing Variable in line %d"), mCurLine));
	RTreeNode* v = new RTreeNode();
	v->mNodeType = NodeType::Variable;
	v->mLine = mCurLine;

	Token t = GetCurToken();

	if (Match(LexType::IDENTIFIER))
	{
		v->mChilds.push_back(GetMatchedTerminal(t));
	}
	v->mChilds.push_back(VariMore());

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[Variable] Finished")));
	return v;
}

/* 书上产生式93：Predict 集少了 RSQUAREBRACKET */
// VariMore ::= 
//				ε { ASSIGN, MULTIPLY, DIVIDE, PLUS, MINUS, LT, EQU,THEN, ELSE, FI, DO, ENDWH, RPARENTHESIS, END, SEMICOLON, COMMA, RSQUAREBRACKET }
//				| T-<LSQUAREBRACKET> Exp T-<RSQUAREBRACKET> { LSQUAREBRACKET }
//				| T-<DOT> FieldVar	{DOT}
RTreeNode* RSyntaxParser::VariMore()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [VariMore]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing VariMore in line %d"), mCurLine));
	RTreeNode* vm = new RTreeNode();
	vm->mNodeType = NodeType::VariMore;
	vm->mLine = mCurLine;

	if (GetCurToken().lex == LexType::ASSIGN || GetCurToken().lex == LexType::MULTIPLY
		|| GetCurToken().lex == LexType::DIVIDE || GetCurToken().lex == LexType::PLUS
		|| GetCurToken().lex == LexType::MINUS || GetCurToken().lex == LexType::LT
		|| GetCurToken().lex == LexType::EQU || GetCurToken().lex == LexType::THEN
		|| GetCurToken().lex == LexType::ELSE || GetCurToken().lex == LexType::FI
		|| GetCurToken().lex == LexType::DO || GetCurToken().lex == LexType::ENDWHILE
		|| GetCurToken().lex == LexType::RPARENTHESIS || GetCurToken().lex == LexType::END
		|| GetCurToken().lex == LexType::SEMICOLON || GetCurToken().lex == LexType::COMMA
		|| GetCurToken().lex == LexType::RSQUAREBRACKET)	// 向前看 1 个
	{
		RTreeNode* n = GetMatchedTerminal(Token(LexType::EPSILON));
		vm->mChilds.push_back(n);
		mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[VariMore] is EPSILON")));
		LogUtil::Info(Utils::FormatCString(_T("VariMore is EPSILON near line %d"), mCurLine));
	}
	else if (GetCurToken().lex == LexType::LSQUAREBRACKET)
	{
		Token t = GetCurToken();

		if (Match(LexType::LSQUAREBRACKET))
		{
			vm->mChilds.push_back(GetMatchedTerminal(t));
		}
		vm->mChilds.push_back(Exp());
		t = GetCurToken();

		if (Match(LexType::RSQUAREBRACKET))
		{
			vm->mChilds.push_back(GetMatchedTerminal(t));
		}
	}
	else if (GetCurToken().lex == LexType::DOT)
	{
		Token t = GetCurToken();

		if (Match(LexType::DOT))
		{
			vm->mChilds.push_back(GetMatchedTerminal(t));
		}
		vm->mChilds.push_back(FieldVar());
	}
	else
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[VariMore] Finished")));
	return vm;
}

// FieldVar ::= T-<IDENTIFIER> VariMore
RTreeNode* RSyntaxParser::FieldVar()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [FieldVar]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing FieldVar in line %d"), mCurLine));
	RTreeNode* fv = new RTreeNode();
	fv->mNodeType = NodeType::FieldVar;
	fv->mLine = mCurLine;

	Token t = GetCurToken();

	if (Match(LexType::IDENTIFIER))
	{
		fv->mChilds.push_back(GetMatchedTerminal(t));
	}
	fv->mChilds.push_back(VariMore());

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[FieldVar] Finished")));
	return fv;
}


// FieldVarMore ::= 
//				ε { ASSIGN, MULTIPLY, DIVIDE, PLUS, MINUS, LT, EQU,THEN, ELSE, FI, DO, ENDWH, RPARENTHESIS, END, SEMICOLON, COMMA }
//				| T-<LSQUAREBRACKET> Exp T-<RSQUAREBRACKET> { LSQUAREBRACKET }
RTreeNode* RSyntaxParser::FieldVarMore()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [FieldVarMore]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing FieldVarMore in line %d"), mCurLine));
	RTreeNode* fvm = new RTreeNode();
	fvm->mNodeType = NodeType::FieldVarMore;
	fvm->mLine = mCurLine;

	if (GetCurToken().lex == LexType::ASSIGN || GetCurToken().lex == LexType::MULTIPLY
		|| GetCurToken().lex == LexType::DIVIDE || GetCurToken().lex == LexType::PLUS
		|| GetCurToken().lex == LexType::MINUS || GetCurToken().lex == LexType::LT
		|| GetCurToken().lex == LexType::EQU || GetCurToken().lex == LexType::THEN
		|| GetCurToken().lex == LexType::ELSE || GetCurToken().lex == LexType::FI
		|| GetCurToken().lex == LexType::DO || GetCurToken().lex == LexType::ENDWHILE
		|| GetCurToken().lex == LexType::RPARENTHESIS || GetCurToken().lex == LexType::END
		|| GetCurToken().lex == LexType::SEMICOLON || GetCurToken().lex == LexType::COMMA)	// 向前看 1 个
	{
		RTreeNode* n = GetMatchedTerminal(Token(LexType::EPSILON));
		fvm->mChilds.push_back(n);
		mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[FieldVarMore] is EPSILON")));
		LogUtil::Info(Utils::FormatCString(_T("FieldVarMore is EPSILON near line %d"), mCurLine));
	}
	else if (GetCurToken().lex == LexType::LSQUAREBRACKET)
	{
		Token t = GetCurToken();

		if (Match(LexType::LSQUAREBRACKET))
		{
			fvm->mChilds.push_back(GetMatchedTerminal(t));
		}
		fvm->mChilds.push_back(Exp());
		t = GetCurToken();

		if (Match(LexType::RSQUAREBRACKET))
		{
			fvm->mChilds.push_back(GetMatchedTerminal(t));
		}
	}
	else
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[FieldVarMore] Finished")));
	return fvm;
}


/* 书上算法部分少了这一条 */
// CmpOp ::= 
//			T-<LT>	{ LT }
//			|T-<EQU>	{ EQU }
RTreeNode* RSyntaxParser::CmpOp()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [CmpOp]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing CmpOp in line %d"), mCurLine));
	RTreeNode* co = new RTreeNode();
	co->mNodeType = NodeType::CmpOp;
	co->mLine = mCurLine;

	if (GetCurToken().lex == LexType::LT)
	{
		Token t = GetCurToken();

		if (Match(LexType::LT))
		{
			co->mChilds.push_back(GetMatchedTerminal(t));
		}
	}
	else if (GetCurToken().lex == LexType::EQU)
	{
		Token t = GetCurToken();

		if (Match(LexType::EQU))
		{
			co->mChilds.push_back(GetMatchedTerminal(t));
		}
	}
	else
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}	

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[CmpOp] Finished")));
	return co;
}

// AddOp ::= 
//			T-<PLUS>	{ PLUS }
//			|T-<MINUS>	{ MINUS }
RTreeNode* RSyntaxParser::AddOp()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [AddOp]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing AddOp in line %d"), mCurLine));
	RTreeNode* ao = new RTreeNode();
	ao->mNodeType = NodeType::AddOp;
	ao->mLine = mCurLine;

	if (GetCurToken().lex == LexType::PLUS)
	{
		Token t = GetCurToken();

		if (Match(LexType::PLUS))
		{
			ao->mChilds.push_back(GetMatchedTerminal(t));
		}
	}
	else if (GetCurToken().lex == LexType::MINUS)
	{
		Token t = GetCurToken();

		if (Match(LexType::MINUS))
		{
			ao->mChilds.push_back(GetMatchedTerminal(t));
		}
	}
	else
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[AddOp] Finished")));
	return ao;
}

// MultOp ::= 
//			T-<MULTIPLY> { MULTIPLY }
//			|T-<DIVIDE>	{ DIVIDE }
RTreeNode* RSyntaxParser::MultOp()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [MultOp]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing MultOp in line %d"), mCurLine));
	RTreeNode* mo = new RTreeNode();
	mo->mNodeType = NodeType::MultOp;
	mo->mLine = mCurLine;

	if (GetCurToken().lex == LexType::MULTIPLY)
	{
		Token t = GetCurToken();

		if (Match(LexType::MULTIPLY))
		{
			mo->mChilds.push_back(GetMatchedTerminal(t));
		}
	}
	else if (GetCurToken().lex == LexType::DIVIDE)
	{
		Token t = GetCurToken();

		if (Match(LexType::DIVIDE))
		{
			mo->mChilds.push_back(GetMatchedTerminal(t));
		}
	}
	else
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}	

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[MultOp] Finished")));
	return mo;
}

// ProgramBody ::= T-<BEGIN> StmList T-<END>
RTreeNode* RSyntaxParser::ProgramBody()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [ProgramBody]")));
	LogUtil::Info(Utils::FormatCString(_T("Parsing ProgramBody in line %d"), mCurLine));
	RTreeNode* pb = new RTreeNode();
	pb->mNodeType = NodeType::ProgramBody;
	pb->mLine = mCurLine;

	Token t = GetCurToken();

	if (Match(LexType::BEGIN))
	{
		pb->mChilds.push_back(GetMatchedTerminal(t));
	}

	pb->mChilds.push_back(StmList());

	t = GetCurToken();

	if (Match(LexType::END))
	{
		pb->mChilds.push_back(GetMatchedTerminal(t));
	}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[ProgramBody] Finished")));
	return pb;
}

bool RSyntaxParser::Match(LexType type)
{
	if (GetCurToken().lex == type)
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, Utils::FormatCString(_T("%s [%s] Matched"), mLexicalAnalyzer.mLex2String[type], GetCurToken().sem)));
		LogUtil::Info(Utils::FormatCString(_T("%s [%s] matched near line %d"), mLexicalAnalyzer.mLex2String[type], GetCurToken().sem, mCurLine));
		NextToken();
		mCurLine = GetCurToken().line;
		return true;
	}
	else
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Missing %s"), mLexicalAnalyzer.mLex2String[type])));
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		LogUtil::Error(Utils::FormatCString(_T("Missing %s near line %d"), mLexicalAnalyzer.mLex2String[type], mCurLine));
		NextToken();
		mCurLine = GetCurToken().line;
		return false;
	}
}

RTreeNode* RSyntaxParser::GetMatchedTerminal(Token t)
{
	RTreeNode* r = new RTreeNode();
	r->mNodeType = NodeType::Terminal;
	r->mLine = mCurLine;
	r->mToken = t;
	return r;
}

void RSyntaxParser::ReleaseTree(RTreeNode* r)
{
	if (r == NULL)
		return;
	for (int i = 0; i < r->mChilds.size(); i++)
	{
		ReleaseTree(r->mChilds[i]);
	}
	delete r;
	r = NULL;
}

void RSyntaxParser::RecordLog(LogType type, int line, CString log)
{
	switch (type)
	{
	case LERROR:

		break;
	case LINFO:
		break;
	case LDEBUG:
		break;
	default:
		break;
	}
}

void RSyntaxParser::InitMap()
{
	mNodeType2Str[NodeType::Program] = _T("Program");
	mNodeType2Str[NodeType::ProgramHead] = _T("ProgramHead");
	mNodeType2Str[NodeType::ProgramName] = _T("ProgramName");
	mNodeType2Str[NodeType::ProgramBody] = _T("ProgramBody");
	mNodeType2Str[NodeType::StmList] = _T("StmList");
	mNodeType2Str[NodeType::Stm] = _T("Stm");
	mNodeType2Str[NodeType::StmMore] = _T("StmMore");
	mNodeType2Str[NodeType::DeclarePart] = _T("DeclarePart");
	mNodeType2Str[NodeType::TypeDec] = _T("TypeDec");
	//mNodeType2Str[NodeType::EMPTY] = _T("EMPTY");
	mNodeType2Str[NodeType::TypeDecList] = _T("TypeDecList");
	mNodeType2Str[NodeType::TypeId] = _T("TypeId");
	mNodeType2Str[NodeType::TypeDef] = _T("TypeDef");
	mNodeType2Str[NodeType::TypeDecMore] = _T("TypeDecMore");
	mNodeType2Str[NodeType::BaseType] = _T("BaseType");
	mNodeType2Str[NodeType::StructureType] = _T("StructureType");
	mNodeType2Str[NodeType::ArrayType] = _T("ArrayType");
	mNodeType2Str[NodeType::RecType] = _T("RecType");
	mNodeType2Str[NodeType::FieldDecList] = _T("FieldDecList");
	mNodeType2Str[NodeType::IdList] = _T("IdList");
	mNodeType2Str[NodeType::FieldDecMore] = _T("FieldDecMore");
	mNodeType2Str[NodeType::IdMore] = _T("IdMore");
	mNodeType2Str[NodeType::VarDec] = _T("VarDec");
	mNodeType2Str[NodeType::VarDeclaration] = _T("VarDeclaration");
	mNodeType2Str[NodeType::VarDecList] = _T("VarDecList");
	mNodeType2Str[NodeType::VarIdList] = _T("VarIdList");
	mNodeType2Str[NodeType::VarIdMore] = _T("VarIdMore");
	mNodeType2Str[NodeType::VarDecMore] = _T("VarDecMore");
	mNodeType2Str[NodeType::ProcDec] = _T("ProcDec");
	mNodeType2Str[NodeType::ProcDeclaration] = _T("ProcDeclaration");
	mNodeType2Str[NodeType::ProcDecMore] = _T("ProcDecMore");
	mNodeType2Str[NodeType::ParamList] = _T("ParamList");
	mNodeType2Str[NodeType::ParamDecList] = _T("ParamDecList");
	mNodeType2Str[NodeType::Param] = _T("Param");
	mNodeType2Str[NodeType::ParamMore] = _T("ParamMore");
	mNodeType2Str[NodeType::FormList] = _T("FormList");
	mNodeType2Str[NodeType::FidMore] = _T("FidMore");
	//mNodeType2Str[NodeType::ProcDecPart] = _T("ProcDecPart");
	mNodeType2Str[NodeType::ProcBody] = _T("ProcBody");
	mNodeType2Str[NodeType::AssCall] = _T("AssCall");
	mNodeType2Str[NodeType::AssignmentRest] = _T("AssignmentRest");
	mNodeType2Str[NodeType::ConditionalStm] = _T("ConditionalStm");
	mNodeType2Str[NodeType::LoopStm] = _T("LoopStm");
	mNodeType2Str[NodeType::InputStm] = _T("InputStm");
	mNodeType2Str[NodeType::OutputStm] = _T("OutputStm");
	mNodeType2Str[NodeType::ReturnStm] = _T("ReturnStm");
	mNodeType2Str[NodeType::CallStmRest] = _T("CallStmRest");
	mNodeType2Str[NodeType::ActParamList] = _T("ActParamList");
	mNodeType2Str[NodeType::ActParamMore] = _T("ActParamMore");
	mNodeType2Str[NodeType::Exp] = _T("Exp");
	mNodeType2Str[NodeType::OtherTerm] = _T("OtherTerm");
	mNodeType2Str[NodeType::Term] = _T("Term");
	mNodeType2Str[NodeType::OtherFactor] = _T("OtherFactor");
	mNodeType2Str[NodeType::Factor] = _T("Factor");
	mNodeType2Str[NodeType::Variable] = _T("Variable");
	mNodeType2Str[NodeType::VariMore] = _T("VariMore");
	mNodeType2Str[NodeType::FieldVar] = _T("FieldVar");
	mNodeType2Str[NodeType::FieldVarMore] = _T("FieldVarMore");
	mNodeType2Str[NodeType::RelExp] = _T("RelExp");
	mNodeType2Str[NodeType::OtherRelE] = _T("OtherRelE");
	mNodeType2Str[NodeType::CmpOp] = _T("CmpOp");
	mNodeType2Str[NodeType::AddOp] = _T("AddOp");
	mNodeType2Str[NodeType::MultOp] = _T("MultOp");
	mNodeType2Str[NodeType::Terminal] = _T("Terminal");
}

CString RSyntaxParser::GetSyntaxTreeStr(CString lftstr, CString append, RTreeNode* r)
{

	CString b = append;
	if (r->mNodeType == NodeType::Terminal)
		//b += mLexicalAnalyzer.mLex2String[mLexicalAnalyzer.mReservedWords[r->mSemName]];

	{
		b += mLexicalAnalyzer.mLex2String[r->mToken.lex];
	}
	else
		b += mNodeType2Str[r->mNodeType];
	b += _T("\r\n");;

	vector<RTreeNode*> childs = r->mChilds;
	if (childs.size() > 0)
	{
		for (int i = 0; i < childs.size() - 1; i++)
		{
			b += (lftstr + GetSyntaxTreeStr(lftstr + _T("      | "), _T("      |-"), childs[i]));
			
		}
		b += (lftstr + GetSyntaxTreeStr(lftstr + _T("      "), _T("      |-"), childs[childs.size() - 1]));
	}
	return b;
}

CString RSyntaxParser::GetStrByLen(int len)
{
	CString str = _T("");
	for (int i = 0; i < len; i++)
		str += "-";
	return str;
}
