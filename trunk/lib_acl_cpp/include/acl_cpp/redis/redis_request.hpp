#pragma once
#include "acl_cpp/acl_cpp_define.hpp"

namespace acl
{

class dbuf_pool;

class ACL_CPP_API redis_request
{
public:
	redis_request(dbuf_pool* pool);
	~redis_request();

	const string& build(int argc, const char* argv[], const size_t lens[]);
	void reset();

private:
	dbuf_pool* pool_;
	string buf_;
};

} // namespace acl
