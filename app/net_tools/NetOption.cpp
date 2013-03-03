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
	DDX_Text(pDX, IDC_SMTP_ADDR, m_smtpDomain);
	DDX_Text(pDX, IDC_SMTP_PORT, m_smtpPort);
	DDX_Text(pDX, IDC_POP3_ADDR, m_pop3Domain);
	DDX_Text(pDX, IDC_POP3_PORT, m_pop3Port);
	DDX_Text(pDX, IDC_USER_ACCOUNT, m_userAccount);
	DDX_Text(pDX, IDC_USER_PASSWD, m_userPasswd);
	DDX_Text(pDX, IDC_RECIPIENTS, m_recipients);

	m_smtpAddr.Format("%s:%d", m_smtpDomain, m_smtpPort);
	m_pop3Addr.Format("%s:%d", m_pop3Domain, m_pop3Port);
}

BOOL CNetOption::OnInitDialog()
{
	return TRUE;
}

BEGIN_MESSAGE_MAP(CNetOption, CDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()

CNetOption& CNetOption::SetSmtpAddr(const char* s)
{
	ExactAddr(s, m_smtpDomain, m_smtpPort);
	return *this;
}

CNetOption& CNetOption::SetPop3Addr(const char* s)
{
	ExactAddr(s, m_pop3Domain, m_pop3Port);
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

void CNetOption::ExactAddr(const char* s, CString& domain, int& port)
{
	char  buf[256];
	_snprintf(buf, sizeof(buf), "%s", s);
	char* p = strchr(buf, ':');
	if (p == NULL || *(p + 1) == 0 || p == s)
	{
		CString msg;
		msg.Format("无效的地址格式：%s", s);
		MessageBox(msg);
		return;
	}
	*p++ = 0;
	domain = buf;
	port = atoi(p);
}

void CNetOption::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()
	UpdateData(FALSE);
}
