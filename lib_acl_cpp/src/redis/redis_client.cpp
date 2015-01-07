#include "acl_stdafx.hpp"
#include "acl_cpp/stdlib/util.hpp"
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/stream/socket_stream.hpp"
#include "acl_cpp/redis/redis_client.hpp"

namespace acl
{

redis_client::redis_client(const char* addr, int conn_timeout /* = 60 */,
	int rw_timeout /* = 30 */, bool retry /* = true */)
: conn_timeout_(conn_timeout)
, rw_timeout_(rw_timeout)
, retry_(retry)
, argv_size_(0)
, argv_(NULL)
, argv_lens_(NULL)
{
	addr_ = acl_mystrdup(addr);
	pool_ = NEW dbuf_pool();
}

redis_client::~redis_client()
{
	acl_myfree(addr_);
	delete pool_;
}

bool redis_client::open()
{
	if (conn_.opened())
		return true;
	if (conn_.open(addr_, conn_timeout_, rw_timeout_) == false)
	{
		logger_error("connect redis %s error: %s",
			addr_, last_serror());
		return false;
	}
	return true;
}

void redis_client::close()
{
	conn_.close();
}

void redis_client::clear()
{
	std::vector<redis_response*>::iterator it = res_.begin();
	for (; it != res_.end(); ++it)
		delete *it;
	res_.clear();
}

bool redis_client::send_request()
{
	bool retried = false;

	clear();

	while (true)
	{
		if (!conn_.opened() && conn_.open(addr_, conn_timeout_,
			rw_timeout_) == false)
		{
			logger_error("connect server: %s error: %s",
				addr_, last_serror());
			return false;
		}

		if (conn_.write(request_) == -1)
		{
			conn_.close();
			if (retry_ && !retried)
			{
				retried = true;
				continue;
			}
			logger_error("write to redis(%s) error: %s",
				addr_, last_serror());
			return false;
		}

		break;
	}

	return true;
}

void redis_client::argv_space(size_t n)
{
	if (argv_size_ >= n)
		return;
	argv_size_ = n;
	argv_ = (const char**) pool_->dbuf_alloc(n * sizeof(char*));
	argv_lens_ = (size_t*) pool_->dbuf_alloc(n * sizeof(size_t));
}

void redis_client::build_request()
{
	request_.clear();
	request_.append("*%d\r\n", argc_);
	for (size_t i = 0; i < argc_; i++)
	{
		request_.append("$%lu\r\n", (unsigned long) argv_lens_[i]);
		request_.append(argv_[i], argv_lens_[i]);
		request_.append("\r\n");
	}
}

void redis_client::build_attrs(const char* cmd, const char* key,
	const std::map<string, string>& value)
{
	argc_ = 2 + value.size() * 2;
	argv_space(argc_);

	size_t i = 0;
	argv_[i] = cmd;
	argv_lens_[i] = strlen(cmd);
	i++;

	argv_[i] = key;
	argv_lens_[i] = strlen(key);
	i++;

	std::map<string, string>::const_iterator cit = value.begin();
	for (; cit != value.end(); ++cit)
	{
		argv_[i] = cit->first.c_str();
		argv_lens_[i] = strlen(argv_[i]);
		i++;

		argv_[i] = cit->second.c_str();
		argv_lens_[i] = strlen(argv_[i]);
		i++;
	}
}

bool redis_client::hmset(const char* key,
	const std::map<string, string>& attrs, int ttl /* = 0 */)
{
	build_attrs("HMSET", key, attrs);
	build_request();
	return true;
}

bool redis_client::hmget(const char* key,
	const std::map<string, string>& attrs, redis_result& result)
{
	build_attrs("HMGET", key, attrs);
	build_request();
	return true;
}

bool redis_client::delete_keys(const std::list<string>& keys)
{
	return true;
}

bool redis_client::delete_keys(const char* key1, ...)
{
	return true;
}

} // end namespace acl
