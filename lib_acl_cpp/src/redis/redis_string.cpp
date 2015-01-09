#include "acl_stdafx.hpp"
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/redis/redis_client.hpp"
#include "acl_cpp/redis/redis_result.hpp"
#include "acl_cpp/redis/redis_string.hpp"

namespace acl
{

redis_string::redis_string(redis_client& conn)
: conn_(conn)
{

}

redis_string::~redis_string()
{

}

bool redis_string::set(const char* key, const char* value)
{
	return set(key, value, strlen(value));
}

bool redis_string::set(const char* key, const char* value, size_t size)
{
	const char* argv[1];
	argv[0] = value;

	size_t lens[1];
	lens[0] = size;
	const string& req = conn_.build("SET", key, argv, lens, 1);
	const redis_result* rr = conn_.run(req);
	if (rr == NULL)
		return false;
	const char* status = rr->get_status();
	if (status == NULL || strcasecmp(status, "OK") != 0)
		return false;
	else
		return true;
}

bool redis_string::get(const char* key, string& buf)
{
	const char* keys[1];
	keys[0] = key;
	const string& req = conn_.build("GET", keys, 1);
	const redis_result* rr = conn_.run(req);
	if (rr == NULL)
		return false;
	redis_result_t type = rr->get_type();
	if (type != REDIS_RESULT_STRING)
		return false;
	(void) rr->argv_to_string(buf);
	return true;
}

} // namespace acl
