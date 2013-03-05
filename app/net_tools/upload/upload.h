#pragma once

//////////////////////////////////////////////////////////////////////////

class upload_callback
{
public:
	upload_callback() {}
	virtual ~upload_callback() {}

	virtual void upload_report(const char* msg, size_t total, size_t curr) = 0;
protected:
private:
};

//////////////////////////////////////////////////////////////////////////

class upload : public acl::rpc_request
{
public:
	upload();
	upload& set_callback(upload_callback*);
	upload& set_dbpath(const char*);
	upload& set_server(const char*);
	upload& set_conn_timeout(int);
	upload& set_rw_timeout(int);
	upload& set_from(const char*);
	upload& set_account(const char*);
	upload& set_passwd(const char*);
	upload& set_subject(const char*);
	upload& add_to(const char*);
protected:
	~upload();

	// 基类虚函数：子线程处理函数
	virtual void rpc_run();

	// 基类虚函数：主线程处理过程，收到子线程任务完成的消息
	virtual void rpc_onover();

	// 基类虚函数：主线程处理过程，收到子线程的通知消息
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
