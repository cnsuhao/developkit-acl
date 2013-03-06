// NetOption.cpp : 实现文件
//

#include "stdafx.h"
#include "net_tools.h"
#include "NetOption.h"
#include ".\netoption.h"


// CNetOption 对话框

IMPLEMENT_DYNAMIC(CNetOption, CDialog)

CNetOption::CNetOption(CWnd* pParent /*=NULL*/)
	: CDialog(CNetOption::IDD, pParent)
	, m_smtpPort(25)
	, m_pop3Port(110)
{

}

CNetOption::~CNetOption()
{
}

void CNetOption::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SMTP_ADDR, m_smtpAddr);
	DDX_Text(pDX, IDC_SMTP_PORT, m_smtpPort);
	DDX_Text(pDX, IDC_POP3_ADDR, m_pop3Addr);
	DDX_Text(pDX, IDC_POP3_PORT, m_pop3Port);
	DDX_Text(pDX, IDC_USER_ACCOUNT, m_userAccount);
	DDX_Text(pDX, IDC_USER_PASSWD, m_userPasswd);
	DDX_Text(pDX, IDC_RECIPIENTS, m_recipients);
}

BOOL CNetOption::OnInitDialog()
{
	return TRUE;
}

BEGIN_MESSAGE_MAP(CNetOption, CDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()

CNetOption& CNetOption::SetSmtpAddr(const char* addr, int port)
{
	m_smtpAddr = addr;
	m_smtpPort = port;
	return *this;
}

CNetOption& CNetOption::SetPop3Addr(const char* addr, int port)
{
	m_pop3Addr = addr;
	m_pop3Port = port;
	return *this;
}

CNetOption& CNetOption::SetUserAccount(const char* s)
{
	m_userAccount = s;
	return *this;
}

CNetOption& CNetOption::SetUserPasswd(const char* s)
{
	m_userPasswd = s;
	return *this;
}

CNetOption& CNetOption::SetRecipients(const char* s)
{
	m_recipients = s;
	return *this;
}

void CNetOption::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()
	UpdateData(FALSE);
}
