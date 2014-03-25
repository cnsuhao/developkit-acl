#include "stdafx.h"
#include "ServerIOCallback.h"
#include "ServerConnection.h"

ServerConnection::ServerConnection(acl::aio_socket_stream* conn)
: IConnection(conn)
, nconns_(0)
{
}

void ServerConnection::run()
{
	ServerIOCallback* callback = new ServerIOCallback(this);

	conn_->add_read_callback(callback);
	conn_->add_close_callback(callback);
	conn_->add_timeout_callback(callback);
	conn_->gets();
}

void ServerConnection::set_nconns(unsigned int nconns)
{
	nconns_ = nconns;
}

void ServerConnection::close()
{
	conn_->close();
}

void ServerConnection::inc_nconns()
{
	nconns_++;
}
