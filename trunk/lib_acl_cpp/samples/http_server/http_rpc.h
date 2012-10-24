#pragma once
#include "rpc.hpp"
#include "aio_socket_stream.hpp"
#include "socket_stream.hpp"
//class acl::aio_socket_stream;
//class acl::socket_stream;

class http_rpc : public acl::rpc_request
{
public:
	http_rpc(acl::aio_socket_stream* client);
	~http_rpc() {}

protected:
	// 子线程处理函数
	virtual void rpc_run();

	// 主线程处理过程，收到子线程任务完成的消息
	virtual void rpc_onover();

private:
	bool proc_quit_;  // 程序是否应该退出
	acl::aio_handle& handle_;  // 异步框架引擎
	acl::aio_socket_stream* client_;  // 客户端连接流
	bool keep_alive_; // 是否与客户端保持长连接

	// 在子线程中以阻塞方式处理客户端请求
	void handle_conn(acl::socket_stream* stream);
};
