#pragma once
#include "acl_cpp/stream/socket_stream.hpp"
#include "acl_cpp/redis/redis_response.hpp"

namespace acl
{

class socket_stream;

class redis
{
public:
	redis(const char* addr, int conn_timeout = 60, int rw_timeout = 30,
		bool retry = true);
	virtual ~redis();

	bool open();
	void close();
	const std::vector<redis_response*>& request(const char* cmd);
	void clear();
private:
	socket_stream conn_;
	char* addr_;
	int   conn_timeout_;
	int   rw_timeout_;
	bool  retry_;
	std::vector<redis_response*> res_;
};

} // end namespace acl
