#pragma once

class http_thread : public acl::thread
{
public:
	http_thread(const char* domain, const char* ip, int port,
		const char* url, double spent_dns);
	~http_thread();

protected:
	virtual void* run();

private:
	acl::string domain_;
	acl::string ip_;
	int   port_;
	acl::string url_;
	double spent_dns_;
	double spent_connect_;
	double spent_http_;
	double spent_total_;
	bool success_;
	char   addr_[256];

	// 连接服务器
	acl::socket_stream* connect_server();

	// 发送 HTTP 请求过程
	bool http_request(acl::socket_stream* conn, const char* host);

	// 结果插入数据库
	void dbsave();
	// 创建数据表
	bool dbtbl_create(acl::db_handle& db);
	// 更新数据表
	void dbtbl_update(acl::db_handle& db);
};
