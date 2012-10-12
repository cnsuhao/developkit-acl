// gui_rpcDlg.h : ͷ�ļ�
//

#pragma once
#include "MeterBar.h"

class acl::aio_handle;
class acl::rpc_service;

// Cgui_rpcDlg �Ի���
class Cgui_rpcDlg : public CDialog
{
// ����
public:
	Cgui_rpcDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~Cgui_rpcDlg();

// �Ի�������
	enum { IDD = IDD_GUI_RPC_DIALOG };

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

protected:
	CMeterBar m_wndMeterBar;

private:
	CWndResizer m_resizer;
	//CRect m_rect;  //���ڱ���Ի����С�仯ǰ�Ĵ�С

	CEdit m_reqCtlEdit;
	CEdit m_resCtlEdit;

	CString m_serverIp;
	CString m_serverPort;

	// �첽��Ϣ���
	acl::aio_handle* m_handle;
	// �첽 RPC ͨ�ŷ�����
	acl::rpc_service* m_rpcService;

public:
	void OnDownload(int content_length, int total_read);
	void SetRequestHdr(const char* hdr);
	void SetResponseHdr(const char* hdr);
	void DownloadOver(int total_read, double spent);
public:
	afx_msg void OnBnClickedButtonRun();
	afx_msg void OnBnClickedClear();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnEnChangeUrl();
};
