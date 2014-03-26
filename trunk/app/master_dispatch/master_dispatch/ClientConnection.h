#pragma once
#include "IConnection.h"

class ClientConnection : public IConnection
{
public:
	ClientConnection(acl::aio_socket_stream* conn);
	~ClientConnection() {}

protected:
	// 基类纯虚函数
	void run();
};
