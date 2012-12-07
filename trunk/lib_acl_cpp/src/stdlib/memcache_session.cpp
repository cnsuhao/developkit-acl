#include "acl_stdafx.hpp"
#include "log.hpp"
#include "mem_cache.hpp"
#include "memcache_session.hpp"

namespace acl
{

memcache_session::memcache_session(const char* cache_addr,
	const char* prefix /* = NULL */, time_t ttl /* = 0 */,
	const char* sid /* = NULL */)
: session(ttl, sid)
{
	acl_assert(cache_addr && *cache_addr);
	cache_ = NEW mem_cache(prefix ? prefix : "_", cache_addr);
}

memcache_session::~memcache_session()
{
	delete cache_;
}

bool memcache_session::get_data(string& buf)
{
	return cache_->get(get_sid(), buf);
}

bool memcache_session::set_data(const char* buf, size_t len, time_t ttl)
{
	return cache_->set(get_sid(), buf, len, ttl);
}

bool memcache_session::del_data()
{
	return cache_->del(get_sid());
}

bool memcache_session::set_timeout(time_t ttl)
{
	return cache_->set(get_sid(), ttl);
}

} // namespace acl
