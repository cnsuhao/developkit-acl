#include "stdafx.h"
#include "server/ServerConnection.h"
#include "server/ServerManager.h"

ServerConnection* ServerManager::min()
{
	if (conns_.empty())
		return NULL;

	ServerConnection* conn = NULL;

	std::vector<ServerConnection*>::iterator it = conns_.begin();
	for (; it != conns_.end(); ++it)
	{
		if (conn == NULL || (*it)->get_conns() < conn->get_conns())
			conn = *it;
	}

	return conn;
}

void ServerManager::set(ServerConnection* conn)
{
	std::vector<ServerConnection*>::iterator it = conns_.begin();
	for (; it != conns_.end(); ++it)
	{
		if ((*it) == conn)
			return;
	}
	conns_.push_back(conn);
}

void ServerManager::del(ServerConnection* conn)
{
	std::vector<ServerConnection*>::iterator it = conns_.begin();
	for (; it != conns_.end(); ++it)
	{
		if ((*it) == conn)
		{
			conns_.erase(it);
			break;
		}
	}
}

void ServerManager::buildJson()
{
	lock_.lock();

	json_.reset();

	acl::json_node& servers = json_.create_array();
	json_.get_root().add_child("servers", servers);

	std::vector<ServerConnection*>::const_iterator cit = conns_.begin();
	for (; cit != conns_.end(); ++cit)
	{
		acl::json_node& server = json_.create_node();
		server.add_number("conns", (*cit)->get_conns())
			.add_number("used", (*cit)->get_used())
			.add_number("pid", (*cit)->get_pid())
			.add_number("max_threads", (*cit)->get_max_threads())
			.add_number("curr_threads", (*cit)->get_curr_threads())
			.add_number("busy_threads", (*cit)->get_busy_threads())
			.add_number("qlen", (*cit)->get_qlen())
			.add_text("type", (*cit)->get_type());

		servers.add_child(server);
	}

	lock_.unlock();
}

void ServerManager::toString(acl::string& buf)
{
	lock_.lock();
	buf = json_.to_string();
	lock_.unlock();
}
