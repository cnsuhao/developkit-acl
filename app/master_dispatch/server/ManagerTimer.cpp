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

	// 从服务端连接管理对象中取得连接数最小的一个
	// 服务端对象，并将所给客户端连接传递给它，
	// 一直到成功或所有传输都失败为止

	while (true)
	{
		server = ServerManager::get_instance().min();
		if (server == NULL)
			return false;

		peer = client->get_peer();
		if (peer == NULL)
			peer = "unkonwn";
		memset(buf, 0, sizeof(buf));
		snprintf(buf, sizeof(buf), "%s", peer);

		// 将客户端连接传递给服务端，如果失败，则尝试下一个
		// 服务端，同时将失败的服务端从服务端管理集合中删除
		ret = acl_write_fd(server->sock_handle(), buf,
			strlen(buf), client->sock_handle());
		if (ret == -1)
		{
			ServerManager::get_instance().del(server);
			server->close();
		}
		else
			server->inc_nconns();

		return true;
	}
}

void ManagerTimer::timer_callback(unsigned int)
{
	ClientConnection* client;

	// 从客户端管理对象弹出所有延迟待处理的客户端连接对象
	// 并传递给服务端，如果传递失败，则再次置入客户端管理
	// 对象，由下次定时器再次尝试处理

	logger("total client: %d, total server: %d",
		(int) ClientManager::get_instance().length(),
		(int) ServerManager::get_instance().length());

	while (true)
	{
		client = ClientManager::get_instance().pop();
		if (client == NULL)
			break;

		if (transfer(client) == true)
		{
			ClientManager::get_instance().del(client);
			delete client;
			continue;
		}

		// 如果在规定的时间内依然没有服务端准备接收连接，
		// 则直接删除该对象
		if (client->expired())
		{
			logger_warn("no server side, client(%s) expired!",
				client->get_peer());
			delete client;
		}
		else
			ClientManager::get_instance().set(client);
		break;
	}
}
