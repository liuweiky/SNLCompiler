// SyntaxTreeDlg.cpp: 实现文件
//

#include "pch.h"
#include "SNLCompiler.h"
#include "SyntaxTreeDlg.h"
#include "afxdialogex.h"


// SyntaxTreeDlg 对话框

IMPLEMENT_DYNAMIC(SyntaxTreeDlg, CDialogEx)

SyntaxTreeDlg::SyntaxTreeDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SYNTAXTREE_DIALOG, pParent)
{

}

SyntaxTreeDlg::~SyntaxTreeDlg()
{
}

void SyntaxTreeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SYNTAX_TREE_EDIT, mSyntaxTreeEdit);
}


BEGIN_MESSAGE_MAP(SyntaxTreeDlg, CDialogEx)
	ON_EN_CHANGE(IDC_TREEEDIT, &SyntaxTreeDlg::OnEnChangeTreeedit)
	ON_EN_CHANGE(IDC_SYNTAX_TREE_EDIT, &SyntaxTreeDlg::OnEnChangeSyntaxTreeEdit)
END_MESSAGE_MAP()


// SyntaxTreeDlg 消息处理程序


void SyntaxTreeDlg::OnEnChangeTreeedit()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


void SyntaxTreeDlg::OnEnChangeSyntaxTreeEdit()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}
