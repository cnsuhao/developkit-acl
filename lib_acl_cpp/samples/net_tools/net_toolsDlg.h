// net_toolsDlg.h : ͷ�ļ�
//

#pragma once
#include "ping/ping.h"

// Cnet_toolsDlg �Ի���
class Cnet_toolsDlg : public CDialog
	, public rpc_callback
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
	UINT m_nPkt;
	UINT m_delay;
	UINT m_timeout;
	FILE* m_dosFp;
	void DisableAll();

public:
	virtual void enable_ping();
	afx_msg void OnBnClickedOpenDos();
};
