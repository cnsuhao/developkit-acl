#include "stdafx.h"

static acl::string __keypre("zset_key");

static void test_zadd(acl::redis_zset& option, int n)
{
	acl::string key;
	std::map<acl::string, double> members;
	acl::string member;

	for (int i = 0; i < n; i++)
	{
		key.format("%s_%d", __keypre.c_str(), i);

		for (int j = 0; j < 1000; j++)
		{
			member.format("member_%d", j);
			members[member] = i;
		}

		option.reset();
		int ret = option.zadd(key, members);
		if (ret < 0)
		{
			printf("add key: %s error\r\n", key.c_str());
			break;
		}
		else if (i < 10)
			printf("add ok, key: %s\r\n", key.c_str());
	}
}

static void test_zcard(acl::redis_zset& option, int n)
{
	acl::string key;

	for (int i = 0; i < n; i++)
	{
		key.format("%s_%d", __keypre.c_str(), i);
		option.reset();
		int ret = option.zcard(key.c_str());
		if (ret < 0)
		{
			printf("zcard key: %s error\r\n", key.c_str());
			break;
		}
		else if (i < 10)
			printf("zcard ok, key: %s, count: %d\r\n",
				key.c_str(), ret);
	}
}

static void test_zcount(acl::redis_zset& option, int n)
{
	acl::string key;
	double min = 2, max = 10;

	for (int i = 0; i < n; i++)
	{
		key.format("%s_%d", __keypre.c_str(), i);
		option.reset();
		int ret = option.zcount(key.c_str(), min, max);
		if (ret < 0)
		{
			printf("zcount key: %s error\r\n", key.c_str());
			break;
		}
		else if (i < 10)
			printf("zcount ok, key: %s, count: %d\r\n",
				key.c_str(), ret);
	}
}

static void test_zincrby(acl::redis_zset& option, int n)
{
	acl::string key;
	double inc = 2, result;
	acl::string member;

	for (int i = 0; i < n; i++)
	{
		key.format("%s_%d", __keypre.c_str(), i);

		for (int j = 0; j < 1000; j++)
		{
			member.format("member_%d", j);

			option.reset();
			if (option.zincrby(key.c_str(), inc, member.c_str(),
				&result) == false)
			{
				printf("zincrby error, key: %s\r\n", key.c_str());
				break;
			}
			else if (j < 10 && i * j < 100)
				printf("zincrby ok key: %s, result: %.2f\r\n",
					key.c_str(), result);
		}
	}
}

static void test_zrange(acl::redis_zset& option, int n)
{
	acl::string key;
	std::vector<acl::string> result;
	int start = 0, stop = 10;

	for (int i = 0; i < n; i++)
	{
		key.format("%s_%d", __keypre.c_str(), i);
		option.reset();
		result.clear();

		int ret = option.zrange(key.c_str(), start, stop, result);
		if (ret < 0)
		{
			printf("zrange error, key: %s\r\n", key.c_str());
			break;
		}
		else if (i < 10)
			printf("zrange ok, key: %s, ret: %d\r\n",
				key.c_str(), ret);
		result.clear();
	}

	std::vector<std::pair<acl::string, double> > result2;

	for (int i = 0; i < n; i++)
	{
		key.format("%s_%d", __keypre.c_str(), i);
		option.reset();
		result.clear();

		int ret = option.zrange_with_scores(key.c_str(), start, stop,
				result2);
		if (ret < 0)
		{
			printf("zrange error, key: %s\r\n", key.c_str());
			break;
		}
		else if (i < 10)
			printf("zrange ok, key: %s, ret: %d\r\n",
				key.c_str(), ret);
		result2.clear();
	}
}

static void test_zrangebyscore(acl::redis_zset& option, int n)
{
	acl::string key;
	std::vector<acl::string> result;
	double min = 2, max = 10;

	for (int i = 0; i < n; i++)
	{
		key.format("%s_%d", __keypre.c_str(), i);
		option.reset();
		result.clear();

		int ret = option.zrangebyscore(key.c_str(), min, max, result);
		if (ret < 0)
		{
			printf("zrangebyscore error, key: %s\r\n", key.c_str());
			break;
		}
		else if (i < 10)
			printf("zrangebyscore ok, key: %s, ret: %d\r\n",
				key.c_str(), ret);
		result.clear();
	}
}

static void test_zrank(acl::redis_zset& option, int n)
{
	acl::string key;
	acl::string member;

	for (int i = 0; i < n; i++)
	{
		key.format("%s_%d", __keypre.c_str(), i);
		option.reset();

		for (int j = 0; j < 1000; j++)
		{
			int ret = option.zrank(key.c_str(), member.c_str());
			if (ret < 0)
			{
				printf("zrank error, key: %s\r\n", key.c_str());
				break;
			}
			else if (i < 10)
				printf("zrank ok, key: %s, ret: %d\r\n",
					key.c_str(), ret);
		}
	}
}
static void usage(const char* procname)
{
	printf("usage: %s -h[help]\r\n"
		"-s redis_addr[127.0.0.1:6379]\r\n"
		"-n count\r\n"
		"-C connect_timeout[default: 10]\r\n"
		"-T rw_timeout[default: 10]\r\n"
		"-a cmd\r\n",
		procname);
}

int main(int argc, char* argv[])
{
	int  ch, n = 1, conn_timeout = 10, rw_timeout = 10;
	acl::string addr("127.0.0.1:6379"), cmd;

	while ((ch = getopt(argc, argv, "hs:n:C:T:a:")) > 0)
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
		case 'T':
			rw_timeout = atoi(optarg);
			break;
		case 'a':
			cmd = optarg;
			break;
		default:
			break;
		}
	}

	acl::acl_cpp_init();
	acl::redis_client client(addr.c_str(), conn_timeout, rw_timeout);
	acl::redis_zset option(&client);

	if (cmd == "zadd")
		test_zadd(option, n);
	else if (cmd == "zcard")
		test_zcard(option, n);
	else if (cmd == "zcount")
		test_zcount(option, n);
	else if (cmd == "zincrby")
		test_zincrby(option, n);
	else if (cmd == "zrange")
		test_zrange(option, n);
	else if (cmd == "zrangebyscore")
		test_zrangebyscore(option, n);
	else if (cmd == "zrank")
		test_zrank(option, n);
#if 0
	else if (cmd == "zrem")
		test_zrem(option, n);
	else if (cmd == "zremrangebyrank")
		test_zremrangebyrank(option, n);
	else if (cmd == "zremrangebyscore")
		test_zremrangebyscore(option, n);
	else if (cmd == "zrevrange")
		test_zrevrange(option, n);
	else if (cmd == "zrevrangebyscore")
		test_zrevrangebyscore(option, n);
	else if (cmd == "zrevrank")
		test_zrevrank(option, n);
	else if (cmd == "zscore")
		test_zscore(option, n);
	else if (cmd == "zunionstore")
		test_zunionstore(option, n);
	else if (cmd == "zinterstore")
		test_zinterstore(option, n);
	else if (cmd == "zscan")
		test_zscan(option, n);
	else if (cmd == "zrangebylex")
		test_zrangebylex(option, n);
	else if (cmd == "zlexcount")
		test_zlexcount(option, n);
	else if (cmd == "zremrangebylex")
		test_zremrangebylex("option, n");
#endif
	else if (cmd == "all")
	{
	}
	else
		printf("unknown cmd: %s\r\n", cmd.c_str());

#ifdef WIN32
	printf("enter any key to exit\r\n");
	getchar();
#endif
	return 0;
}
