#include "stdafx.h"
#include "ManagerTimer.h"
#include "ClientManager.h"
#include "ServerManager.h"
#include "ServerConnection.h"
#include "ClientConnection.h"

ClientConnection::ClientConnection(acl::aio_socket_stream* conn)
: IConnection(conn)
{
	conn_->close();
}

void ClientConnection::run()
{
	// 调用描述字发送过程将客户端套接字传给服务端
	if (ManagerTimer::transfer(this) == false)
		// 如果传输描述字失败，则加入待处理队列，由定时器
		// 进行处理
		ClientManager::get_instance().set(this);
	else
		delete this;
}
