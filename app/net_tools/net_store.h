#pragma once

//////////////////////////////////////////////////////////////////////////

class net_store_callback
{
public:
	net_store_callback() {}
	virtual ~net_store_callback() {}

	virtual void load_db_callback(const char* smtp_addr,
		const char* pop3_addr,
		const char* user, const char* pass,
		const char* recipients, bool store) = 0;
private:
};
//////////////////////////////////////////////////////////////////////////

class net_store : public acl::rpc_request
{
public:
	net_store(const char* smtp_addr, const char* pop3_addr,
		const char* user, const char* pass,
		const char* recipients, net_store_callback* callback,
		bool store = false);
protected:
	~net_store();

	// 基类虚函数：子线程处理函数
	virtual void rpc_run();

	// 基类虚函数：主线程处理过程，收到子线程任务完成的消息
	virtual void rpc_onover();
private:
	acl::string smtp_addr_;
	acl::string pop3_addr_;
	acl::string user_;
	acl::string pass_;
	acl::string recipients_;
	net_store_callback* callback_;
	bool store_;

	acl::string dbpath_;

	bool create_tbl(acl::db_handle& db);
	void load_db(acl::db_handle& db);
	void save_db(acl::db_handle& db);
};