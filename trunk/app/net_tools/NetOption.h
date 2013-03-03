#pragma once


// CNetOption 对话框

class CNetOption : public CDialog
{
	DECLARE_DYNAMIC(CNetOption)

public:
	CNetOption(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CNetOption();

// 对话框数据
	enum { IDD = IDD_OPTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	CNetOption& SetUserPasswd(const char* s);
	CNetOption& SetSmtpAddr(const char* s);
	CNetOption& SetPop3Addr(const char* s);
	CNetOption& SetUserAccount(const char* s);
	CNetOption& SetRecipients(const char* s);

	const CString& GetSmtpAddr() const
	{
		return m_smtpAddr;
	}
	const CString& GetPop3Addr() const
	{
		return m_pop3Addr;
	}
	const CString& GetUserAccount() const
	{
		return m_userAccount;
	}
	const CString& GetUserPasswd() const
	{
		return m_userPasswd;
	}
	const CString& GetRecipients() const
	{
		return m_recipients;
	}
//private:
	CString m_smtpAddr;
	CString m_smtpDomain;
	int   m_smtpPort;
	CString m_pop3Addr;
	CString m_pop3Domain;
	int   m_pop3Port;
	CString m_userAccount;
	CString m_userPasswd;
	CString m_recipients;

	void ExactAddr(const char* s, CString& domain, int& port);
public:
	afx_msg void OnPaint();
};
