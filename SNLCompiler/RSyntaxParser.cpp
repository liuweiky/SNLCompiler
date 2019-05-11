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
}


RSyntaxParser::~RSyntaxParser()
{
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
		return Token();
	else
		return mTokenList[mTokenPtr];
}

// 该函数调用总程序处理函数，创建语法分析树。
// 同时处理文件的提前结束错误。
// 函数处理成功，则返回所生成的语句类型语法树节点。
// 否则，函数返回NULL。
RTreeNode* RSyntaxParser::Parse()
{
	LogUtil::Info(_T("Now begin parsing. Good luck!"));
	RTreeNode* r = Program();
	if (GetCurToken().lex != LexType::LEXEOF)
	{
		LogUtil::Error(_T("The code ends too early!"));
	}
	return r;
}


// Program ::= ProgramHead DeclarePart ProgramBody
RTreeNode* RSyntaxParser::Program()
{
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
		LogUtil::Error(Utils::FormatCString(_T("Missing ProgramHead near line %d"), mCurLine));
	}

	if (dp == NULL)
	{
		LogUtil::Error(Utils::FormatCString(_T("Missing DeclarePart near line %d"), mCurLine));
	}

	if (pb == NULL)
	{
		LogUtil::Error(Utils::FormatCString(_T("Missing ProgramBody near line %d"), mCurLine));
	}

	if (!Match(LexType::DOT))
	{
		/*CString s;
		s.Format(_T("Missing <DOT> in line %d"), mCurLine);*/
		//LogUtil::Error(Utils::FormatCString(_T("Missing <DOT> near line %d"), mCurLine));
	}

	return prog;
}


// ProgramHead ::= T-<PROGRAM> ProgramName
RTreeNode* RSyntaxParser::ProgramHead()
{
	LogUtil::Info(Utils::FormatCString(_T("Parsing ProgramHead in line %d"), mCurLine));
	RTreeNode* ph = new RTreeNode();
	ph->mNodeType = NodeType::ProgramHead;
	ph->mLine = mCurLine;
	if (!Match(LexType::PROGRAM))
	{
		//LogUtil::Error(Utils::FormatCString(_T("Missing <PROGRAM> near line %d"), mCurLine));
	}

	RTreeNode* pn = ProgramName();
	ph->mChilds.push_back(pn);
	if (pn == NULL)
	{
		LogUtil::Error(Utils::FormatCString(_T("Missing ProgramName near line %d"), mCurLine));
	}

	

	return ph;
}


// ProgramName ::= T-<IDENTIFIER>
RTreeNode* RSyntaxParser::ProgramName()
{
	LogUtil::Info(Utils::FormatCString(_T("Parsing ProgramName in line %d"), mCurLine));
	RTreeNode* pn = new RTreeNode();
	pn->mNodeType = NodeType::ProgramName;
	pn->mLine = mCurLine;
	
	Token t = GetCurToken();

	if (Match(LexType::IDENTIFIER))
	{
		RTreeNode* id = new RTreeNode();
		id->mNodeType = NodeType::Terminal;
		id->mLine = mCurLine;
		id->mSemName = t.sem;
		pn->mChilds.push_back(id);
	}
	else
	{
		//LogUtil::Error(Utils::FormatCString(_T("Missing <IDENTIFIER> near line %d"), mCurLine));
	}
	return pn;
}

// DeclarePart ::= TypeDec VarDec ProcDec
RTreeNode* RSyntaxParser::DeclarePart()
{
	// 在 DeclarePart 中可以声明自定义类型（数组等）、变量、过程
	// DeclarePart 本身只是一个标记
	LogUtil::Info(Utils::FormatCString(_T("Parsing DeclarePart in line %d"), mCurLine));
	RTreeNode* dp = new RTreeNode();
	dp->mNodeType = NodeType::DeclarePart;
	dp->mLine = mCurLine;

	RTreeNode* td = TypeDec();
	dp->mChilds.push_back(td);
	if (td == NULL)
		LogUtil::Error(Utils::FormatCString(_T("Missing TypeDec near line %d"), mCurLine));

	RTreeNode* vd = VarDec();
	dp->mChilds.push_back(vd);
	if (vd == NULL)
		LogUtil::Error(Utils::FormatCString(_T("Missing VarDec near line %d"), mCurLine));

	RTreeNode* pd = ProcDec();
	dp->mChilds.push_back(pd);
	if (vd == NULL)
		LogUtil::Error(Utils::FormatCString(_T("Missing ProcDec near line %d"), mCurLine));

	return dp;
}


