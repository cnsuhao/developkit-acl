#include "stdafx.h"
#include "ClientConnection.h"
#include "ClientManager.h"

void ClientManager::set(ClientConnection* conn)
{
	std::vector<ClientConnection*>::iterator it = conns_.begin();
	for (; it != conns_.end(); ++it)
	{
		if ((*it) == conn)
			logger_fatal("duplicate ClientConnection!");
	}

	conns_.push_back(conn);
}

void ClientManager::del(ClientConnection* conn)
{
	std::vector<ClientConnection*>::iterator it = conns_.begin();
	for (; it != conns_.end(); ++it)
	{
		if ((*it) == conn)
		{
			conns_.erase(it);
			break;
		}
	}
}

ClientConnection* ClientManager::pop()
{
	if (conns_.empty())
		return NULL;
	return conns_[0];
}
