// net_toolsDlg.h : ͷ�ļ�
//

#pragma once
#include "ui/MeterBar.h"
#include "ping/ping.h"
#include "upload/upload.h"
#include "dns/nslookup.h"

// Cnet_toolsDlg �Ի���
class Cnet_toolsDlg : public CDialog
	, public ping_callback
	, public nslookup_callback
	, public upload_callback
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
	CMeterBar m_wndMeterBar;

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
	UINT m_pktSize;
	FILE* m_dosFp;
	BOOL m_pingBusy;
	CString m_pingDbPath;

	// dns ��ز���
	CString m_dnsIp;
	UINT m_dnsPort;
	UINT m_lookupTimeout;
	BOOL m_dnsBusy;
	CString m_dnsDbPath;

	// �ϴ���־��ز���
	CString m_smtpAddr;
	int m_connecTimeout;
	int m_rwTimeout;
	CString m_smtpUser;
	CString m_smtpPass;

protected:
	void DisableAll();
	virtual void ping_report(size_t total, size_t curr, size_t nerror);
	virtual void nslookup_report(size_t total, size_t curr);
	virtual void upload_report();

public:
	virtual void enable_ping();
	virtual void enable_nslookup();
	afx_msg void OnBnClickedOpenDos();
	afx_msg void OnBnClickedUpload();
};
