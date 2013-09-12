#include "acl_stdafx.hpp"
#include "acl_cpp/memcache/memcache_pool.hpp"
#include "acl_cpp/memcache/memcache_manager.hpp"

namespace acl
{

memcache_manager::memcache_manager()
{
}

memcache_manager::~memcache_manager()
{
}

connect_pool* memcache_manager::create_pool(size_t idx,
	const char* addr, int count)
{
	memcache_pool* conns = NEW memcache_pool(idx, addr, count);

	return conns;
}

} // namespace acl
