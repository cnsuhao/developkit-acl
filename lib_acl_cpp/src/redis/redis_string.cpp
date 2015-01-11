#include "acl_stdafx.hpp"
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/redis/redis_client.hpp"
#include "acl_cpp/redis/redis_result.hpp"
#include "acl_cpp/redis/redis_string.hpp"

namespace acl
{

#define INT64_LEN	21
#define FLOAT_LEN	32

redis_string::redis_string(redis_client& conn)
: conn_(conn)
, result_(NULL)
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
	const char* names[1];
	names[0] = key;
	size_t names_len[1];
	names_len[0] = strlen(key);
	const char* values[1];
	values[0] = value;
	size_t lens[1];
	lens[0] = size;
	const string& req = conn_.build("SET", NULL, names, names_len,
		values, lens, 1);
	result_ = conn_.run(req);
	if (result_ == NULL)
		return false;
	if (result_->get_type() != REDIS_RESULT_STATUS)
		return false;
	const char* status = result_->get_status();
	if (status == NULL || strcasecmp(status, "OK") != 0)
		return false;
	else
		return true;
}

int redis_string::append(const char* key, const char* value)
{
	return append(key, value, strlen(value));
}

int redis_string::append(const char* key, const char* value, size_t size)
{
	const char* names[1];
	names[0] = key;
	size_t names_len[1];
	names_len[0] = strlen(key);
	const char* values[1];
	values[0] = value;
	size_t lens[1];
	lens[0] = size;
	const string& req = conn_.build("APPEND", NULL, names, names_len,
		values, lens, 1);
	result_ = conn_.run(req);
	if (result_ == NULL)
		return -1;
	if (result_->get_type() != REDIS_RESULT_INTEGER)
		return -1;
	return result_->get_integer();
}

bool redis_string::get(const char* key, string& buf)
{
	result_ = get(key);
	if (result_ == NULL)
		return false;
	(void) result_->argv_to_string(buf);
	return true;
}

const redis_result* redis_string::get(const char* key)
{
	const char* keys[1];
	keys[0] = key;
	const string& req = conn_.build("GET", NULL, keys, 1);
	result_ = conn_.run(req);
	if (result_ == NULL)
		return NULL;
	if (result_->get_type() != REDIS_RESULT_STRING)
		return NULL;
	return result_;
}

