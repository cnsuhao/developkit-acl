#pragma once
#include "acl_cpp/ipc/rpc.hpp"
#include "acl_cpp/stdlib/string.hpp"

class acl::aio_handle;
class Cgui_rpcDlg;

class rpc_download : public acl::rpc_request
{
public:
	rpc_download(acl::aio_handle& handle, const char* addr,
		const char* url, Cgui_rpcDlg& dlg)
		: handle_(handle)
		, addr_(addr)
		, url_(url)
		, m_dlg(dlg)
		, error_(false)
		, total_read_(0)
		, content_length_(0)
		, total_spent_(0)
	{}
protected:
	~rpc_download() {}

	// ���̴߳�����
	virtual void rpc_run();

	// ���̴߳�����̣��յ����߳�������ɵ���Ϣ
	virtual void rpc_onover();

	// ���̴߳�����̣��յ����̵߳�֪ͨ��Ϣ
	virtual void rpc_wakeup(void* ctx);
private:
	acl::aio_handle& handle_;
	acl::string addr_;
	acl::string url_;
	acl::string req_hdr_;
	acl::string res_hdr_;
	bool  error_;
	int   total_read_;
	int   content_length_;
	double total_spent_;

	Cgui_rpcDlg& m_dlg;
};
