// gui_rpcDlg.h : 头文件
//

#pragma once
#include "MeterBar.h"

class acl::aio_handle;
class acl::rpc_service;

// Cgui_rpcDlg 对话框
class Cgui_rpcDlg : public CDialog
{
// 构造
public:
	Cgui_rpcDlg(CWnd* pParent = NULL);	// 标准构造函数
	~Cgui_rpcDlg();

// 对话框数据
	enum { IDD = IDD_GUI_RPC_DIALOG };

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

protected:
	CMeterBar m_wndMeterBar;

private:
	CWndResizer m_resizer;
	//CRect m_rect;  //用于保存对话框大小变化前的大小

	CEdit m_reqCtlEdit;
	CEdit m_resCtlEdit;

	CString m_serverIp;
	CString m_serverPort;

	// 异步消息句柄
	acl::aio_handle* m_handle;
	// 异步 RPC 通信服务句柄
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
