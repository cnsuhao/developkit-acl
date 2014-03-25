#include "stdafx.h"
#include "ServerManager.h"
#include "ServerConnection.h"
#include "ServerIOCallback.h"

ServerIOCallback::ServerIOCallback(ServerConnection* conn)
: conn_(conn)
{
}

ServerIOCallback::~ServerIOCallback()
{
	ServerManager::get_instance().del(conn_);
	delete conn_;
}

bool ServerIOCallback::read_callback(char* data, int len)
{
	if (data == NULL || *data == 0 || len <= 0)
	{
		logger_warn("invalid data: %s, len: %d",
			data ? data : "null", len);
		return false;
	}

	acl::url_coder coder;
	coder.decode(data);

	const char* ptr = coder.get("nconnections");
	if (ptr == NULL)
	{
		logger_warn("no nconnecttions");
	}

	unsigned int nconns = (unsigned int) atoi(ptr);
	conn_->set_nconns(nconns);

	ServerManager::get_instance().set(conn_);

	return true;
}

void ServerIOCallback::close_callback()
{
}

bool ServerIOCallback::timeout_callback()
{
	return true;
}
