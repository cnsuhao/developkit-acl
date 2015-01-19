#include "acl_stdafx.hpp"
#include "acl_cpp/redis/redis_client.hpp"
#include "acl_cpp/redis/redis_result.hpp"
#include "acl_cpp/redis/redis_zset.hpp"

namespace acl
{

redis_zset::redis_zset(redis_client* conn /* = NULL */)
: conn_(conn)
, result_(NULL)
{

}

redis_zset::~redis_zset()
{

}

void redis_zset::reset()
{
	if (conn_)
		conn_->reset();
}

void redis_zset::set_client(redis_client* conn)
{
	conn_ = conn;
}

} // namespace acl
