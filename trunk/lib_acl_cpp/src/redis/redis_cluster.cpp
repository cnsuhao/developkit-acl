#include "acl_stdafx.hpp"
#include "acl_cpp/redis/redis_pool.hpp"
#include "acl_cpp/redis/redis_cluster.hpp"

namespace acl
{

redis_cluster::redis_cluster(int conn_timeout, int rw_timeout,
	int max_slot /* = 16384 */)
: conn_timeout_(conn_timeout)
, rw_timeout_(rw_timeout)
, max_slot_(max_slot)
{
	slots_ = (int*) acl_mycalloc(max_slot_, sizeof(int));
}

redis_cluster::~redis_cluster()
{
	acl_myfree(slots_);
}

connect_pool* redis_cluster::create_pool(const char* addr,
	int count, size_t idx)
{
	redis_pool* pool = NEW redis_pool(addr, count, idx);
	pool->set_timeout(conn_timeout_, rw_timeout_);

	return pool;
}

} // namespace acl
