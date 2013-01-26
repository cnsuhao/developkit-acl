#include "acl_stdafx.hpp"
#include <stdarg.h>
#include "acl_cpp/stream/socket_stream.hpp"
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/stdlib/util.hpp"
#include "acl_cpp/beanstalk/beanstalk.hpp"

namespace acl
{

#ifdef WIN32
#define atoll _atoi64
#endif

static ACL_ARGV* request(socket_stream& conn, const char* addr,
	int timeout, bool retry, const string& cmdline,
	const void* data = NULL, size_t len = 0)
{
	string line(128);
	bool retried = false;

	while (true)
	{
		if (!conn.opened() && conn.open(addr, timeout, 0) == false)
		{
			logger_error("connect server: %s error: %s",
				addr, last_serror());
			return NULL;
		}

		// 先写入数据头
		if (conn.write(cmdline) == -1)
		{
			conn.close();
			if (retry && !retried)
			{
				retried = true;
				continue;
			}
			logger_error("write to beanstalkd(%s) error: %s",
				addr, last_serror());
			return NULL;
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
			logger_error("write to beanstalkd(%s) error: %s",
				addr, last_serror());
			return NULL;
		}

		line.clear();
		if (conn.gets(line) == false || line.empty())
		{
			conn.close();
			if (retry && !retried)
			{
				retried = true;
				continue;
			}
			logger_error("gets from beanstalkd(%s) error: %s",
				addr, last_serror());
			return NULL;
		}
		break;
	}

	ACL_ARGV* tokens = acl_argv_split(line.c_str(), "\t ");
	return tokens;
}

//////////////////////////////////////////////////////////////////////////

beanstalk::beanstalk(const char* addr, int conn_timeout,
	bool retry /* = true */)
: timeout_(conn_timeout)
, retry_(retry)
{
	addr_ = acl_mystrdup(addr);
	errbuf_[0] = 0;
}

beanstalk::~beanstalk()
{
	acl_myfree(addr_);
}

bool beanstalk::open()
{
	if (conn_.opened())
		return true;

	if (conn_.open(addr_, timeout_, 0) == false)
	{
		logger_error("connect beanstalkd %s error: %s",
			addr_, last_serror());
		return false;
	}
	return true;
}

void beanstalk::close()
{
	conn_.close();
}

bool beanstalk::use(const char* tube)
{
	string cmdline(128);
	cmdline.format("use %s\r\n", tube);
	ACL_ARGV* tokens = request(conn_, addr_, timeout_, retry_, cmdline);
	if (tokens == NULL)
	{
		logger_error("'%s' error", cmdline.c_str());
		return false;
	}
	if (tokens->argc < 2 || strcasecmp(tokens->argv[0], "USING")
		|| strcasecmp(tokens->argv[1], tube))
	{
		logger_error("'%s' error %s", cmdline.c_str(), tokens->argv[0]);
		ACL_SAFE_STRNCPY(errbuf_, tokens->argv[0], sizeof(errbuf_));
		acl_argv_free(tokens);
		close();
		return false;
	}

	acl_argv_free(tokens);
	return true;
}

unsigned long long beanstalk::put(const void* data, size_t n,
	unsigned pri /* = 1024 */, unsigned delay /* = 0 */,
	unsigned int ttr /* = 60 */)
{
	string cmdline(128);
	cmdline.format("put %u %u %u %u\r\n", pri, delay, ttr, (unsigned int) n);
	ACL_ARGV* tokens = request(conn_, addr_, timeout_, retry_,
		cmdline, data, n);
	if (tokens == NULL)
	{
		logger_error("'%s' error", cmdline.c_str());
		return 0;
	}

	if (tokens->argc < 2 || strcasecmp(tokens->argv[0], "INSERTED"))
	{
		logger_error("'%s' error", cmdline.c_str());
		ACL_SAFE_STRNCPY(errbuf_, tokens->argv[0], sizeof(errbuf_));
		acl_argv_free(tokens);
		close();
		return 0;
	}

	unsigned long long id = (unsigned long long) atoll(tokens->argv[1]);
	acl_argv_free(tokens);
	return id;
}

unsigned beanstalk::watch(const char* tube)
{
	string cmdline(128);
	cmdline.format("watch %s\r\n", tube);
	ACL_ARGV* tokens = request(conn_, addr_, timeout_, retry_, cmdline);
	if (tokens == NULL)
	{
		logger_error("'%s' error", cmdline.c_str());
		return 0;
	}
	if (tokens->argc < 2 || strcasecmp(tokens->argv[0], "WATCHING"))
	{
		logger_error("'%s' error %s", cmdline.c_str(), tokens->argv[0]);
		ACL_SAFE_STRNCPY(errbuf_, tokens->argv[0], sizeof(errbuf_));
		acl_argv_free(tokens);
		close();
		return 0;
	}

	unsigned n = (unsigned) atoi(tokens->argv[1]);
	acl_argv_free(tokens);
	return n;
}

unsigned beanstalk::ignore(const char* tube)
{
	string cmdline(128);
	cmdline.format("ignore %s\r\n", tube);
	ACL_ARGV* tokens = request(conn_, addr_, timeout_, retry_, cmdline);
	if (tokens == NULL)
	{
		logger_error("'%s' error", cmdline.c_str());
		return 0;
	}
	if (tokens->argc < 2 || strcasecmp(tokens->argv[0], "WATCHING"))
	{
		logger_error("'%s' error %s", cmdline.c_str(), tokens->argv[0]);
		ACL_SAFE_STRNCPY(errbuf_, tokens->argv[0], sizeof(errbuf_));
		acl_argv_free(tokens);
		close();
		return 0;
	}

	unsigned n = (unsigned) atoi(tokens->argv[1]);
	acl_argv_free(tokens);
	return n;
}

unsigned long long beanstalk::reserve(string& buf, int timeout /* = -1 */)
{
	string cmdline(128);
	if (timeout >= 0)
		cmdline.format("reserve-with-timeout %d\r\n", timeout);
	else
		cmdline.format("reserve\r\n");
	ACL_ARGV* tokens = request(conn_, addr_, timeout_, retry_, cmdline);
	if (tokens == NULL)
	{
		logger_error("'%s' error", cmdline.c_str());
		return 0;
	}
	if (tokens->argc < 3 || strcasecmp(tokens->argv[0], "RESERVED"))
	{
		logger_error("reserve failed: %s", tokens->argv[0]);
		ACL_SAFE_STRNCPY(errbuf_, tokens->argv[0], sizeof(errbuf_));
		acl_argv_free(tokens);
		close();
		return 0;
	}

	unsigned long long id = (unsigned long long) atoll(tokens->argv[1]);
	unsigned short n = (unsigned short) atoi(tokens->argv[2]);
	acl_argv_free(tokens);

	if (n == 0)
	{
		logger_error("reserve data's length 0");
		close();
		return 0;
	}

	// 读取规定的字节数
	if (conn_.read(buf, n, true) == false)
	{
		logger_error("reserve read body failed");
		close();
		return 0;
	}
	else if (conn_.gets(cmdline) == false)
	{
		logger_error("reserve: gets last line failed");
		close();
		return 0;
	}
	return id;
}

bool beanstalk::delete_id(unsigned long long id)
{
	string cmdline(128);
	cmdline.format("delete %llu\r\n", id);
	ACL_ARGV* tokens = request(conn_, addr_, timeout_, retry_, cmdline);
	if (tokens == NULL)
	{
		logger_error("'%s' error", cmdline.c_str());
		return false;
	}
	if (strcasecmp(tokens->argv[0], "DELETED"))
	{
		logger_error("'%s' error %s", cmdline.c_str(), tokens->argv[0]);
		ACL_SAFE_STRNCPY(errbuf_, tokens->argv[0], sizeof(errbuf_));
		acl_argv_free(tokens);
		return false;
	}

	acl_argv_free(tokens);
	return true;
}

bool beanstalk::release(unsigned long long id, unsigned pri /* = 1024 */,
	unsigned delay /* = 0*/)
{
	string cmdline(128);
	cmdline.format("release %llu %u %u\r\n", id, pri, delay);
	ACL_ARGV* tokens = request(conn_, addr_, timeout_, retry_, cmdline);
	if (tokens == NULL)
	{
		logger_error("'%s' error", cmdline.c_str());
		return false;
	}
	if (strcasecmp(tokens->argv[0], "RELEASED"))
	{
		logger_error("'%s' error %s", cmdline.c_str(), tokens->argv[0]);
		ACL_SAFE_STRNCPY(errbuf_, tokens->argv[0], sizeof(errbuf_));
		acl_argv_free(tokens);
		return false;
	}

	acl_argv_free(tokens);
	return true;
}

bool beanstalk::bury(unsigned long long id, unsigned int pri /* = 1024 */)
{
	string cmdline(128);
	cmdline.format("bury %llu %u %u\r\n", id, pri);
	ACL_ARGV* tokens = request(conn_, addr_, timeout_, retry_, cmdline);
	if (tokens == NULL)
	{
		logger_error("'%s' error", cmdline.c_str());
		return false;
	}
	if (strcasecmp(tokens->argv[0], "BURIED"))
	{
		logger_error("'%s' error %s", cmdline.c_str(), tokens->argv[0]);
		ACL_SAFE_STRNCPY(errbuf_, tokens->argv[0], sizeof(errbuf_));
		acl_argv_free(tokens);
		return false;
	}

	acl_argv_free(tokens);
	return true;
}

bool beanstalk::touch(unsigned long long id)
{
	string cmdline(128);
	cmdline.format("touch %llu\r\n", id);
	ACL_ARGV* tokens = request(conn_, addr_, timeout_, retry_, cmdline);
	if (tokens == NULL)
	{
		logger_error("'%s' error", cmdline.c_str());
		return false;
	}
	if (strcasecmp(tokens->argv[0], "TOUCHED"))
	{
		logger_error("'%s' error %s", cmdline.c_str(), tokens->argv[0]);
		ACL_SAFE_STRNCPY(errbuf_, tokens->argv[0], sizeof(errbuf_));
		acl_argv_free(tokens);
		return false;
	}

	acl_argv_free(tokens);
	return true;
}

unsigned long long beanstalk::peek_fmt(string& buf, const char* fmt, ...)
{
	string cmdline(128);
	va_list ap;
	va_start(ap, fmt);
	cmdline.vformat(fmt, ap);
	va_end(ap);

	ACL_ARGV* tokens = request(conn_, addr_, timeout_, retry_, cmdline);
	if (tokens == NULL)
	{
		logger_error("'%s' error", cmdline.c_str());
		return 0;
	}
	if (tokens->argc < 3 || strcasecmp(tokens->argv[0], "FOUND"))
	{
		ACL_SAFE_STRNCPY(errbuf_, tokens->argv[0], sizeof(errbuf_));
		acl_argv_free(tokens);
		return 0;
	}
	unsigned long long id = (unsigned long long) atoll(tokens->argv[1]);
	unsigned short n = (unsigned short) atoi(tokens->argv[2]);
	acl_argv_free(tokens);

	if (n == 0)
	{
		logger_error("peek data's length 0");
		close();
		return 0;
	}

	// 读取规定的字节数
	if (conn_.read(buf, n, true) == false)
	{
		logger_error("peek read body failed");
		close();
		return 0;
	}
	else if (conn_.gets(cmdline) == false)
	{
		logger_error("peek: gets last line falied");
		close();
		return 0;
	}
	return id;
}

unsigned long long beanstalk::peek(string& buf, unsigned long long id)
{
	return peek_fmt(buf, "peek %llu\r\n", id);
}

unsigned long long beanstalk::peek_ready(string& buf)
{
	return peek_fmt(buf, "peek-ready\r\n");
}

unsigned long long beanstalk::peek_delayed(string& buf)
{
	return peek_fmt(buf, "peek-delayed\r\n");
}

unsigned long long beanstalk::peek_buried(string& buf)
{
	return peek_fmt(buf, "peek-buried\r\n");
}

int beanstalk::kick(unsigned n)
{
	string cmdline(128);
	cmdline.format("kick %u\r\n", n);
	ACL_ARGV* tokens = request(conn_, addr_, timeout_, retry_, cmdline);
	if (tokens == NULL)
	{
		logger_error("'%s' error", cmdline.c_str());
		return -1;
	}
	if (strcasecmp(tokens->argv[0], "KICKED"))
	{
		logger_error("'%s' error %s", cmdline.c_str(), tokens->argv[0]);
		ACL_SAFE_STRNCPY(errbuf_, tokens->argv[0], sizeof(errbuf_));
		acl_argv_free(tokens);
		return -1;
	}

	int  ret;
	if (tokens->argc >= 2)
		ret = atoi(tokens->argv[1]);
	else
		ret = 0;
	acl_argv_free(tokens);
	return ret;
}

bool beanstalk::list_tube_used(string& buf)
{
	string cmdline(128);
	cmdline.format("list-tube-used\r\n");
	ACL_ARGV* tokens = request(conn_, addr_, timeout_, retry_, cmdline);
	if (tokens == NULL)
	{
		logger_error("'%s' error", cmdline.c_str());
		return false;
	}
	if (tokens->argc < 2 || strcasecmp(tokens->argv[0], "USING"))
	{
		logger_error("'%s' error %s", cmdline.c_str(), tokens->argv[0]);
		ACL_SAFE_STRNCPY(errbuf_, tokens->argv[0], sizeof(errbuf_));
		acl_argv_free(tokens);
		close();
		return false;
	}

	buf = tokens->argv[1];
	acl_argv_free(tokens);
	return true;
}

bool beanstalk::list_tubes_fmt(string& buf, const char* fmt, ...)
{
	string cmdline(128);
	va_list ap;
	va_start(ap, fmt);
	cmdline.vformat(fmt, ap);
	va_end(ap);

	ACL_ARGV* tokens = request(conn_, addr_, timeout_, retry_, cmdline);
	if (tokens == NULL)
	{
		logger_error("'%s' error", cmdline.c_str());
		return false;
	}
	if (tokens->argc < 2 || strcasecmp(tokens->argv[0], "OK"))
	{
		ACL_SAFE_STRNCPY(errbuf_, tokens->argv[0], sizeof(errbuf_));
		acl_argv_free(tokens);
		close();
		return false;
	}
	unsigned short n = (unsigned short) atoi(tokens->argv[1]);
	acl_argv_free(tokens);

	if (n == 0)
	{
		logger_error("list data's length 0");
		close();
		return false;
	}

	// 读取规定的字节数
	if (conn_.read(buf, n, true) == false)
	{
		logger_error("peek read body failed");
		close();
		return false;
	}
	return true;
}

bool beanstalk::list_tubes(string& buf)
{
	return list_tubes_fmt(buf, "list-tubes\r\n");
}

bool beanstalk::list_tubes_watched(string& buf)
{
	return list_tubes_fmt(buf, "list-tubes-watched\r\n");
}

bool beanstalk::pause_tube(const char* tube, unsigned delay)
{
	string cmdline(128);
	cmdline.format("pause-tube %s %u\r\n", tube, delay);
	ACL_ARGV* tokens = request(conn_, addr_, timeout_, retry_, cmdline);
	if (tokens == NULL)
	{
		logger_error("'%s' error", cmdline.c_str());
		return false;
	}
	if (strcasecmp(tokens->argv[0], "PAUSED"))
	{
		logger_error("'%s' error %s", cmdline.c_str(), tokens->argv[0]);
		ACL_SAFE_STRNCPY(errbuf_, tokens->argv[0], sizeof(errbuf_));
		acl_argv_free(tokens);
		return false;
	}

	acl_argv_free(tokens);
	return true;
}

void beanstalk::quit()
{
	if (conn_.opened())
	{
		conn_.format("quit\r\n");
		conn_.close();
	}
}

} // namespace acl
