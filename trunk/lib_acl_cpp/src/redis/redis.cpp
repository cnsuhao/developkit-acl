#include "acl_stdafx.hpp"
#include "acl_cpp/stdlib/util.hpp"
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/stream/socket_stream.hpp"
#include "acl_cpp/redis/redis.hpp"

redis::redis(const char* addr, int conn_timeout /* = 60 */,
	int rw_timeout /* = 30 */, bool retry /* = true */)
: conn_timeout_(conn_timeout)
, rw_timeout_(rw_timeout)
, retry_(retry)
{
	addr_ = acl_mystrdup(addr);
}

redis::~redis()
{
	acl_myfree(addr_);
}

bool redis::open()
{
	if (conn_.opened())
		return true;
	if (conn_.open(addr_, conn_timeout, rw_timeout_) == false)
	{
		logger_error("connect redis %s error: %s",
			addr_, last_serror());
		return false;
	}
	return true;
}

void redis::close()
{
	conn_.close();
}

void redis::clear()
{
	std::vector<redis_response*>::iterator it = res_.begin();
	for (; it != res_.end(); ++res_)
		delete *it;
	res_.clear();
}

const std::vector<redis_response*>& redis::request(const char* cmd)
{
	string line(128);
	bool retried = false;

	clear();

	while (true)
	{
		if (!conn_.opened() && conn_.open(addr_, conn_timeout_,
			rw_timeout_) == false)
		{
			logger_error("connect server: %s error: %s",
				addr_, last_serror());
			return res_;
		}

		// 先写入数据头
		if (conn_.write(cmd) == -1)
		{
			conn_.close();
			if (retry_ && !retried)
			{
				retried = true;
				continue;
			}
			logger_error("write to redis(%s) error: %s",
				addr_, last_serror());
			return res_;
		}

		// 如果有数据体，则写入数据体
		if (data && len > 0 && (conn.write(data, len) == -1
			|| conn.write("\r\n", 2) == -1))
		{
			conn.close();
			if (retry && !retried)
			{
				retried = true;
				continue;
			}
			logger_error("write to redis(%s) error: %s",
				addr, last_serror());
			return res_;
		}

		line.clear();
		if (conn_.gets(line) == false || line.empty())
		{
			conn_.close();
			if (retry_ && !retried)
			{
				retried = true;
				continue;
			}
			logger_error("gets from redis(%s) error: %s",
				addr_, last_serror());
			return res_;
		}
		break;
	}

	ACL_ARGV* tokens = acl_argv_split(line.c_str(), "\t ");
	return tokens;
}
