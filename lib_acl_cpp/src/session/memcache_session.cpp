#include "acl_stdafx.hpp"
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/memcache/mem_cache.hpp"
#include "acl_cpp/session/memcache_session.hpp"

namespace acl
{

memcache_session::memcache_session(const char* cache_addr,
	int conn_timeout /* = 180 */, int rw_timeout /* = 300 */,
	const char* prefix /* = NULL */, time_t ttl /* = 0 */, 
	const char* sid /* = NULL */, bool encode_key /* = true */)
: session(ttl, sid)
, auth_free_(true)
{
	acl_assert(cache_addr && *cache_addr);
	cache_ = NEW mem_cache(prefix ? prefix : "_", cache_addr, true,
		conn_timeout, rw_timeout, encode_key);
}

memcache_session::memcache_session(mem_cache* cache, bool auto_free /* = false */,
	time_t ttl /* = 0 */, const char* sid /* = NULL */)
: session(ttl, sid)
, cache_(cache)
, auth_free_(auto_free)
{

}

memcache_session::~memcache_session()
{
	if (auth_free_)
		delete cache_;
}

bool memcache_session::get_data(const char* sid, string& buf)
{
	return cache_->get(sid, buf);
}

bool memcache_session::set_data(const char* sid, const char* buf,
	size_t len, time_t ttl)
{
	return cache_->set(sid, buf, len, ttl);
}

bool memcache_session::del_data(const char* sid)
{
	return cache_->del(sid);
}

bool memcache_session::set_timeout(const char* sid, time_t ttl)
{
	return cache_->set(sid, ttl);
}

} // namespace acl
