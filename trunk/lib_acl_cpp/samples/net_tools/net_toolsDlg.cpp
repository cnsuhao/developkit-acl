// net_toolsDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "net_tools.h"
#include "ping/ping.h"
#include "dns/nslookup.h"
#include "upload/upload.h"
#include "rpc/rpc_manager.h"
#include "net_toolsDlg.h"
#include ".\net_toolsdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// Cnet_toolsDlg �Ի���



Cnet_toolsDlg::Cnet_toolsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(Cnet_toolsDlg::IDD, pParent)
	, m_nPkt(10)
	, m_delay(1)
	, m_pingTimeout(5)
	, m_pingBusy(FALSE)
	, m_dosFp(NULL)
	, m_dnsIp("8.8.8.8")
	, m_dnsPort(53)
	, m_lookupTimeout(10)
	, m_pktSize(64)
	, m_dnsBusy(FALSE)
	, m_smtpAddr("smtp.263.net")
	, m_connecTimeout(60)
	, m_rwTimeout(60)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

Cnet_toolsDlg::~Cnet_toolsDlg()
{
	if (m_dosFp)
	{
		fclose(m_dosFp);
		FreeConsole();
	}
}

void Cnet_toolsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_NPKT, m_nPkt);
	DDX_Text(pDX, IDC_DELAY, m_delay);
	DDX_Text(pDX, IDC_TIMEOUT, m_pingTimeout);
	DDX_Text(pDX, IDC_DNS_IP, m_dnsIp);
	DDX_Text(pDX, IDC_DNS_PORT, m_dnsPort);
	DDX_Text(pDX, IDC_LOOKUP_TIMEOUT, m_lookupTimeout);
	DDX_Text(pDX, IDC_PKT_SIZE, m_pktSize);
}

BEGIN_MESSAGE_MAP(Cnet_toolsDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_LOAD_IP, OnBnClickedLoadIp)
	ON_BN_CLICKED(IDC_PING, OnBnClickedPing)
	ON_BN_CLICKED(IDC_LOAD_DOMAIN, OnBnClickedLoadDomain)
	ON_BN_CLICKED(IDC_NSLOOKUP, OnBnClickedNslookup)
	ON_BN_CLICKED(IDC_OPEN_DOS, OnBnClickedOpenDos)
END_MESSAGE_MAP()


// Cnet_toolsDlg ��Ϣ�������

BOOL Cnet_toolsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��\������...\���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	//ShowWindow(SW_MAXIMIZE);

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	// ���״̬��
	int aWidths[3] = {50, 300, -1};
	m_wndMeterBar.SetParts(3, aWidths);

	m_wndMeterBar.Create(WS_CHILD | WS_VISIBLE | WS_BORDER
		| CCS_BOTTOM | SBARS_SIZEGRIP,
		CRect(0,0,0,0), this, 0); 
	m_wndMeterBar.SetText("����", 0, 0);
	m_wndMeterBar.SetText("", 1, 0);
	m_wndMeterBar.SetText("", 2, 0);

	DisableAll();

	return TRUE;  // ���������˿ؼ��Ľ��㣬���򷵻� TRUE
}

void Cnet_toolsDlg::DisableAll()
{
	GetDlgItem(IDC_PING)->EnableWindow(FALSE);
	GetDlgItem(IDC_NSLOOKUP)->EnableWindow(FALSE);
}

void Cnet_toolsDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void Cnet_toolsDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
HCURSOR Cnet_toolsDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void Cnet_toolsDlg::OnBnClickedLoadIp()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CFileDialog file(TRUE,"�ļ�","",OFN_HIDEREADONLY,"FILE(*.*)|*.*||",NULL);
	if(file.DoModal()==IDOK)
	{
		CString pathname;

		pathname=file.GetPathName();
		GetDlgItem(IDC_IP_FILE_PATH)->SetWindowText(pathname);
		GetDlgItem(IDC_PING)->EnableWindow(TRUE);
	}
}

void Cnet_toolsDlg::OnBnClickedPing()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_pingBusy)
		return;

	UpdateData();

	GetDlgItem(IDC_PING)->EnableWindow(FALSE);

	CString filePath;
	GetDlgItem(IDC_IP_FILE_PATH)->GetWindowText(filePath);
	if (filePath.IsEmpty())
	{
		MessageBox("����ѡ�� ip �б������ļ���");

		return;
	}

	m_pingBusy = TRUE;

	GetDlgItem(IDC_LOAD_IP)->EnableWindow(FALSE);
	logger("npkt: %d, delay: %d, timeout: %d",
		m_nPkt, m_delay, m_pingTimeout);

	ping* p = new ping(filePath.GetString(), this,
		m_nPkt, m_delay, m_pingTimeout, m_pktSize);
	rpc_manager::get_instance().fork(p);
}

