#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <vector>
#include "acl_cpp/stream/socket_stream.hpp"
#include "acl_cpp/redis/redis_response.hpp"
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/connpool/connect_client.hpp"

namespace acl
{

class dbuf_pool;
class redis_result;

class ACL_CPP_API redis_client : public connect_client
{
public:
	redis_client(const char* addr, int conn_timeout = 60,
		int rw_timeout = 30, bool retry = true);
	~redis_client();

	void close();
	
	void clear();

	bool hmset(const char* key, const std::map<string, string>& attrs,
		int ttl = 0);
	bool hmget(const char* key, const std::map<string, string>& attrs,
		redis_result& result);
	bool delete_keys(const std::list<string>& keys);
	bool delete_keys(const char* key1, ...);

protected:
	// »ùÀàÐéº¯Êý
	virtual bool open();

private:
	dbuf_pool* pool_;
	socket_stream conn_;
	char* addr_;
	int   conn_timeout_;
	int   rw_timeout_;
	bool  retry_;
	std::vector<redis_response*> res_;
	size_t  argv_size_;
	const char**  argv_;
	size_t  argc_;
	size_t* argv_lens_;
	string  request_;

	void argv_space(size_t n);
	void build_request();
	void build_attrs(const char* cmd, const char* key,
		const std::map<string, string>& value);
	bool send_request();
};

} // end namespace acl
