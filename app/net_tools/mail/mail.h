#pragma once

//////////////////////////////////////////////////////////////////////////

struct MAIL_METER
{
	double smtp_nslookup_elapsed;
	double smtp_connect_elapsed;
	double smtp_envelope_eplased;
	double smtp_auth_elapsed;
	double smtp_data_elapsed;
	double smtp_total_elapsed;

	double pop3_nslookup_elapsed;
	double pop3_connect_elapsed;
	double pop3_auth_elapsed;
	double pop3_list_elapsed;
	double pop3_total_elapsed;
};

//////////////////////////////////////////////////////////////////////////

class mail_callback
{
public:
	mail_callback() {}
	virtual ~mail_callback() {}

	virtual void mail_finish(const char* dbpath) = 0;
	virtual void mail_report(const char* msg, size_t total,
		size_t curr, const MAIL_METER& meter) = 0;
};

//////////////////////////////////////////////////////////////////////////

class mail : public acl::rpc_request
{
public:
	mail();

	mail& set_callback(mail_callback*);

	mail& set_account(const char*);
	mail& set_passwd(const char*);
	mail& set_conn_timeout(int);
	mail& set_rw_timeout(int);

	mail& set_smtp(const char*, int);
	mail& set_from(const char*);
	mail& add_to(const char*);
	mail& set_subject(const char*);
	mail& add_file(const char*);

	mail& set_pop3(const char*, int);
protected:
	~mail();

	// 基类虚函数：子线程处理函数
	virtual void rpc_run();

	// 基类虚函数：主线程处理过程，收到子线程任务完成的消息
	virtual void rpc_onover();

	// 基类虚函数：主线程处理过程，收到子线程的通知消息
	virtual void rpc_wakeup(void* ctx);
private:
	MAIL_METER meter_;
private:
	mail_callback* callback_;
	int connect_timeout_;
	int rw_timeout_;
	acl::string auth_account_;
	acl::string auth_passwd_;

	acl::string smtp_ip_;
	acl::string smtp_addr_;
	int smtp_port_;
	acl::string mail_from_;
	std::list<acl::string> recipients_;
	acl::string subject_;
	std::vector<acl::string> files_;

	acl::string pop3_ip_;
	acl::string pop3_addr_;
	int pop3_port_;

	acl::string mailpath_;
private:
	void test_smtp();
	void test_pop3();
};