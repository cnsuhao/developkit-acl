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
	delete (result_, result_->get_pool());
	result_ = NULL;
}

bool redis_hash::hmset(const char* key, const std::map<string, string>& attrs)
{
	reset();

	const string& req = conn_.build_set("HMSET", key, attrs);
	result_ = conn_.run(req);
	if (result_ == NULL)
		return false;
	const char* res = result_->get(0);
	if (res == NULL || strcasecmp(res, "ok") != 0)
		return false;
	return true;
}

bool redis_hash::hmset(const char* key, const std::map<string, char*>& attrs)
{
	reset();

	const string& req = conn_.build_set("HMSET", key, attrs);
	result_ = conn_.run(req);
	if (result_ == NULL)
		return false;
	const char* res = result_->get(0);
	if (res == NULL || strcasecmp(res, "ok") != 0)
		return false;
	return true;
}

bool redis_hash::hmset(const char* key, const std::map<string, const char*>& attrs)
{
	reset();

	const string& req = conn_.build_set("HMSET", key, attrs);
	result_ = conn_.run(req);
	if (result_ == NULL)
		return false;
	const char* res = result_->get(0);
	if (res == NULL || strcasecmp(res, "ok") != 0)
		return false;
	return true;
}

bool redis_hash::hmset(const char* key, const std::map<int, string>& attrs)
{
	reset();

	const string& req = conn_.build_set("HMSET", key, attrs);
	result_ = conn_.run(req);
	if (result_ == NULL)
		return false;
	const char* res = result_->get(0);
	if (res == NULL || strcasecmp(res, "ok") != 0)
		return false;
	return true;
}

bool redis_hash::hmset(const char* key, const std::map<int, char*>& attrs)
{
	reset();

	const string& req = conn_.build_set("HMSET", key, attrs);
	result_ = conn_.run(req);
	if (result_ == NULL)
		return false;
	const char* res = result_->get(0);
	if (res == NULL || strcasecmp(res, "ok") != 0)
		return false;
	return true;
}

bool redis_hash::hmset(const char* key, const std::map<int, const char*>& attrs)
{
	reset();

	const string& req = conn_.build_set("HMSET", key, attrs);
	result_ = conn_.run(req);
	if (result_ == NULL)
		return false;
	const char* res = result_->get(0);
	if (res == NULL || strcasecmp(res, "ok") != 0)
		return false;
	return true;
}

bool redis_hash::hmget(const char* key, const std::vector<string>& names)
{
	reset();

	const string& req = conn_.build_get("HMGET", key, names);
	result_ = conn_.run(req);
	if (result_ == NULL)
		return false;
	return true;
}

bool redis_hash::hmget(const char* key, const std::vector<char*>& names)
{
	reset();

	const string& req = conn_.build_get("HMGET", key, names);
	result_ = conn_.run(req);
	if (result_ == NULL)
		return false;
	return true;
}

bool redis_hash::hmget(const char* key, const std::vector<const char*>& names)
{
	reset();

	const string& req = conn_.build_get("HMGET", key, names);
	result_ = conn_.run(req);
	if (result_ == NULL)
		return false;
	return true;
}

bool redis_hash::hmget(const char* key, const std::vector<int>& names)
{
	reset();

	const string& req = conn_.build_get("HMGET", key, names);
	result_ = conn_.run(req);
	if (result_ == NULL)
		return false;
	return true;
}

bool redis_hash::hmget(const char* key, const char* names[], size_t argc)
{
	reset();

	const string& req = conn_.build_get("HMGET", key, names, argc);
	result_ = conn_.run(req);
	if (result_ == NULL)
		return false;
	return true;
}

bool redis_hash::hmget(const char* key, const char* names[],
	const size_t lens[], size_t argc)
{
	reset();

	const string& req = conn_.build_get("HMGET", key,
		names, lens, argc);
	result_ = conn_.run(req);
	if (result_ == NULL)
		return false;
	return true;
}

const char* redis_hash::hmget_result(size_t i, size_t* len /* = NULL */) const
{
	if (result_ == NULL)
		return NULL;
	const redis_result* rr = result_->get_child(i);
	if (rr == NULL)
		return NULL;
	return rr->get(0, len);
}

} // namespace acl