// ProcDec ::= 
//				ε { BEGIN }
//				| ProcDeclaration { PROCEDURE }
RTreeNode* RSyntaxParser::ProcDec()
{
	LogUtil::Info(Utils::FormatCString(_T("Parsing ProcDec in line %d"), mCurLine));
	RTreeNode* pd = new RTreeNode();
	pd->mNodeType = NodeType::ProcDec;
	pd->mLine = mCurLine;


	if (GetCurToken().lex == LexType::BEGIN)	// 向前看 1 个
	{
		RTreeNode* n = new RTreeNode();
		n->mNodeType = NodeType::EMPTY;	//空产生式
		n->mLine = mCurLine;
		pd->mChilds.push_back(n);
		LogUtil::Info(Utils::FormatCString(_T("ProcDec is EPS near line %d"), mCurLine));
	}
	else if (GetCurToken().lex == LexType::PROCEDURE)
	{
		pd->mChilds.push_back(ProcDeclaration());
	}

	return pd;
}

// ProcDeclaration ::= 
//						T-<PROCEDURE>
//						ProcName(ParamList)
//						ProcDecPart
//						ProcBody
RTreeNode* RSyntaxParser::ProcDeclaration()
{
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
	return pd;
}


// ProcDecPart ::= DeclarePart
RTreeNode* RSyntaxParser::ProcDecPart()
{
	LogUtil::Info(Utils::FormatCString(_T("Parsing ProcDecPart in line %d"), mCurLine));
	RTreeNode* pdp = new RTreeNode();
	pdp->mNodeType = NodeType::ProcDecPart;
	pdp->mLine = mCurLine;
	
	pdp->mChilds.push_back(DeclarePart());

	return pdp;
}

// ProcBody ::= ProgramBody
RTreeNode* RSyntaxParser::ProcBody()
{
	LogUtil::Info(Utils::FormatCString(_T("Parsing ProcBody in line %d"), mCurLine));
	RTreeNode* pb = new RTreeNode();
	pb->mNodeType = NodeType::ProcBody;
	pb->mLine = mCurLine;

	pb->mChilds.push_back(ProgramBody());

	return pb;
}

// ParamList :: = 
//				ε { RPAREN }
//				| ParamDecList { INTEGER, CHAR, ARRAY, RECORD, ID, VAR }
RTreeNode* RSyntaxParser::ParamList()
{
	LogUtil::Info(Utils::FormatCString(_T("Parsing ParamList in line %d"), mCurLine));
	RTreeNode* pl = new RTreeNode();
	pl->mNodeType = NodeType::ParamList;
	pl->mLine = mCurLine;


	if (GetCurToken().lex == LexType::RPARENTHESIS)	// 向前看 1 个
	{
		RTreeNode* n = new RTreeNode();
		n->mNodeType = NodeType::EMPTY;	//空产生式
		n->mLine = mCurLine;
		pl->mChilds.push_back(n);
		LogUtil::Info(Utils::FormatCString(_T("ParamList is EPS near line %d"), mCurLine));
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
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}


	return pl;
}


// ParamDecList ::= Param ParamMore
RTreeNode* RSyntaxParser::ParamDecList()
{
	LogUtil::Info(Utils::FormatCString(_T("Parsing ParamDecList in line %d"), mCurLine));
	RTreeNode* vd = new RTreeNode();
	vd->mNodeType = NodeType::ParamDecList;
	vd->mLine = mCurLine;
	vd->mChilds.push_back(Param());
	vd->mChilds.push_back(ParamMore());
	
	return vd;
}


