#include "stdafx.h"
#include "status_manager.h"

status_manager::status_manager()
{
}

status_manager::~status_manager()
{
	std::map<acl::string, server_status*>::iterator it =
		servers_status_.begin();
	for (; it != servers_status_.end(); ++it)
		delete it->second;
}

void status_manager::set_status(const char* key, const char* data)
{
	lock_.lock();
	std::map<acl::string, server_status*>::iterator it =
		servers_status_.find(key);
	if (it != servers_status_.end())
	{
		delete it->second;
		servers_status_.erase(it);
	}
	servers_status_[key] = new server_status(key, data);
	lock_.unlock();
}

acl::string& status_manager::get_status(acl::string& out)
{
	lock_.lock();
	std::map<acl::string, server_status*>::const_iterator cit =
		servers_status_.begin();
	for (; cit != servers_status_.end(); ++cit)
		out << cit->second->get_data();
	lock_.unlock();

	return out;
}

int status_manager::check_timeout()
{
	return 0;
}
