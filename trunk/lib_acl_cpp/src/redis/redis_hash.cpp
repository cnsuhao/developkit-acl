#include "acl_stdafx.hpp"
#include "acl_cpp/redis/redis_client.hpp"
#include "acl_cpp/redis/redis_result.hpp"
#include "acl_cpp/redis/redis_hash.hpp"

namespace acl
{

redis_hash::redis_hash(redis_client& conn)
: conn_(conn)
, result_(NULL)
{

}

redis_hash::~redis_hash()
{
	reset();
}

void redis_hash::reset()
{
	delete result_;
	result_ = NULL;
}

bool redis_hash::hmset(const char* key, const std::map<string, string>& attrs)
{
	reset();

	const string& req = conn_.build_request("HMSET", key, attrs);
	result_ = conn_.run(req);
	if (result_ == NULL)
		return false;
	const char* res = (*result_)[0];
	if (res == NULL || strcasecmp(res, "ok") != 0)
		return false;
	return true;
}

bool redis_hash::hmget(const char* key, const std::vector<string>& names)
{
	reset();

	const string& req = conn_.build_request("HMGET", key, names);
	result_ = conn_.run(req);
	if (result_ == NULL)
		return false;
	return true;
}

bool redis_hash::hmget(const char* key, const std::vector<char*>& names)
{
	reset();

	const string& req = conn_.build_request("HMGET", key, names);
	result_ = conn_.run(req);
	if (result_ == NULL)
		return false;
	return true;
}

bool redis_hash::hmget(const char* key, const std::vector<const char*>& names)
{
	reset();

	const string& req = conn_.build_request("HMGET", key, names);
	result_ = conn_.run(req);
	if (result_ == NULL)
		return false;
	return true;
}
bool redis_hash::hmget(const char* key, const char* names[], size_t argc)
{
	reset();

	const string& req = conn_.build_request("HMGET", key, names, argc);
	result_ = conn_.run(req);
	if (result_ == NULL)
		return false;
	return true;
}

bool redis_hash::hmget(const char* key, const char* names[],
	const size_t lens[], size_t argc)
{
	reset();

	const string& req = conn_.build_request("HMGET", key,
		names, lens, argc);
	result_ = conn_.run(req);
	if (result_ == NULL)
		return false;
	return true;
}

} // namespace acl