// ParamMore ::= 
//				ε { RPAREN }
//				| T-<SEMICOLON> ParamDecList { SEMICOLON }
RTreeNode* RSyntaxParser::ParamMore()
{
	LogUtil::Info(Utils::FormatCString(_T("Parsing ParamMore in line %d"), mCurLine));
	RTreeNode* pm = new RTreeNode();
	pm->mNodeType = NodeType::ParamMore;
	pm->mLine = mCurLine;
	
	if (GetCurToken().lex == LexType::RPARENTHESIS)	// 向前看 1 个
	{
		RTreeNode* n = new RTreeNode();
		n->mNodeType = NodeType::EMPTY;	//空产生式
		n->mLine = mCurLine;
		pm->mChilds.push_back(n);
		LogUtil::Info(Utils::FormatCString(_T("FidMore is EPS near line %d"), mCurLine));
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
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	return pm;
}

// Param ::= 
//			TypeDef FormList { INTEGER, CHARARCTER, RECORD, ARRAY, IDENTIFIER }
//			| T-<VAR> TypeDef FormList { VAR }
RTreeNode* RSyntaxParser::Param()
{
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
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	return p;
}


// FormList ::= T-<IDENTIFIER> FidMore
RTreeNode* RSyntaxParser::FormList()
{
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

	return fl;
}


// FidMore ::= 
//				ε { SEMICOLON, RPARENTHESIS }
//				| T-<COMMA> FormList { COMMA }
RTreeNode* RSyntaxParser::FidMore()
{
	LogUtil::Info(Utils::FormatCString(_T("Parsing FidMore in line %d"), mCurLine));
	RTreeNode* fm = new RTreeNode();
	fm->mNodeType = NodeType::FidMore;
	fm->mLine = mCurLine;

	if (GetCurToken().lex == LexType::SEMICOLON || GetCurToken().lex == LexType::RPARENTHESIS)	// 向前看 1 个
	{
		RTreeNode* n = new RTreeNode();
		n->mNodeType = NodeType::EMPTY;	//空产生式
		n->mLine = mCurLine;
		fm->mChilds.push_back(n);
		LogUtil::Info(Utils::FormatCString(_T("FidMore is EPS near line %d"), mCurLine));
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
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	return fm;
}

// VarDec ::=
//				ε { PROCEDURE, BEGIN }
//				| VarDeclaration { VAR }
RTreeNode* RSyntaxParser::VarDec()
{
	LogUtil::Info(Utils::FormatCString(_T("Parsing VarDec in line %d"), mCurLine));
	RTreeNode* vd = new RTreeNode();
	vd->mNodeType = NodeType::VarDec;
	vd->mLine = mCurLine;

	
	if (GetCurToken().lex == LexType::PROCEDURE || GetCurToken().lex == LexType::BEGIN)	// 向前看 1 个
	{
		RTreeNode* n = new RTreeNode();
		n->mNodeType = NodeType::EMPTY;	//空产生式
		n->mLine = mCurLine;
		vd->mChilds.push_back(n);
		LogUtil::Info(Utils::FormatCString(_T("VarDec is EPS near line %d"), mCurLine));
	}
	else if (GetCurToken().lex == LexType::VAR)
	{
		vd->mChilds.push_back(VarDeclaration());
	}
	else
	{
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	return vd;
}


// VarDeclaration ::= T-<VAR> VarDecList
RTreeNode* RSyntaxParser::VarDeclaration()
{
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
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	return vd;
}

// VarDecList ::= TypeDef VarIdList T-<SEMICOLON> VarDecMore
RTreeNode* RSyntaxParser::VarDecList()
{
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
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	return vdl;
}

// VarIdMore :: = 
//				ε { SEMICOLON }
//				| T-<COMMA> VarIdList { COMMA }
RTreeNode* RSyntaxParser::VarIdMore()
{
	LogUtil::Info(Utils::FormatCString(_T("Parsing VarIdMore in line %d"), mCurLine));
	RTreeNode* vim = new RTreeNode();
	vim->mNodeType = NodeType::VarIdMore;
	vim->mLine = mCurLine;

	if (GetCurToken().lex == LexType::SEMICOLON)	// 向前看 1 个
	{
		RTreeNode* n = new RTreeNode();
		n->mNodeType = NodeType::EMPTY;	//空产生式
		n->mLine = mCurLine;
		vim->mChilds.push_back(n);
		LogUtil::Info(Utils::FormatCString(_T("VarIdMore is EPS near line %d"), mCurLine));
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
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	return vim;
}


// VarIdList ::= T-<IDENTIFIER> VarIdMore
RTreeNode* RSyntaxParser::VarIdList()
{
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

	return vil;
}

// VarDecMore ::= 
//				ε { PROCEDURE, BEGIN }
//				| VarDecList { INTEGER, CHARACTER, RECORD, ARRAY, IDENTIFIER }
RTreeNode* RSyntaxParser::VarDecMore()
{
	LogUtil::Info(Utils::FormatCString(_T("Parsing VarDecMore in line %d"), mCurLine));
	RTreeNode* vdm = new RTreeNode();
	vdm->mNodeType = NodeType::VarDecMore;
	vdm->mLine = mCurLine;

	if (GetCurToken().lex == LexType::PROCEDURE || GetCurToken().lex == LexType::BEGIN)	// 向前看 1 个
	{
		RTreeNode* n = new RTreeNode();
		n->mNodeType = NodeType::EMPTY;	//空产生式
		n->mLine = mCurLine;
		vdm->mChilds.push_back(n);
		LogUtil::Info(Utils::FormatCString(_T("VarDecMore is EPS near line %d"), mCurLine));
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
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	return vdm;
}


// TypeDec ::= 
//			ε Expected {VAR, PROCEDURE, BEGIN}
//			| TypeDeclaration Expected { TYPE }
RTreeNode* RSyntaxParser::TypeDec()
{
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
		RTreeNode* n = new RTreeNode();
		n->mNodeType = NodeType::EMPTY;	//空产生式
		n->mLine = mCurLine;
		td->mChilds.push_back(n);
		LogUtil::Info(Utils::FormatCString(_T("TypeDec is EPS near line %d"), mCurLine));
	}
	else
	{
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	return td;
}


// TypeDeclaration ::= T-<TYPE> TypeDecList
RTreeNode* RSyntaxParser::TypeDeclaration()
{
	LogUtil::Info(Utils::FormatCString(_T("Parsing TypeDeclaration in line %d"), mCurLine));
	RTreeNode* td = new RTreeNode();
	td->mNodeType = NodeType::TypeDec;
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

	return td;
}


// 
// TypeDecList ::= TypeId T-<EQU> TypeDef T-<SEMICOLON> TypeDecMore
RTreeNode* RSyntaxParser::TypeDecList()
{
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

	return tdl;
}


// TypeId :: = T-<IDENTIFIER>
RTreeNode* RSyntaxParser::TypeId()
{
	LogUtil::Info(Utils::FormatCString(_T("Parsing TypeId in line %d"), mCurLine));
	RTreeNode* ti = new RTreeNode();
	ti->mNodeType = NodeType::TypeId;
	ti->mLine = mCurLine;
	Token t = GetCurToken();

	if (Match(LexType::IDENTIFIER))
	{
		ti->mChilds.push_back(GetMatchedTerminal(t));
	}
	return ti;
}


// TypeDef ::=
//			BaseType { INTEGER, CHARACTER }
//			| StructureType { ARRAY, RECORD }
//			| T-<IDENTIFIER> { IDENTIFIER }
RTreeNode* RSyntaxParser::TypeDef()
{
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
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	return tdf;
}


// BaseType ::=
//				T-<INTEGER> { INTEGER }
//				| T-<CHAR> { CHARACTER }
RTreeNode* RSyntaxParser::BaseType()
{
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
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	return bt;
}


// StructureType ::=
//					ArrayType { ARRAY }
//					| RecType { RECORD }
RTreeNode* RSyntaxParser::StructureType()
{
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
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	return st;
}


// ArrayType ::= T-<ARRAY> T-<[low..top]> T-<OF> BaseType
RTreeNode* RSyntaxParser::ArrayType()
{
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

	return at;
}


// RecType ::= T-<RECORD> FieldDecList T-<END>
RTreeNode* RSyntaxParser::RecType()
{
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

	return rt;
}


// FieldDecList ::=
//				BaseType IdList T-<SEMICOLON> FieldDecMore { INTEGER, CHARACTER }
//				| ArrayType IdList T-<SEMICOLON> FieldDecMore { ARRAY }
RTreeNode* RSyntaxParser::FieldDecList()
{
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
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	return fd;
}


// IdList ::= T-<IDENTIFIER> IdMore
RTreeNode* RSyntaxParser::IdList()
{
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

	return idl;
}


// IdMore ::= 
//			ε { SEMICOLON }
//			| T-<COMMA> IdList { COMMA }
RTreeNode* RSyntaxParser::IdMore()
{
	LogUtil::Info(Utils::FormatCString(_T("Parsing IdMore in line %d"), mCurLine));
	RTreeNode* idm = new RTreeNode();
	idm->mNodeType = NodeType::IdMore;
	idm->mLine = mCurLine;

	if (GetCurToken().lex == LexType::SEMICOLON)	// 向前看 1 个
	{
		RTreeNode* n = new RTreeNode();
		n->mNodeType = NodeType::EMPTY;	//空产生式
		n->mLine = mCurLine;
		idm->mChilds.push_back(n);
		LogUtil::Info(Utils::FormatCString(_T("IdMore is EPS near line %d"), mCurLine));
	}
	else if (GetCurToken().lex == LexType::COMMA)
	{
		idm->mChilds.push_back(IdList());
	}
	else
	{
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	return idm;
}


// FieldDecMore ::= 
//					ε { END }
//					| FieldDecList { INTEGER, CHARACTER, ARRAY }
RTreeNode* RSyntaxParser::FieldDecMore()
{
	LogUtil::Info(Utils::FormatCString(_T("Parsing FieldDecMore in line %d"), mCurLine));
	RTreeNode* fdm = new RTreeNode();
	fdm->mNodeType = NodeType::FieldDecMore;
	fdm->mLine = mCurLine;

	if (GetCurToken().lex == LexType::END)	// 向前看 1 个
	{
		RTreeNode* n = new RTreeNode();
		n->mNodeType = NodeType::EMPTY;	//空产生式
		n->mLine = mCurLine;
		fdm->mChilds.push_back(n);
		LogUtil::Info(Utils::FormatCString(_T("FieldDecMore is EPS near line %d"), mCurLine));
	}
	else if (GetCurToken().lex == LexType::INTEGER || GetCurToken().lex == LexType::CHARACTER || GetCurToken().lex == LexType::ARRAY)
	{
		fdm->mChilds.push_back(FieldDecList());
	}
	else
	{
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}


	return fdm;
}



// TypeDecMore ::= 
//					ε { VAR, PROCEDURE, BEGIN }
//					| TypeDecList { IDENTIFIER }
RTreeNode* RSyntaxParser::TypeDecMore()
{
	LogUtil::Info(Utils::FormatCString(_T("Parsing TypeDecMore in line %d"), mCurLine));
	RTreeNode* tdm = new RTreeNode();
	tdm->mNodeType = NodeType::TypeDecMore;
	tdm->mLine = mCurLine;

	if (GetCurToken().lex == LexType::VAR || GetCurToken().lex == LexType::PROCEDURE || GetCurToken().lex == LexType::BEGIN)	// 向前看 1 个
	{
		RTreeNode* n = new RTreeNode();
		n->mNodeType = NodeType::EMPTY;	//空产生式
		n->mLine = mCurLine;
		tdm->mChilds.push_back(n);
		LogUtil::Info(Utils::FormatCString(_T("TypeDecMore is EPS near line %d"), mCurLine));
	}
	else if (GetCurToken().lex == LexType::IDENTIFIER)
	{
		tdm->mChilds.push_back(TypeDecList());
	}
	else
	{
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	return tdm;
}


// StmList ::= Stm StmMore
RTreeNode* RSyntaxParser::StmList()
{
	LogUtil::Info(Utils::FormatCString(_T("Parsing StmList in line %d"), mCurLine));
	RTreeNode* sl = new RTreeNode();
	sl->mNodeType = NodeType::StmList;
	sl->mLine = mCurLine;

	sl->mChilds.push_back(Stm());
	sl->mChilds.push_back(StmMore());

	return sl;
}


/* 书上算法37：StmMore ::= ε 的 Preict 集少了 ELSE、FI */
// StmMore ::=
//				ε { ELSE, FI, END, ENDWHILE }
//				| T-<SEMICOLON> StmList { SEMICOLON }
RTreeNode* RSyntaxParser::StmMore()
{
	LogUtil::Info(Utils::FormatCString(_T("Parsing StmMore in line %d"), mCurLine));
	RTreeNode* sm = new RTreeNode();
	sm->mNodeType = NodeType::StmMore;
	sm->mLine = mCurLine;

	if (
		GetCurToken().lex == LexType::END || GetCurToken().lex == LexType::ENDWHILE
		|| GetCurToken().lex == LexType::ELSE || GetCurToken().lex == LexType::FI)	// 向前看 1 个
	{
		RTreeNode* n = new RTreeNode();
		n->mNodeType = NodeType::EMPTY;	//空产生式
		n->mLine = mCurLine;
		sm->mChilds.push_back(n);
		LogUtil::Info(Utils::FormatCString(_T("StmMore is EPS near line %d"), mCurLine));
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
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}


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
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	return s;
}

/* 书上产生式67：AssCall ::= AssignmentRest 的 Predict 集少了 LSQUAREBRACKET、DOT */
/* 算法39：AssCall ::= CallStmRest Predict 集应为 { LPARENTHESIS } 而不是 { RPARENTHESIS } */
// AssCall ::= 
//				AssignmentRest { ASSIGN, LSQUAREBRACKET, DOT }
//				| CallStmRest { LPARENTHESIS }
RTreeNode* RSyntaxParser::AssCall()
{
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
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	return ac;
}


/* 书上算法40：产生式少了 VariMore */
// AssignmentRest ::= VariMore T-<ASSIGN> Exp
RTreeNode* RSyntaxParser::AssignmentRest()
{
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

	return ar;
}


/* 书上算法41：Exp 应改为 RelExp，StmL 应改为 StmList */
// ConditionalStm :: = T-<IF> RelExp T-<THEN> StmList T-<ELSE> StmList T-<FI>
RTreeNode* RSyntaxParser::ConditionalStm()
{
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

	return cs;
}


/* 书上算法42：Exp 应改为 RelExp */
// LoopStm :: = T-<WHILE> RelExp T-<DO> StmList T-<ENDWHILE>
RTreeNode* RSyntaxParser::LoopStm()
{
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

	return ls;
}

// InputStm ::= T-<READ> T-<LPARENTHESIS> T-<IDENTIFIER> T-<RPARENTHESIS>
RTreeNode* RSyntaxParser::InputStm()
{
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

	return is;
}


// OutputStm ::= T-<WRITE> T-<LPARENTHESIS> Exp T-<RPARENTHESIS>
RTreeNode* RSyntaxParser::OutputStm()
{
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

	return os;
}


// ReturnStm ::= T-<RETURN>
RTreeNode* RSyntaxParser::ReturnStm()
{
	LogUtil::Info(Utils::FormatCString(_T("Parsing ReturnStm in line %d"), mCurLine));
	RTreeNode* rs = new RTreeNode();
	rs->mNodeType = NodeType::ReturnStm;
	rs->mLine = mCurLine;

	Token t = GetCurToken();

	if (Match(LexType::RETURN))
	{
		rs->mChilds.push_back(GetMatchedTerminal(t));
	}

	return rs;
}


// CallStmRest ::= T-<LPARENTHESIS> ActParamList T-<RPARENTHESIS>
RTreeNode* RSyntaxParser::CallStmRest()
{
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

	return cs;
}


// ActParamList ::=
//				ε { RPARENTHESIS }
//				| Exp ActParamMore { IDENTIFIER, UINTEGER }
RTreeNode* RSyntaxParser::ActParamList()
{
	LogUtil::Info(Utils::FormatCString(_T("Parsing ActParamList in line %d"), mCurLine));
	RTreeNode* apl = new RTreeNode();
	apl->mNodeType = NodeType::ActParamList;
	apl->mLine = mCurLine;

	if (GetCurToken().lex == LexType::RPARENTHESIS)	// 向前看 1 个
	{
		RTreeNode* n = new RTreeNode();
		n->mNodeType = NodeType::EMPTY;	//空产生式
		n->mLine = mCurLine;
		apl->mChilds.push_back(n);
		LogUtil::Info(Utils::FormatCString(_T("ActParamList is EPS near line %d"), mCurLine));
	}
	else if (GetCurToken().lex == LexType::IDENTIFIER || GetCurToken().lex == LexType::UINTEGER)
	{
		apl->mChilds.push_back(Exp());
		apl->mChilds.push_back(ActParamMore());
	}
	else
	{
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	return apl;
}


// ActParamMore ::=
//				ε { RPARENTHESIS }
//				| T-<COMMA> ActParamList { COMMA }
RTreeNode* RSyntaxParser::ActParamMore()
{
	LogUtil::Info(Utils::FormatCString(_T("Parsing ActParamMore in line %d"), mCurLine));
	RTreeNode* apm = new RTreeNode();
	apm->mNodeType = NodeType::ActParamMore;
	apm->mLine = mCurLine;

	if (GetCurToken().lex == LexType::RPARENTHESIS)	// 向前看 1 个
	{
		RTreeNode* n = new RTreeNode();
		n->mNodeType = NodeType::EMPTY;	//空产生式
		n->mLine = mCurLine;
		apm->mChilds.push_back(n);
		LogUtil::Info(Utils::FormatCString(_T("ActParamMore is EPS near line %d"), mCurLine));
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
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	return apm;
}

/* 书上算法部分貌似少了该条产生式 */
// RelExp ::= Exp OtherRelE
RTreeNode* RSyntaxParser::RelExp()
{
	LogUtil::Info(Utils::FormatCString(_T("Parsing RelExp in line %d"), mCurLine));
	RTreeNode* re = new RTreeNode();
	re->mNodeType = NodeType::RelExp;
	re->mLine = mCurLine;

	re->mChilds.push_back(Exp());
	re->mChilds.push_back(OtherRelE());

	return re;
}

/* 书上算法部分貌似少了该条产生式 */
// OtherRelE ::= CmpOp Exp
RTreeNode* RSyntaxParser::OtherRelE()
{
	LogUtil::Info(Utils::FormatCString(_T("Parsing OtherRelE in line %d"), mCurLine));
	RTreeNode* ore = new RTreeNode();
	ore->mNodeType = NodeType::OtherRelE;
	ore->mLine = mCurLine;

	ore->mChilds.push_back(CmpOp());
	ore->mChilds.push_back(Exp());

	return ore;
}

// Exp ::= Term OtherTerm
RTreeNode* RSyntaxParser::Exp()
{
	LogUtil::Info(Utils::FormatCString(_T("Parsing Exp in line %d"), mCurLine));
	RTreeNode* exp = new RTreeNode();
	exp->mNodeType = NodeType::Exp;
	exp->mLine = mCurLine;

	exp->mChilds.push_back(Term());
	exp->mChilds.push_back(OtherTerm());
	
	return exp;
}

// OtherTerm ::= 
//				ε { LT , EQU, RSQUAREBRACKET, THEN, ELSE, FI, DO, ENDWH, RPARENTHESIS, END, SEMICOLON,COMMA }
//				| AddOp Exp { PLUS, MINUS }
RTreeNode* RSyntaxParser::OtherTerm()
{
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
		RTreeNode* n = new RTreeNode();
		n->mNodeType = NodeType::EMPTY;	//空产生式
		n->mLine = mCurLine;
		ot->mChilds.push_back(n);
		LogUtil::Info(Utils::FormatCString(_T("OtherTerm is EPS near line %d"), mCurLine));
	}
	else if (GetCurToken().lex == LexType::PLUS || GetCurToken().lex == LexType::MINUS)
	{
		ot->mChilds.push_back(AddOp());
		ot->mChilds.push_back(Exp());
	}
	else
	{
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}
	return ot;
}

// Term ::= Factor OtherFactor
RTreeNode* RSyntaxParser::Term()
{
	LogUtil::Info(Utils::FormatCString(_T("Parsing Term in line %d"), mCurLine));
	RTreeNode* t = new RTreeNode();
	t->mNodeType = NodeType::Term;
	t->mLine = mCurLine;
	
	t->mChilds.push_back(Factor());
	t->mChilds.push_back(OtherFactor());
	return t;
}


// Factor ::= 
//			T-<LPARENTHESIS> Exp T-<RPARENTHESIS>	{ LPARENTHESIS }
//			| T-<UINTEGER>	{ UINTEGER }
//			| Variable { IDENTIFIER }
RTreeNode* RSyntaxParser::Factor()
{
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
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}
	return f;
}


// OtherFactor ::= 
//				ε { PLUS, MINUS, LT, EQU, RSQUAREBRACKET, THEN , ELSE , FI, DO, ENDWH, RPARENTHESIS, END, SEMICOLON, COMMA }
//				| MultOp Term { MULTIPLY, DIVIDE}
RTreeNode* RSyntaxParser::OtherFactor()
{
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
		RTreeNode* n = new RTreeNode();
		n->mNodeType = NodeType::EMPTY;	//空产生式
		n->mLine = mCurLine;
		of->mChilds.push_back(n);
		LogUtil::Info(Utils::FormatCString(_T("OtherFactor is EPS near line %d"), mCurLine));
	}
	else if (GetCurToken().lex == LexType::MULTIPLY || GetCurToken().lex == LexType::DIVIDE)
	{
		of->mChilds.push_back(MultOp());
		of->mChilds.push_back(Term());
	}
	else
	{
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}
	return of;
}


// Variable ::= T-<IDENTIFIER> VariMore
RTreeNode* RSyntaxParser::Variable()
{
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
	
	return v;
}

/* 书上产生式93：Predict 集少了 RSQUAREBRACKET */
// VariMore ::= 
//				ε { ASSIGN, MULTIPLY, DIVIDE, PLUS, MINUS, LT, EQU,THEN, ELSE, FI, DO, ENDWH, RPARENTHESIS, END, SEMICOLON, COMMA, RSQUAREBRACKET }
//				| T-<LSQUAREBRACKET> Exp T-<RSQUAREBRACKET> { LSQUAREBRACKET }
//				| T-<DOT> FieldVar	{DOT}
RTreeNode* RSyntaxParser::VariMore()
{
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
		RTreeNode* n = new RTreeNode();
		n->mNodeType = NodeType::EMPTY;	//空产生式
		n->mLine = mCurLine;
		vm->mChilds.push_back(n);
		LogUtil::Info(Utils::FormatCString(_T("VariMore is EPS near line %d"), mCurLine));
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
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}
	return vm;
}

// FieldVar ::= T-<IDENTIFIER> VariMore
RTreeNode* RSyntaxParser::FieldVar()
{
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
	
	return fv;
}


// FieldVarMore ::= 
//				ε { ASSIGN, MULTIPLY, DIVIDE, PLUS, MINUS, LT, EQU,THEN, ELSE, FI, DO, ENDWH, RPARENTHESIS, END, SEMICOLON, COMMA }
//				| T-<LSQUAREBRACKET> Exp T-<RSQUAREBRACKET> { LSQUAREBRACKET }
RTreeNode* RSyntaxParser::FieldVarMore()
{
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
		RTreeNode* n = new RTreeNode();
		n->mNodeType = NodeType::EMPTY;	//空产生式
		n->mLine = mCurLine;
		fvm->mChilds.push_back(n);
		LogUtil::Info(Utils::FormatCString(_T("FieldVarMore is EPS near line %d"), mCurLine));
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
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}
	return fvm;
}


/* 书上算法部分少了这一条 */
// CmpOp ::= 
//			T-<LT>	{ LT }
//			|T-<EQU>	{ EQU }
RTreeNode* RSyntaxParser::CmpOp()
{
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
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}	

	return co;
}

// AddOp ::= 
//			T-<PLUS>	{ PLUS }
//			|T-<MINUS>	{ MINUS }
RTreeNode* RSyntaxParser::AddOp()
{
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
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	return ao;
}

// MultOp ::= 
//			T-<MULTIPLY> { MULTIPLY }
//			|T-<DIVIDE>	{ DIVIDE }
RTreeNode* RSyntaxParser::MultOp()
{
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
		LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}	

	return mo;
}

// ProgramBody ::= T-<BEGIN> StmList T-<END>
RTreeNode* RSyntaxParser::ProgramBody()
{
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

	return pb;
}

bool RSyntaxParser::Match(LexType type)
{
	if (GetCurToken().lex == type)
	{
		LogUtil::Info(Utils::FormatCString(_T("%s [%s] matched near line %d"), mLexicalAnalyzer.mLex2String[type], GetCurToken().sem, mCurLine));
		NextToken();
		mCurLine = GetCurToken().line;
		return true;
	}
	else
	{
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
	r->mSemName = t.sem;
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
