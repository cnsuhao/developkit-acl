#include "acl_stdafx.hpp"
#include "acl_cpp/stdlib/locker.hpp"
#include "acl_cpp/stream/connect_client.hpp"
#include "acl_cpp/stream/connect_pool.hpp"

namespace acl
{

connect_pool::connect_pool(const char* addr, int max, int retry_inter /* = 1 */)
: alive_(true)
, last_dead_(0)
, max_(max)
, count_(0)
, total_used_(0)
, current_used_(0)
, last_(0)
{
	if (retry_inter < 1 || retry_inter >= 60)
		retry_inter_ = 1;
	else
		retry_inter_ = retry_inter;

	if (max_ < 1)
		max_ = 10;

	ACL_SAFE_STRNCPY(addr_, addr, sizeof(addr_));
}

connect_pool::~connect_pool()
{
	std::list<connect_client*>::iterator it = pool_.begin();
	for (; it != pool_.end(); ++it)
		delete *it;
}

connect_client* connect_pool::peek()
{
	lock_.lock();
	if (alive_ == false)
	{
		time_t now = time(NULL);
		if (now - last_dead_ < retry_inter_)
		{
			lock_.unlock();
			return NULL;
		}
		alive_ = true;
	}

	connect_client* conn;

	std::list<connect_client*>::iterator it = pool_.begin();
	if (it != pool_.end())
	{
		conn = *it;
		pool_.erase(it);
		total_used_++;
		current_used_++;
		lock_.unlock();
		return conn;
	}
	else if (count_ >= max_)
	{
		logger_error("too many connect(%d > %d) to redis: %s",
			count_, max_, addr_);
		lock_.unlock();
		return NULL;
	}

	conn = create_connect();
	if (conn->open(addr_) == false)
	{
		delete conn;
		alive_ = false;
		time(&last_dead_);

		lock_.unlock();
		return NULL;
	}

	count_++;

	total_used_++;
	current_used_++;

	lock_.unlock();
	return conn;
}

void connect_pool::put(connect_client* conn, bool keep /* = true */)
{
	lock_.lock();
	if (keep && alive_)
		pool_.push_back(conn);
	else
	{
		delete conn;
		count_--;
		acl_assert(count_ >= 0);
	}
	lock_.unlock();
}

void connect_pool::set_alive(bool ok /* true | false */)
{
	lock_.lock();
	alive_ = ok;
	if (ok == false)
		time(&last_dead_);
	lock_.unlock();
}

} // namespace acl
