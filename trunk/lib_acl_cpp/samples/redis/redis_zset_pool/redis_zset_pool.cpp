#include "stdafx.h"

static acl::string __keypre("zset_key");

static bool test_zadd(acl::redis_zset& option, int i, size_t length)
{
	acl::string key;
	std::map<acl::string, double> members;
	acl::string member;

	key.format("%s_%d", __keypre.c_str(), i);

	for (int j = 0; j < 1000; j++)
	{
		member.format("member_%d", j);
		members[member] = j;
	}

	int ret = option.zadd(key, members);
	if (ret < 0)
	{
		printf("add key: %s error\r\n", key.c_str());
		return false;
	}
	else if (i < 10)
		printf("add ok, key: %s, ret: %d\r\n", key.c_str(), ret);

	return true;
}

static bool test_zcard(acl::redis_zset& option, int i)
{
	acl::string key;

	key.format("%s_%d", __keypre.c_str(), i);
	int ret = option.zcard(key.c_str());
	if (ret < 0)
	{
		printf("zcard key: %s error\r\n", key.c_str());
		return false;
	}
	else if (i < 10)
		printf("zcard ok, key: %s, count: %d\r\n", key.c_str(), ret);

	return true;
}

static bool test_zrange(acl::redis_zset& option, int i)
{
	acl::string key;
	int start = 0, end = 100;
	std::vector<acl::string> result;

	key.format("%s_%d", __keypre.c_str(), i);
	int ret = option.zrange(key, start, end, result);
	if (ret <= 0)
		return false;
	return true;
}

/////////////////////////////////////////////////////////////////////////////

class test_thread : public acl::thread
{
public:
	test_thread(acl::redis_pool& pool, const char* cmd,
		int n, size_t length)
		: pool_(pool), cmd_(cmd), n_(n), length_(length) {}

	~test_thread() {}

protected:
	virtual void* run()
	{
		bool ret;
		acl::redis_client* conn;
		acl::redis_zset option;

		for (int i = 0; i < n_; i++)
		{
			conn = (acl::redis_client*) pool_.peek();
			
			if (conn == NULL)
			{
				printf("peek redis_client failed\r\n");
				break;
			}

			option.set_client(conn);

			if (cmd_ == "del")
				ret = test_zadd(option, i, length_);
			else if (cmd_ == "expire")
				ret = test_zcard(option, i);
			else if (cmd_ == "ttl")
				ret = test_zrange(option, i);
			else if (cmd_ == "all")
			{
				if (test_zadd(option, i, length_) == false
					|| test_zcard(option, i) == false
					|| test_zrange(option, i) == false)
				{
					ret = false;
				}
				else
					ret = true;
			}
			else
			{
				printf("unknown cmd: %s\r\n", cmd_.c_str());
				break;
			}

			pool_.put(conn, ret && !conn->eof());

			if (ret == false)
				break;
		}

		return NULL;
	}

private:
	acl::redis_pool& pool_;
	acl::string cmd_;
	int n_;
	size_t length_;
};

static void usage(const char* procname)
{
	printf("usage: %s -h[help]\r\n"
		"-s redis_addr[127.0.0.1:6379]\r\n"
		"-n count[default: 10]\r\n"
		"-C connect_timeout[default: 10]\r\n"
		"-I rw_timeout[default: 10]\r\n"
		"-c max_threads[default: 10]\r\n"
		"-l max_data_length\r\n"
		"-a cmd\r\n",
		procname);
}

int main(int argc, char* argv[])
{
	int  ch, n = 1, conn_timeout = 10, rw_timeout = 10;
	int  max_threads = 10;
	size_t length = 102400;
	acl::string addr("127.0.0.1:6379"), cmd;

	while ((ch = getopt(argc, argv, "hs:n:C:I:c:a:l:")) > 0)
	{
		switch (ch)
		{
		case 'h':
			usage(argv[0]);
			return 0;
		case 's':
			addr = optarg;
			break;
		case 'n':
			n = atoi(optarg);
			break;
		case 'C':
			conn_timeout = atoi(optarg);
			break;
		case 'I':
			rw_timeout = atoi(optarg);
			break;
		case 'c':
			max_threads = atoi(optarg);
			break;
		case 'a':
			cmd = optarg;
			break;
		case 'l':
			length = (unsigned long) atol(optarg);
			break;
		default:
			break;
		}
	}

	acl::acl_cpp_init();
	acl::redis_pool pool(addr.c_str(), max_threads);
	pool.set_timeout(conn_timeout, rw_timeout);

	std::vector<test_thread*> threads;
	for (int i = 0; i < max_threads; i++)
	{
		test_thread* thread = new test_thread(pool, cmd.c_str(),
			n, length);
		threads.push_back(thread);
		thread->set_detachable(false);
		thread->start();
	}

	std::vector<test_thread*>::iterator it = threads.begin();
	for (; it != threads.end(); ++it)
	{
		(*it)->wait();
		delete (*it);
	}

#ifdef WIN32
	printf("enter any key to exit\r\n");
	getchar();
#endif

	return 0;
}
