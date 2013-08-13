#pragma once

namespace acl
{

class connect_client
{
public:
	connect_client() {}
	virtual ~connect_client() {}

	virtual bool open(const char* addr) = 0;
private:

};

} // namespace acl
