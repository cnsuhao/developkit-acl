#include "stdafx.h"

static acl::string __keypre("test_key");

static void test_set(acl::redis_string& option, int n)
{
	acl::string key;
	acl::string value;

	for (int i = 0; i < n; i++)
	{
		key.format("%s_%d", __keypre.c_str(), i);
		value.format("value_%s_%d", key.c_str());

		if (option.set(key.c_str(), value.c_str()) == false)
		{
			printf("set key: %s error\r\n", key.c_str());
			break;
		}
	}
}

static void test_setex(acl::redis_string& option, int n)
{
	acl::string key, value;

	for (int i = 0; i < n; i++)
	{
		key.format("%s_%d", __keypre.c_str(), i);
		if (option.setex(key.c_str(), value.c_str(), 100) == false)
		{
			printf("setex key: %s error\r\n", key.c_str());
			break;
		}
	}
}

static void test_setnx(acl::redis_string& option, int n)
{
	acl::string key;
	acl::string value;

	for (int i = 0; i < n; i++)
	{
		key.format("%s_%d", __keypre.c_str(), i);
		value.format("value_%s", key.c_str());

		if (option.setnx(key.c_str(), value.c_str()) < 0)
		{
			printf("setnx key: %s error\r\n", key.c_str());
			break;
		}
	}
}

static void test_append(acl::redis_string& option, int n)
{
	acl::string key;
	acl::string value;

	for (int i = 0; i < n; i++)
	{
		key.format("%s_%d", __keypre.c_str(), i);
		value.format("value_%s", key.c_str());

		if (option.append(key.c_str(), value.c_str()) < 0)
		{
			printf("append key: %s\r\n", key.c_str());
			break;
		}
	}
}

static void test_get(acl::redis_string& option, int n)
{
	acl::string key;
	acl::string value;

	for (int i = 0; i < n; i++)
	{
		key.format("%s_%d", __keypre.c_str(), i);
		value.clear();

		if (option.get(key.c_str(), value) == false)
		{
			printf("get key: %s\r\n", key.c_str());
			break;
		}
	}
}

static void usage(const char* procname)
{
	printf("usage: %s -h[help]\r\n"
		"-s redis_addr[127.0.0.1:6380]\r\n"
		"-n count\r\n"
		"-C connect_timeout[default: 10]\r\n"
		"-T rw_timeout[default: 10]\r\n"
		"-a cmd\r\n",
		procname);
}

int main(int argc, char* argv[])
{
	int  ch, n = 1, conn_timeout = 10, rw_timeout = 10;
	acl::string addr("127.0.0.1:6380"), cmd;

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
	acl::log::stdout_open(true);
	acl::redis_client client(addr.c_str(), conn_timeout, rw_timeout);
	acl::redis_string option(client);

	if (cmd == "set")
		test_set(option, n);
	else if (cmd == "setex")
		test_setex(option, n);
	else if (cmd == "setnx")
		test_setnx(option, n);
	else if (cmd == "append")
		test_append(option, n);
	else if (cmd == "get")
		test_get(option, n);
	/*
	else if (cmd == "getset")
		test_getset(option, n);
	else if (cmd == "strlen")
		test_strlen(option, n);
	else if (cmd == "setrange")
		test_setrange(option, n);
	else if (cmd == "getrange")
		test_getrange(option, n);
	else if (cmd == "setbit")
		test_setbit(option, n);
	else if (cmd == "getbit")
		test_getbit(option, n);
	else if (cmd == "bitcount")
		test_bitcount(option, n);
	else if (cmd == "bitop_and")
		test_bitop_and(option, n);
	else if (cmd == "bitop_or")
		test_bitop_or(option, n);
	else if (cmd == "bitop_xor")
		test_bitop_xor(option, n);
	else if (cmd == "mset")
		test_mset(option, n);
	else if (cmd == "msetnx")
		test_msetnx(option, n);
	else if (cmd == "mget")
		test_mget(option, n);
	else if (cmd == "incr")
		test_incr(option, n);
	else if (cmd == "incrby")
		test_incrby(option, n);
	else if (cmd == "incrbyfloat")
		test_incrbyfloat(option, n);
	else if (cmd == "decr")
		test_decr(option, n);
	else if (cmd == "decrby")
		test_decrby(option, n);
	*/
	else
		printf("unknown cmd: %s\r\n", cmd.c_str());

#ifdef WIN32
	printf("enter any key to exit\r\n");
	getchar();
#endif
	return 0;
}
