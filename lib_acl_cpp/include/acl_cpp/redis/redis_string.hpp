#pragma once
#include "acl_cpp/acl_cpp_define.hpp"

namespace acl
{

class string;
class redis_client;

class ACL_CPP_API redis_string
{
public:
	redis_string(redis_client& conn);
	~redis_string();

	bool set(const char* key, const char* value);
	bool set(const char* key, const char* value, size_t size);

	bool get(const char* key, string& buf);

private:
	redis_client& conn_;
};

} // namespace acl
