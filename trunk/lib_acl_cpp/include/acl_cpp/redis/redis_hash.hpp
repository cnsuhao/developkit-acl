#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <map>
#include "acl_cpp/stdlib/string.hpp"

namespace acl
{

class redis_client;
class redis_result;

class ACL_CPP_API redis_hash
{
public:
	redis_hash(redis_client& conn);
	~redis_hash();

	const redis_result* get_result() const
	{
		return result_;
	}

	/////////////////////////////////////////////////////////////////////

	bool hmset(const char* key, const std::map<string, string>& attrs);
	bool hmset(const char* key, const std::map<string, char*>& attrs);
	bool hmset(const char* key, const std::map<string, const char*>& attrs);

	bool hmset(const char* key, const std::map<int, string>& attrs);
	bool hmset(const char* key, const std::map<int, char*>& attrs);
	bool hmset(const char* key, const std::map<int, const char*>& attrs);

	/////////////////////////////////////////////////////////////////////

	bool hmget(const char* key, const std::vector<string>& names);
	bool hmget(const char* key, const std::vector<char*>& names);
	bool hmget(const char* key, const std::vector<const char*>& names);
	bool hmget(const char* key, const std::vector<int>& names);

	bool hmget(const char* key, const char* names[], size_t argc);
	bool hmget(const char* key, const int names[], size_t argc);
	bool hmget(const char* key, const char* names[],
		const size_t lens[], size_t argc);

	const char* hmget_result(size_t i, size_t* len = NULL) const;

	/////////////////////////////////////////////////////////////////////

private:
	redis_client& conn_;
	const redis_result* result_;
};

} // namespace acl