void Cnet_toolsDlg::ping_report(size_t total, size_t curr, size_t nerror)
{
	if (total > 0)
	{
		int  nStept;

		nStept = (int) ((curr * 100) / total);
		m_wndMeterBar.GetProgressCtrl().SetPos(nStept);
	}

	CString msg;
	msg.Format("%d/%d; failed: %d", curr, total, nerror);
	m_wndMeterBar.SetText(msg, 1, 0);
}

void Cnet_toolsDlg::enable_ping()
{
	m_pingBusy = FALSE;

	GetDlgItem(IDC_LOAD_IP)->EnableWindow(TRUE);
	CString filePath;
	GetDlgItem(IDC_IP_FILE_PATH)->GetWindowText(filePath);
	if (filePath.IsEmpty())
		GetDlgItem(IDC_PING)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_PING)->EnableWindow(TRUE);
}

void Cnet_toolsDlg::OnBnClickedLoadDomain()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CFileDialog file(TRUE,"�ļ�","",OFN_HIDEREADONLY,"FILE(*.*)|*.*||",NULL);
	if(file.DoModal()==IDOK)
	{
		CString pathname;

		pathname=file.GetPathName();
		GetDlgItem(IDC_DOMAIN_FILE)->SetWindowText(pathname);
		GetDlgItem(IDC_NSLOOKUP)->EnableWindow(TRUE);
	}
}

void Cnet_toolsDlg::OnBnClickedNslookup()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	if (m_dnsBusy)
		return;

	UpdateData();

	GetDlgItem(IDC_NSLOOKUP)->EnableWindow(FALSE);

	CString filePath;
	GetDlgItem(IDC_DOMAIN_FILE)->GetWindowText(filePath);
	if (filePath.IsEmpty())
	{
		MessageBox("����ѡ�������б������ļ���");
		return;
	}

	m_dnsBusy = TRUE;

	GetDlgItem(IDC_LOAD_DOMAIN)->EnableWindow(FALSE);

	logger("dns_ip: %s, dns_port: %d, dns_timeout: %d",
		m_dnsIp.GetString(), m_dnsPort, m_lookupTimeout);

	nslookup* dns = new nslookup(filePath.GetString(), this,
		m_dnsIp.GetString(), m_dnsPort, m_lookupTimeout);
	rpc_manager::get_instance().fork(dns);
}

void Cnet_toolsDlg::nslookup_report(size_t total, size_t curr)
{
	if (total > 0)
	{
		int  nStept;

		nStept = (int) ((curr * 100) / total);
		m_wndMeterBar.GetProgressCtrl().SetPos(nStept);
	}

	CString msg;
	msg.Format("�� %d ������, ��� %d ������", total, curr);
	m_wndMeterBar.SetText(msg, 1, 0);
}

void Cnet_toolsDlg::enable_nslookup(const char* dbpath)
{
	m_dnsBusy = FALSE;

	GetDlgItem(IDC_LOAD_DOMAIN)->EnableWindow(TRUE);
	CString filePath;
	GetDlgItem(IDC_DOMAIN_FILE)->GetWindowText(filePath);
	if (filePath.IsEmpty())
		GetDlgItem(IDC_NSLOOKUP)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_NSLOOKUP)->EnableWindow(TRUE);

	if (dbpath && *dbpath)
	{
		// �����ݿ��ļ����ʼ���������
		upload* up = new upload();
		(*up).set_callback(this)
			.set_dbpath(dbpath)
			.set_server(m_smtpAddr.GetString())
			.set_conn_timeout(m_connecTimeout)
			.set_rw_timeout(m_rwTimeout)
			.set_account(m_smtpUser.GetString())
			.set_passwd(m_smtpPass.GetString())
			.set_from(m_smtpUser.GetString())
			.set_subject("DNS ��ѯ�������")
			.add_to(m_toUser.GetString());
		rpc_manager::get_instance().fork(up);
	}
}

void Cnet_toolsDlg::OnBnClickedOpenDos()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_dosFp == NULL)
	{
		//GetDlgItem(IDC_OPEN_DOS)->EnableWindow(FALSE);
		AllocConsole();
		m_dosFp = freopen("CONOUT$","w+t",stdout);
		printf("DOS opened now!\r\n");
		const char* path = acl_getcwd();
		printf("current path: %s\r\n", path);
		GetDlgItem(IDC_OPEN_DOS)->SetWindowText("�ر� DOS ����");
		acl::log::stdout_open(true);
		logger_close();
	}
	else
	{
		fclose(m_dosFp);
		m_dosFp = NULL;
		FreeConsole();
		GetDlgItem(IDC_OPEN_DOS)->SetWindowText("�� DOS ����");
		acl::log::stdout_open(false);
		const char* path = acl_getcwd();
		acl::string logpath;
		logpath.format("%s/net_tools.txt", path);
		printf("current path: %s\r\n", path);
		logger_open(logpath.c_str(), "net_tools");
	}
}

void Cnet_toolsDlg::upload_report()
{

}