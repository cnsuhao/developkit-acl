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
	slot_addrs_ = (const char**) acl_mycalloc(max_slot_, sizeof(char*));
}

redis_cluster::~redis_cluster()
{
	acl_myfree(slot_addrs_);
}

connect_pool* redis_cluster::create_pool(const char* addr,
	int count, size_t idx)
{
	redis_pool* pool = NEW redis_pool(addr, count, idx);
	pool->set_timeout(conn_timeout_, rw_timeout_);

	return pool;
}

redis_pool* redis_cluster::peek_slot(int slot)
{
	if (slot < 0 || slot >= max_slot_)
		return NULL;

	// 需要加锁保护
	lock();

	if (slot_addrs_[slot] == NULL)
	{
		unlock();
		return NULL;
	}

	// 因为已经进行了加锁保护，所以在调用 get 方法时的第二个锁保护参数设为 false
	redis_pool* conn = (redis_pool*) get(slot_addrs_[slot], false);

	unlock();

	return conn;
}

void redis_cluster::set_slot(int slot, const char* addr)
{
	if (slot < 0 || slot >= max_slot_ || addr == NULL || *addr == 0)
		return;

	// 遍历缓存的所有地址，若该地址不存在则直接添加，然后使之与 slot 进行关联

	// 该段代码需要加锁保护
	lock();

	std::vector<string>::const_iterator cit = addrs_.begin();
	for (; cit != addrs_.end(); ++cit)
	{
		if ((*cit) == addr)
			break;
	}

	if (cit == addrs_.end())
	{
		addrs_.push_back(addr);
		cit = addrs_.end();
		--cit;
	}
	// 将 slot 与地址进行关联映射
	slot_addrs_[slot] = (*cit).c_str();

	unlock();
}

} // namespace acl
