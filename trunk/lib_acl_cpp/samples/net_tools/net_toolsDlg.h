// net_toolsDlg.h : 头文件
//

#pragma once
#include "ping/ping.h"
#include "dns/nslookup.h"

// Cnet_toolsDlg 对话框
class Cnet_toolsDlg : public CDialog
	, public ping_callback
	, public nslookup_callback
{
// 构造
public:
	Cnet_toolsDlg(CWnd* pParent = NULL);	// 标准构造函数
	~Cnet_toolsDlg();
// 对话框数据
	enum { IDD = IDD_NET_TOOLS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedLoadIp();
	afx_msg void OnBnClickedPing();
	afx_msg void OnBnClickedLoadDomain();
	afx_msg void OnBnClickedNslookup();

private:
	// ping 相关参数
	UINT m_nPkt;
	UINT m_delay;
	UINT m_pingTimeout;
	FILE* m_dosFp;

	// dns 相关参数
	CString m_dnsIp;
	UINT m_dnsPort;
	UINT m_lookupTimeout;

public:
	void DisableAll();

public:
	virtual void enable_ping();
	virtual void enable_nslookup();
	afx_msg void OnBnClickedOpenDos();
};
