#pragma once
#include "master_aio.hpp"

class acl::aio_socket_stream;

class master_service : public acl::master_aio
{
public:
	master_service();
	~master_service();

protected:
	/**
	 * 纯虚函数：当接收到一个客户端连接时调用此函数
	 * @param stream {aio_socket_stream*} 新接收到的客户端异步流对象
	 * @return {bool} 该函数如果返回 false 则通知服务器框架不再接收
	 *  远程客户端连接，否则继续接收客户端连接
	 */
	bool on_accept(acl::aio_socket_stream* stream);
};