bool redis_string::getset(const char* key, const char* value, string& buf)
{
	const char* keys[1];
	keys[0] = key;
	const char* values[1];
	values[0] = value;
	const string& req = conn_.build("GETSET", NULL, keys, values, 1);
	result_ = conn_.run(req);
	if (result_ == NULL)
		return false;
	if (result_->get_type() != REDIS_RESULT_STRING)
		return NULL;
	(void) result_->argv_to_string(buf);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

bool redis_string::mset(const std::map<string, string>& objs)
{
	const string& req = conn_.build("MSET", NULL, objs);
	return mset(req);
}

bool redis_string::mset(const std::map<int, string>& objs)
{
	const string& req = conn_.build("MSET", NULL, objs);
	return mset(req);
}

bool redis_string::mset(const std::vector<string>& keys,
	const std::vector<string>& values)
{
	const string& req = conn_.build("MSET", NULL, keys, values);
	return mset(req);
}

bool redis_string::mset(const std::vector<int>& keys,
	const std::vector<string>& values)
{
	const string& req = conn_.build("MSET", NULL, keys, values);
	return mset(req);
}

bool redis_string::mset(const char* keys[], const char* values[], size_t argc)
{
	const string& req = conn_.build("MSET", NULL, keys, values, argc);
	return mset(req);
}

bool redis_string::mset(const char* keys[], size_t keys_len[],
	const char* values[], size_t values_len[], size_t argc)
{
	const string& req = conn_.build("MSET", NULL, keys, keys_len,
		values, values_len, argc);
	return mset(req);
}

bool redis_string::mset(const string& req)
{
	result_ = conn_.run(req);
	if (result_ == NULL)
		return false;
	if (result_->get_type() != REDIS_RESULT_STATUS)
		return false;
	const char* ptr = result_->get(0);
	if (ptr == NULL || strcasecmp(ptr, "OK") != 0)
		return false;
	return true;
}

/////////////////////////////////////////////////////////////////////////////

bool redis_string::mget(const std::vector<string>& keys)
{
	const string& req = conn_.build("MGET", NULL, keys);
	return mget(req);
}

bool redis_string::mget(const std::vector<const char*>& keys)
{
	const string& req = conn_.build("MGET", NULL, keys);
	return mget(req);
}

bool redis_string::mget(const std::vector<char*>& keys)
{
	const string& req = conn_.build("MGET", NULL, keys);
	return mget(req);
}

bool redis_string::mget(const std::vector<int>& keys)
{
	const string& req = conn_.build("MGET", NULL, keys);
	return mget(req);
}

bool redis_string::mget(const char* first_key, ...)
{
	std::vector<const char*> keys;
	keys.push_back(first_key);
	va_list ap;
	va_start(ap, first_key);
	const char* key;
	while ((key = va_arg(ap, const char*)) != NULL)
		keys.push_back(key);
	va_end(ap);

	return mget(keys);
}

bool redis_string::mget(const char* keys[], size_t argc)
{
	const string& req = conn_.build("MGET", NULL, keys, argc);
	return mget(req);
}

bool redis_string::mget(const int keys[], size_t argc)
{
	const string& req = conn_.build("MGET", NULL, keys, argc);
	return mget(req);
}

bool redis_string::mget(const char* keys[], const size_t keys_len[],
	size_t argc)
{
	const string& req = conn_.build("MGET", NULL, keys, keys_len, argc);
	return mget(req);
}

bool redis_string::mget(const string& req)
{
	result_ = conn_.run(req);
	if (result_ == NULL)
		return false;
	if (result_->get_type() != REDIS_RESULT_ARRAY)
		return false;
	return true;
}

const char* redis_string::mget_result(size_t i, size_t* len /* = NULL */) const
{
	if (result_ == NULL)
		return NULL;
	if (result_->get_type() != REDIS_RESULT_ARRAY)
		return NULL;
	const redis_result* rr = result_->get_child(i);
	if (rr == NULL)
		return NULL;
	size_t size = rr->get_size();
	if (size == 0)
		return NULL;
	if (size == 1)
		return rr->get(0, len);

	// 大内存有可能被切片成多个不连续的小内存
	size = rr->get_length();
	size++;
	char* buf = (char*) conn_.get_pool()->dbuf_alloc(size);
	size = rr->argv_to_string(buf, size);
	if (len)
		*len = size;
	return buf;
}

/////////////////////////////////////////////////////////////////////////////

bool redis_string::incr(const char* key, long long int* result /* = NULL */)
{
	const char* keys[1];
	keys[0] = key;
	const char* values[1];
	values[0] = "1";
	const string& req = conn_.build("INCR", NULL, keys, values, 1);
	result_ = conn_.run(req);
	if (result_ == NULL)
		return false;
	if (result_->get_type() != REDIS_RESULT_INTEGER)
		return false;
	if (result != NULL)
		*result = result_->get_integer64();
	return true;
}

bool redis_string::incrby(const char* key, long long int inc,
	long long int* result /* = NULL */)
{
	const char* keys[1];
	keys[0] = key;
	char buf[INT64_LEN];
	(void) acl_i64toa(inc, buf, sizeof(buf));
	const char* values[1];
	values[0] = buf;
	const string& req = conn_.build("INCRBY", NULL, keys, values, 1);
	result_ = conn_.run(req);
	if (result_ == NULL)
		return false;
	if (result_->get_type() != REDIS_RESULT_INTEGER)
		return false;
	if (result != NULL)
		*result = result_->get_integer64();
	return true;
}

bool redis_string::incrbyfloat(const char* key, double inc,
	double* result /* = NULL */)
{
	const char* keys[1];
	keys[0] = key;
	char buf[FLOAT_LEN];
	(void) safe_snprintf(buf, sizeof(buf), "%f", inc);
	const char* values[1];
	values[0] = buf;
	const string& req = conn_.build("INCRBYFLOAT", NULL, keys, values, 1);
	result_ = conn_.run(req);
	if (result_ == NULL)
		return false;
	if (result_->get_type() != REDIS_RESULT_STRING)
		return false;
	if (result != NULL)
	{
		(void) result_->argv_to_string(buf, sizeof(buf));
		*result = atof(buf);
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////

} // namespace acl
