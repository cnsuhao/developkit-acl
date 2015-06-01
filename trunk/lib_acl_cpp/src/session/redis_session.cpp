#include "acl_stdafx.hpp"
#include "acl_cpp/redis/redis_cluster.hpp"
#include "acl_cpp/redis/redis.hpp"
#include "acl_cpp/session/redis_session.hpp"

namespace acl
{

redis_session::redis_session(const char* addrs, size_t max_conns /* = 128 */,
	int conn_timeout /* = 180 */, int rw_timeout /* = 300 */,
	time_t ttl /* = 0 */, const char* sid /* = NULL */)
{
	cluster_ = NEW redis_cluster(conn_timeout, rw_timeout);
	cluster_->init(NULL, addrs.c_str(), max_conns);

	client_ = NEW redis;
	client_->set_cluster(&cluster_, max_conns);
}

redis_session::~redis_session()
{
	delete client_;
	delete cluster_;
}

bool redis_session::set_attrs(const char* sid, std::map<string, VBUF*>& attrs,
	time_t ttl)
{
	return true;
}

bool redis_session::get_attrs(const char* sid, std::map<string, VBUF*>& attrs)
{
	return true;
}

bool redis_session::del_key(const char* sid)
{
	return client_->del(sid, NULL) >= 0 ? true : false;
}

bool redis_session::set_timeout(const char* sid, time_t ttl)
{
	return client_->expire(sid, (int) ttl) > 0 ? true : false;
}

} // namespace acl
