#include "acl_stdafx.hpp"
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/stdlib/locker.hpp"
#include "acl_cpp/connpool/connect_pool.hpp"
#include "acl_cpp/connpool/connect_manager.hpp"

namespace acl
{

connect_manager::connect_manager()
: default_pool_(NULL)
, service_idx_(0)
, stat_inter_(1)
{
}

connect_manager::~connect_manager()
{
	std::vector<connect_pool*>::iterator it = pools_.begin();

	// default_pool_ �Ѿ������� pools_ ����
	for (; it != pools_.end(); ++it)
		delete *it;
}

// ����һ����������ַ����ʽ��IP:PORT[:MAX_CONN]
// ����ֵ < 0 ��ʾ�Ƿ��ĵ�ַ
static int check_addr(const char* addr, string& buf, int default_count)
{
	// ���ݸ�ʽ��IP:PORT[:CONNECT_COUNT]
	ACL_ARGV* tokens = acl_argv_split(addr, ":|");
	if (tokens->argc < 2)
	{
		logger_error("invalid addr: %s", addr);
		acl_argv_free(tokens);
		return -1;
	}

	int port = atoi(tokens->argv[1]);
	if (port <= 0 || port >= 65535)
	{
		logger_error("invalid addr: %s, port: %d", addr, port);
		acl_argv_free(tokens);
		return -1;
	}
	buf.format("%s:%d", tokens->argv[0], port);
	int conn_max;
	if (tokens->argc >= 3)
		conn_max = atoi(tokens->argv[2]);
	else
		conn_max = default_count;
	if (conn_max <= 0)
		conn_max = default_count;
	acl_argv_free(tokens);
	return conn_max;
}

void connect_manager::init(const char* default_addr,
	const char* addr_list, int count)
{
	if (addr_list != NULL && *addr_list != 0)
		set_service_list(addr_list, count);

	// ����ȱʡ�������ӳض��󣬸ö���һͬ�����ܵ����ӳؼ�Ⱥ��
	if (default_addr != NULL && *default_addr != 0)
	{
		logger("default_pool: %s", default_addr);
		int max = check_addr(default_addr, default_addr_, count);
		default_pool_ = &set(default_addr_.c_str(), max);
	}
	else
		logger("no default redis set");

	// ���뱣֤������һ���������
	if (pools_.empty())
		logger_fatal("no redis service available!");
}

void connect_manager::set_service_list(const char* addr_list, int count)
{
	if (addr_list == NULL || *addr_list == 0)
	{
		logger("addr_list null");
		return;
	}

	// �������ӳط���Ⱥ
	char* buf = acl_mystrdup(addr_list);
	char* ptr = acl_mystr_trim(buf);
	ACL_ARGV* tokens = acl_argv_split(ptr, ";,");
	ACL_ITER iter;
	acl::string addr;
	acl_foreach(iter, tokens)
	{
		const char* ptr = (const char*) iter.data;
		int max = check_addr(ptr, addr, count);
		if (max <= 0)
		{
			logger_error("invalid redis addr: %s", addr.c_str());
			continue;
		}
		set(addr.c_str(), max);
		logger("add one service: %s, max connect: %d",
			addr.c_str(), max);
	}
	acl_argv_free(tokens);
	acl_myfree(buf);
}

connect_pool& connect_manager::set(const char* addr, int count)
{
	char key[256];
	ACL_SAFE_STRNCPY(key, addr, sizeof(key));
	acl_lowercase(key);

	lock_.lock();

	std::vector<connect_pool*>::iterator it = pools_.begin();
	for (; it != pools_.end(); ++it)
	{
		if (strcasecmp(key, (*it)->get_addr()) == 0)
		{
			lock_.unlock();
			return **it;
		}
	}

	connect_pool* pool = create_pool(key, count, pools_.size() - 1);
	pools_.push_back(pool);

	lock_.unlock();

	logger("Add one service, addr: %s, count: %d", addr, count);

	return *pool;
}

void connect_manager::remove(const char* addr)
{
	char key[256];
	ACL_SAFE_STRNCPY(key, addr, sizeof(key));
	acl_lowercase(key);

	lock_.lock();

	std::vector<connect_pool*>::iterator it = pools_.begin();
	for (; it != pools_.end(); ++it)
	{
		if (strcasecmp(key, (*it)->get_addr()) == 0)
		{
			(*it)->set_delay_destroy();
			pools_.erase(it);
			break;
		}
	}
	if (it == pools_.end())
		logger_warn("addr(%s) not found!", addr);

	lock_.unlock();
}


connect_pool* connect_manager::get(const char* addr,
	bool exclusive /* = true */)
{
	char key[256];
	ACL_SAFE_STRNCPY(key, addr, sizeof(key));
	acl_lowercase(key);

	if (exclusive)
		lock_.lock();

	std::vector<connect_pool*>::iterator it = pools_.begin();
	for (; it != pools_.end(); ++it)
	{
		if (strcasecmp(key, (*it)->get_addr()) == 0)
		{
			if (exclusive)
				lock_.unlock();
			return *it;
		}
	}

	if (exclusive)
		lock_.unlock();

	logger_error("no connect pool for addr %s", addr);
	return NULL;
}

//////////////////////////////////////////////////////////////////////////

connect_pool* connect_manager::peek()
{
	connect_pool* pool;
	size_t service_size, n;

	lock_.lock();
	service_size = pools_.size();
	if (service_size == 0)
	{
		lock_.unlock();
		logger_warn("pools's size is 0!");
		return NULL;
	}
	n = service_idx_ % service_size;
	service_idx_++;
	lock_.unlock();
	pool = pools_[n];
	return pool;
}

connect_pool* connect_manager::peek(const char* key,
	bool exclusive /* = true */)
{
	if (key == NULL || *key == 0)
		return peek();

	size_t service_size;
	connect_pool* pool;
	unsigned n = acl_hash_crc32(key, strlen(key));

	if (exclusive)
		lock_.lock();
	service_size = pools_.size();
	if (service_size == 0)
	{
		if (exclusive)
			lock_.unlock();
		logger_warn("pools's size is 0!");
		return NULL;
	}
	pool = pools_[n % service_size];
	if (exclusive)
		lock_.unlock();

	return pool;
}

void connect_manager::lock()
{
	lock_.lock();
}

void connect_manager::unlock()
{
	lock_.unlock();
}

void connect_manager::statistics_record(int, ACL_EVENT*, void* ctx)
{
	connect_manager* manager = (connect_manager*) ctx;

	// ��¼��ǰ����������
	manager->statistics();

	// �������ö�ʱ��
	manager->statistics_timer();
}

void connect_manager::statistics_settimer(int inter /* = 1 */)
{
	if (inter <= 0 || inter >= 3600 * 24 * 32)
	{
		logger_error("invalid inter: %d", inter);
		return;
	}

	stat_inter_ = inter;
#ifndef WIN32
	acl_ioctl_server_request_timer(statistics_record, this, inter);
#endif
	logger("set statistics_timer ok! inter: %d", inter);
}

void connect_manager::statistics_timer()
{
	statistics_settimer(stat_inter_);
}

void connect_manager::statistics()
{
	std::vector<connect_pool*>::const_iterator cit = pools_.begin();
	for (; cit != pools_.end(); ++cit)
	{
		logger("redis: %s, total: %llu, curr: %llu",
			(*cit)->get_addr(), (*cit)->get_total_used(),
			(*cit)->get_current_used());
		(*cit)->reset_statistics(stat_inter_);
	}
}

} // namespace acl
