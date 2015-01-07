#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <vector>
#include "acl_cpp/stream/socket_stream.hpp"
#include "acl_cpp/redis/redis_response.hpp"
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/connpool/connect_client.hpp"

namespace acl
{

class redis_result;

class ACL_CPP_API redis_client : public connect_client
{
public:
	redis_client(const char* addr, int conn_timeout = 60,
		int rw_timeout = 30, bool retry = true);
	~redis_client();

	void close();
	const std::vector<redis_response*>& request(const char* cmd,
		const void* data, size_t len);
	void clear();

	bool hmset(const char* key, const std::map<string, string>& value,
		int ttl = 0);
	bool hmget(const char* key, redis_result& result);
	bool delete_keys(const std::list<string>& keys);
	bool delete_keys(const char* key1, ...);

protected:
	// »ùÀàÐéº¯Êý
	virtual bool open();

private:
	socket_stream conn_;
	char* addr_;
	int   conn_timeout_;
	int   rw_timeout_;
	bool  retry_;
	std::vector<redis_response*> res_;
};

} // end namespace acl
