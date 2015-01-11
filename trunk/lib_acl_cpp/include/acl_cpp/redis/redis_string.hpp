#pragma once
#include "acl_cpp/acl_cpp_define.hpp"

namespace acl
{

class string;
class redis_client;
class redis_result;

class ACL_CPP_API redis_string
{
public:
	redis_string(redis_client& conn);
	~redis_string();

	/////////////////////////////////////////////////////////////////////

	bool set(const char* key, const char* value);
	bool set(const char* key, const char* value, size_t size);
	int append(const char* key, const char* value);
	int append(const char* key, const char* value, size_t size);

	bool get(const char* key, string& buf);
	const redis_result* get(const char* key);

	bool getset(const char* key, const char* value, string& buf);

	/////////////////////////////////////////////////////////////////////

	bool mset(const std::map<string, string>& objs);
	bool mset(const std::map<int, string>& objs);

	bool mset(const std::vector<string>& keys,
		const std::vector<string>& values);
	bool mset(const std::vector<int>& keys,
		const std::vector<string>& values);

	bool mset(const char* keys[], const char* values[], size_t argc);
	bool mset(const char* keys[], size_t keys_len[],
		const char* values[], size_t values_len[], size_t argc);
	bool mset(const string& req);

	/////////////////////////////////////////////////////////////////////

	bool mget(const std::vector<string>& keys);
	bool mget(const std::vector<const char*>& keys);
	bool mget(const std::vector<char*>& keys);
	bool mget(const std::vector<int>& keys);

	bool mget(const char* first_key, ...);
	bool mget(const char* keys[], size_t argc);
	bool mget(const int keys[], size_t argc);
	bool mget(const char* keys[], const size_t keys_len[], size_t argc);

	bool mget(const string& req);
	const char* mget_result(size_t i, size_t* len = NULL) const;

	/////////////////////////////////////////////////////////////////////

	bool incr(const char* key, long long int* result = NULL);
	bool incrby(const char* key, long long int inc,
		long long int* result = NULL);
	bool incrbyfloat(const char* key, double inc, double* result = NULL);

	/////////////////////////////////////////////////////////////////////

private:
	redis_client& conn_;
	const redis_result* result_;
};

} // namespace acl
