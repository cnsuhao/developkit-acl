#pragma once
#include "acl_cpp/acl_cpp_define.hpp"

namespace acl
{

class redis_client;

class ACL_CPP_API redis_list
{
public:
	redis_list(redis_client& conn);
	~redis_list();

private:
	redis_client& conn_;
};

} // namespace acl
