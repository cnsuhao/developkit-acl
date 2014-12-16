#include "stdafx.h"
#include "access_list.h"

access_list::access_list()
: allow_all_(false)
{
}

void access_list::init(const char* whitelist)
{
	if (strcasecmp(whitelist, "all") == 0)
	{
		allow_all_ = true;
		return;
	}

	acl::string buf(whitelist);
	buf.trim_space();

	const std::list<acl::string>& users = buf.split(";,");
	std::list<acl::string>::const_iterator cit = users.begin();
	for (; cit != users.end(); ++cit)
		add_one((*cit).c_str());
}

void access_list::add_one(const char* user)
{
	acl::string buf(user);
	buf.lower();

	std::vector<acl::string>::const_iterator cit = white_list_.begin();
	for (; cit != white_list_.end(); ++cit)
	{
		if ((*cit) == buf)
			return;
	}

	white_list_.push_back(buf);
}

bool access_list::check(const char* user)
{
	if (allow_all_)
		return true;

	acl::string buf(user);
	buf.lower();

	std::vector<acl::string>::const_iterator cit = white_list_.begin();
	for (; cit != white_list_.end(); ++cit)
	{
		const char* ptr = (*cit).c_str();
		if (buf == ptr)
			return true;
		if (*ptr == '@' && buf.rncompare(ptr, strlen(ptr)) == 0)
			return true;
	}

	return false;
}
