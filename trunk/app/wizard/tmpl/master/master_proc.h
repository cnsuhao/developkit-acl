#pragma once
#include "master_proc.hpp"

class acl::socket_stream;

class master_service : public acl::master_proc
{
public:
	master_service();
	~master_service();

protected:
	/**
	 * 纯虚函数：当接收到一个客户端连接时调用此函数
	 * @param stream {aio_socket_stream*} 新接收到的客户端异步流对象
	 * 注：该函数返回后，流连接将会被关闭，用户不应主动关闭该流
	 */
	virtual void on_accept(acl::socket_stream* stream);
};
