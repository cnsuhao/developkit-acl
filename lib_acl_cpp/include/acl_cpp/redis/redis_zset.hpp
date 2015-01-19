#pragma once
#include "acl_cpp/acl_cpp_define.hpp"

namespace acl
{

class redis_client;
class redis_result;

class ACL_CPP_API redis_zset
{
public:
	redis_zset(redis_client* conn = NULL);
	~redis_zset();

	void reset();

	void set_client(redis_client* conn);
	redis_client* get_client() const
	{
		return conn_;
	}

	const redis_result* get_result() const
	{
		return result_;
	}

private:
	redis_client* conn_;
	const redis_result* result_;
};

} // namespace acl
