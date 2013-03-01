#pragma once

//////////////////////////////////////////////////////////////////////////

class upload_callback
{
public:
	upload_callback() {}
	virtual ~upload_callback() {}

	virtual void upload_report() = 0;
protected:
private:
};

//////////////////////////////////////////////////////////////////////////

class upload : public acl::rpc_request
{
public:
	upload(upload_callback* callback, const char* dbpath,
		const char* smtpAddr, int connectTimeout, int rwTimeout);
protected:
	~upload();

	// �����麯�������̴߳�����
	virtual void rpc_run();

	// �����麯�������̴߳�����̣��յ����߳�������ɵ���Ϣ
	virtual void rpc_onover();

	// �����麯�������̴߳�����̣��յ����̵߳�֪ͨ��Ϣ
	virtual void rpc_wakeup(void* ctx);
private:
	upload_callback* callback_;
	acl::string dbpath_;
	acl::string smtp_addr_;
	int connect_timeout_;
	int rw_timeout_;
	acl::string auth_account_;
	acl::string auth_passwd_;
	acl::string mail_from_;
	std::list<acl::string> recipients_;
	acl::string subject_;
	acl::string mailpath_;
};
//////////////////////////////////////////////////////////////////////////
