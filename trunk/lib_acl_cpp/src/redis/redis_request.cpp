#include "acl_stdafx.hpp"
#include "acl_cpp/redis/redis_request.hpp"

namespace acl
{

redis_request::redis_request(dbuf_pool* pool)
	:pool_(pool)
{

}

redis_request::~redis_request()
{

}

void redis_request::reset()
{
	buf_.clear();
}

const string& redis_request::build(size_t argc, const char* argv[],
	const size_t lens[])
{
	buf_.append("*%d\r\n", argc);
	for (size_t i = 0; i < argc; i++)
	{
		buf_.append("$%u\r\n", (unsigned) lens[i]);
		buf_.append(argv[i], lens[i]);
		buf_.append("\r\n");
	}

	return buf_;
}

} // namespace acl
