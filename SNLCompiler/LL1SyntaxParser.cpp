#include "pch.h"
#include "LL1SyntaxParser.h"


LL1SyntaxParser::LL1SyntaxParser()
{
	mLexicalAnalyzer.getTokenList();
	mLexicalAnalyzer.Lex2File();
	for (int i = 0; i < mLexicalAnalyzer.mTokenList.size(); i++)
	{
		if (mLexicalAnalyzer.mTokenList[i].lex != LexType::LEXERR)
			mTokenList.push_back(mLexicalAnalyzer.mTokenList[i]);
	}
}

LL1SyntaxParser::LL1SyntaxParser(vector<Token> tokens)
{
	for (int i = 0; i < tokens.size(); i++)
	{
		if (tokens[i].lex != LexType::LEXERR)
			mTokenList.push_back(tokens[i]);
	}
}

LL1SyntaxParser::~LL1SyntaxParser()
{
}

void LL1SyntaxParser::InitMap()
{
	//1
	mLL1Map[NodeType::Program][LexType::PROGRAM] = Get1();
	// 2
	mLL1Map[NodeType::Program][LexType::PROGRAM] = Get2();
}

vector<StackItem> LL1SyntaxParser::Get1()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::ProgramHead, Token()));
	item.push_back(StackItem(NodeType::DeclarePart, Token()));
	item.push_back(StackItem(NodeType::ProgramBody, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get2()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::PROGRAM, _T(""))));
	item.push_back(StackItem(NodeType::ProgramName, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get3()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::IDENTIFIER, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get4()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::TypeDec, Token()));
	item.push_back(StackItem(NodeType::VarDec, Token()));
	item.push_back(StackItem(NodeType::ProcDec, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get5()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::EMPTY, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get6()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::TypeDec, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get7()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::TYPE, _T(""))));
	item.push_back(StackItem(NodeType::TypeDecList, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get8()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::TypeId, Token()));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::EQU, _T(""))));
	item.push_back(StackItem(NodeType::TypeDef, Token()));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::SEMICOLON, _T(""))));
	item.push_back(StackItem(NodeType::TypeDecMore, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get9()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::EMPTY, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get10()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::TypeDecList, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get11()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::IDENTIFIER, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get12()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::BaseType, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get13()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::StructureType, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get14()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::IDENTIFIER, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get15()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::INTEGER, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get16()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::CHARACTER, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get17()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::ArrayType, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get18()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::RecType, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get19()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::ARRAY, _T(""))));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::LSQUAREBRACKET, _T(""))));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::UINTEGER, _T(""))));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::UNDERANGE, _T(""))));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::UINTEGER, _T(""))));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::RSQUAREBRACKET, _T(""))));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::OF, _T(""))));
	item.push_back(StackItem(NodeType::BaseType, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get22()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::RECORD, _T(""))));
	item.push_back(StackItem(NodeType::FieldDecList, Token()));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::END, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get23()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::BaseType, Token()));
	item.push_back(StackItem(NodeType::IdList, Token()));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::SEMICOLON, _T(""))));
	item.push_back(StackItem(NodeType::FieldDecMore, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get24()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::ArrayType, Token()));
	item.push_back(StackItem(NodeType::IdList, Token()));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::SEMICOLON, _T(""))));
	item.push_back(StackItem(NodeType::FieldDecMore, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get25()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::EMPTY, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get26()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::FieldDecList, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get27()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::IDENTIFIER, _T(""))));
	item.push_back(StackItem(NodeType::IdMore, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get28()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::EMPTY, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get29()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::COMMA, _T(""))));
	item.push_back(StackItem(NodeType::IdList, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get30()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::EMPTY, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get31()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::VarDec, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get32()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::VAR, _T(""))));
	item.push_back(StackItem(NodeType::VarDecList, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get33()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::TypeDef, Token()));
	item.push_back(StackItem(NodeType::VarIdList, Token()));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::SEMICOLON, _T(""))));
	item.push_back(StackItem(NodeType::VarDecMore, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get34()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::EMPTY, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get35()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::VarDecList, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get36()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::IDENTIFIER, _T(""))));
	item.push_back(StackItem(NodeType::VarIdMore, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get37()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::EMPTY, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get38()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::COMMA, _T(""))));
	item.push_back(StackItem(NodeType::VarIdList, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get39()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::EMPTY, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get40()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::ProcDec, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get41()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::PROCEDURE, _T(""))));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::IDENTIFIER, _T(""))));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::LPARENTHESIS, _T(""))));
	item.push_back(StackItem(NodeType::ParamList, Token()));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::RPARENTHESIS, _T(""))));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::SEMICOLON, _T(""))));
	item.push_back(StackItem(NodeType::ProcDecPart, Token()));
	item.push_back(StackItem(NodeType::ProcBody, Token()));
	item.push_back(StackItem(NodeType::ProcDecMore, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get42()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::EMPTY, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get43()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::ProcDeclaration, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get44()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::IDENTIFIER, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get45()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::EMPTY, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get46()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::ParamDecList, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get47()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Param, Token()));
	item.push_back(StackItem(NodeType::ParamMore, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get48()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::EMPTY, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get49()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::SEMICOLON, _T(""))));
	item.push_back(StackItem(NodeType::ParamDecList, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get50()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::TypeDef, Token()));
	item.push_back(StackItem(NodeType::FormList, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get51()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::VAR, _T(""))));
	item.push_back(StackItem(NodeType::TypeDef, Token()));
	item.push_back(StackItem(NodeType::FormList, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get52()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::IDENTIFIER, _T(""))));
	item.push_back(StackItem(NodeType::FidMore, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get53()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::EMPTY, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get54()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::COMMA, _T(""))));
	item.push_back(StackItem(NodeType::FormList, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get55()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::DeclarePart, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get56()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::ProgramBody, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get57()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::BEGIN, _T(""))));
	item.push_back(StackItem(NodeType::StmList, Token()));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::END, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get58()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Stm, Token()));
	item.push_back(StackItem(NodeType::StmMore, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get59()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::EMPTY, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get60()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::SEMICOLON, _T(""))));
	item.push_back(StackItem(NodeType::StmList, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get61()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::ConditionalStm, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get62()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::LoopStm, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get63()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::InputStm, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get64()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::OutputStm, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get65()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::ReturnStm, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get66()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::IDENTIFIER, _T(""))));
	item.push_back(StackItem(NodeType::AssCall, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get67()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::AssignmentRest, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get68()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::CallStmRest, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get69()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::VariMore, Token()));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::ASSIGN, _T(""))));
	item.push_back(StackItem(NodeType::Exp, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get70()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::IF, _T(""))));
	item.push_back(StackItem(NodeType::RelExp, Token()));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::THEN, _T(""))));
	item.push_back(StackItem(NodeType::StmList, Token()));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::ELSE, _T(""))));
	item.push_back(StackItem(NodeType::StmList, Token()));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::FI, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get71()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::WHILE, _T(""))));
	item.push_back(StackItem(NodeType::RelExp, Token()));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::DO, _T(""))));
	item.push_back(StackItem(NodeType::StmList, Token()));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::ENDWHILE, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get72()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::READ, _T(""))));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::LPARENTHESIS, _T(""))));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::IDENTIFIER, _T(""))));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::RPARENTHESIS, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get73()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::IDENTIFIER, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get74()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::WRITE, _T(""))));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::LPARENTHESIS, _T(""))));
	item.push_back(StackItem(NodeType::Exp, Token()));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::RPARENTHESIS, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get75()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::RETURN, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get76()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::LPARENTHESIS, _T(""))));
	item.push_back(StackItem(NodeType::ActParamList, Token()));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::RPARENTHESIS, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get77()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::EMPTY, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get78()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Exp, Token()));
	item.push_back(StackItem(NodeType::ActParamMore, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get79()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::EMPTY, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get80()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::COMMA, _T(""))));
	item.push_back(StackItem(NodeType::ActParamList, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get81()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Exp, Token()));
	item.push_back(StackItem(NodeType::OtherRelE, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get82()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::CmpOp, Token()));
	item.push_back(StackItem(NodeType::Exp, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get83()
{
	vector<StackItem> item;
		item.push_back(StackItem(NodeType::Term, Token()));
		item.push_back(StackItem(NodeType::OtherTerm, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get84()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::EMPTY, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get85()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::AddOp, Token()));
	item.push_back(StackItem(NodeType::Exp, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get86()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Factor, Token()));
	item.push_back(StackItem(NodeType::OtherFactor, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get87()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::EMPTY, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get88()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::MultOp, Token()));
	item.push_back(StackItem(NodeType::Term, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get89()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::LPARENTHESIS, _T(""))));
	item.push_back(StackItem(NodeType::Exp, Token()));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::RPARENTHESIS, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get90()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::UINTEGER, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get91()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Variable, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get92()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::IDENTIFIER, _T(""))));
	item.push_back(StackItem(NodeType::VariMore, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get93()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::EMPTY, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get94()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::LSQUAREBRACKET, _T(""))));
	item.push_back(StackItem(NodeType::Exp, Token()));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::RSQUAREBRACKET, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get95()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::DOT, _T(""))));
	item.push_back(StackItem(NodeType::FieldVar, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get96()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::IDENTIFIER, _T(""))));
	item.push_back(StackItem(NodeType::FieldVarMore, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get97()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::EMPTY, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get98()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::LSQUAREBRACKET, _T(""))));
	item.push_back(StackItem(NodeType::Exp, Token()));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::RSQUAREBRACKET, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get99()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::LT, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get100()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::EQU, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get101()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::PLUS, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get102()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::MINUS, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get103()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::MULTIPLY, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get104()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::DIVIDE, _T(""))));
	return item;
}
