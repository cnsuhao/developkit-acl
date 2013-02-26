// net_toolsDlg.h : ͷ�ļ�
//

#pragma once
#include "ping/ping.h"
#include "dns/nslookup.h"

// Cnet_toolsDlg �Ի���
class Cnet_toolsDlg : public CDialog
	, public ping_callback
	, public nslookup_callback
{
// ����
public:
	Cnet_toolsDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~Cnet_toolsDlg();
// �Ի�������
	enum { IDD = IDD_NET_TOOLS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
	// ping ��ز���
	UINT m_nPkt;
	UINT m_delay;
	UINT m_pingTimeout;
	FILE* m_dosFp;

	// dns ��ز���
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
