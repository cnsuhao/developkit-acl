#include "stdafx.h"
#include "ClientManager.h"
#include "ServerManager.h"
#include "ClientConnection.h"
#include "ServerConnection.h"
#include "ManagerTimer.h"

void ManagerTimer::destroy()
{
	delete this;
}

bool ManagerTimer::transfer(ClientConnection* client)
{
	ServerConnection* server;
	const char* peer;
	char  buf[256];
	int   ret;

	while (true)
	{
		server = ServerManager::get_instance().min();
		if (server == NULL)
			return false;

		peer = client->get_peer();
		if (peer == NULL)
			peer = "unkonwn";
		snprintf(buf, sizeof(buf), "%s", peer);

		ret = acl_write_fd(server->sock_handle(), buf,
			strlen(buf), client->sock_handle());
		if (ret == -1)
		{
			ServerManager::get_instance().del(server);
			server->close();
		}

		server->inc_nconns();
		return true;
	}
}

void ManagerTimer::timer_callback(unsigned int)
{
	ClientConnection* client;

	while (true)
	{
		client = ClientManager::get_instance().pop();
		if (client == NULL)
			break;
		if (transfer(client) == false)
		{
			ClientManager::get_instance().set(client);
			break;
		}
		delete client;
	}
}
