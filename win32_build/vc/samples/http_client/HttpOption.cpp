// HttpOption.cpp : 实现文件
//

#include "stdafx.h"
#include "http_client.h"
#include "HttpOption.h"
#include ".\httpoption.h"


// CHttpOption 对话框

IMPLEMENT_DYNAMIC(CHttpOption, CDialog)
CHttpOption::CHttpOption(CWnd* pParent /*=NULL*/)
	: CDialog(CHttpOption::IDD, pParent)
	, m_bHttp11(FALSE)
	, m_bZip(FALSE)
	, m_bKeepAlive(FALSE)
	, m_sHttpHdrAppend(_T(""))
	, m_sServerAddr(_T(""))
	, m_bUseAddr(FALSE)
	, m_bForwardAuto(FALSE)
	, m_nMaxTry(10)
{
}

CHttpOption::~CHttpOption()
{
}

void CHttpOption::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_CHECK_PROTO, m_bHttp11);
	DDX_Check(pDX, IDC_CHECK_ZIP, m_bZip);
	DDX_Check(pDX, IDC_CHECK_KEEP_ALIVE, m_bKeepAlive);
	DDX_Text(pDX, IDC_EDIT_HTTP_HDR, m_sHttpHdrAppend);
	DDX_Text(pDX, IDC_EDIT_ADDR, m_sServerAddr);
	DDX_Check(pDX, IDC_CHECK_USE_ADDR, m_bUseAddr);
	DDX_Check(pDX, IDC_CHECK_FORWARD, m_bForwardAuto);
	DDX_Text(pDX, IDC_EDIT_MAX_TRY, m_nMaxTry);
}


BEGIN_MESSAGE_MAP(CHttpOption, CDialog)
	ON_BN_CLICKED(IDC_CHECK_PROTO, OnBnClickedCheckProto)
	ON_BN_CLICKED(IDC_CHECK_ZIP, OnBnClickedCheckZip)
	ON_BN_CLICKED(IDC_CHECK_KEEP_ALIVE, OnBnClickedCheckKeepAlive)
END_MESSAGE_MAP()


// CHttpOption 消息处理程序

void CHttpOption::OnBnClickedCheckProto()
{
	// TODO: 在此添加控件通知处理程序代码
	if (IsDlgButtonChecked(IDC_CHECK_PROTO))
		m_bHttp11 = TRUE;
	else
		m_bHttp11 = FALSE;
}

void CHttpOption::OnBnClickedCheckZip()
{
	// TODO: 在此添加控件通知处理程序代码
	if (IsDlgButtonChecked(IDC_CHECK_ZIP))
		m_bZip = TRUE;
	else
		m_bZip = FALSE;
}

void CHttpOption::OnBnClickedCheckKeepAlive()
{
	// TODO: 在此添加控件通知处理程序代码
	if (IsDlgButtonChecked(IDC_CHECK_KEEP_ALIVE))
		m_bKeepAlive = TRUE;
	else
		m_bKeepAlive = FALSE;
}

BOOL CHttpOption::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

