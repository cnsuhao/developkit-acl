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
	if (ManagerTimer::transfer(this) == false)
		ClientManager::get_instance().set(this);
	else
		delete this;
}
