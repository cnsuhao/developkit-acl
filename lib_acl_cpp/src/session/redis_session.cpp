#include "acl_stdafx.hpp"
#include "acl_cpp/redis/redis_client_cluster.hpp"
#include "acl_cpp/redis/redis.hpp"
#include "acl_cpp/session/redis_session.hpp"

namespace acl
{

redis_session::redis_session(const char* addrs, size_t max_conns,
	int conn_timeout /* = 180 */, int rw_timeout /* = 300 */,
	time_t ttl /* = 0 */, const char* sid /* = NULL */)
	: session(ttl, sid)
{
	cluster_ = NEW redis_client_cluster(conn_timeout, rw_timeout);
	cluster_->init(NULL, addrs, max_conns);

	command_ = NEW redis;
	command_->set_cluster(cluster_, max_conns == 0 ? 128 : max_conns);
}

redis_session::~redis_session()
{
	delete command_;
	delete cluster_;
}

bool redis_session::set(const char* name, const void* value, size_t len)
{
	const char* sid = get_sid();
	if (*sid == 0)
		return false;
	if (command_->hset(sid, name, (const char*) value, len) < 0)
		return false;
	time_t ttl = get_ttl();
	if (ttl > 0)
		return set_ttl(sid, ttl);
	return true;
}

const session_string* redis_session::get_buf(const char* name)
{
	const char* sid = get_sid();
	if (sid == NULL || *sid == 0)
		return NULL;

	ss_.clear();
	if (command_->hget(sid, name, ss_) == false)
		return NULL;
	return &ss_;
}

bool redis_session::del(const char* name)
{
	const char* sid = get_sid();
	if (sid == NULL || *sid == 0)
		return false;
	return command_->hdel(sid, name, NULL) >= 0 ? true : false;
}

bool redis_session::set_attrs(const char* sid,
	const std::map<string, session_string>& attrs, time_t ttl)
{
	if (command_->hmset(sid, (const std::map<string, string>&) attrs) == false)
		return false;
	if (ttl > 0)
		return set_ttl(sid, ttl);
	else
		return true;
}

bool redis_session::get_attrs(const char* sid,
	std::map<string, session_string>& attrs)
{
	attrs_clear(attrs);

	if (command_->hgetall(sid, (std::map<string, string>&) attrs) == false)
		return false;
	return true;
}

bool redis_session::del_key(const char* sid)
{
	return command_->del(sid, NULL) >= 0 ? true : false;
}

bool redis_session::set_ttl(const char* sid, time_t ttl)
{
	return command_->expire(sid, (int) ttl) > 0 ? true : false;
}

} // namespace acl
