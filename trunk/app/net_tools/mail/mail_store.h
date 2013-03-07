#pragma once

struct MAIL_METER;
class mail_callback;
class mail_store : public acl::rpc_request
{
public:
	mail_store(const char* user, const char* smtp_ip, const char* pop3_ip,
		const MAIL_METER& meter, mail_callback& callback);
protected:
	~mail_store();

	// 基类虚函数：子线程处理函数
	virtual void rpc_run();

	// 基类虚函数：主线程处理过程，收到子线程任务完成的消息
	virtual void rpc_onover();

private:
	bool ok_;
	char* user_;
	char* smtp_ip_;
	char* pop3_ip_;
	MAIL_METER* meter_;
	mail_callback& callback_;
	acl::string dbpath_;
	bool create_tbl(acl::db_handle& db);
	void insert_tbl(acl::db_handle& db);
};