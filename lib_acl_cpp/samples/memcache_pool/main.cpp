#include "stdafx.h"

using namespace acl;

static int __loop_count = 10;
static connect_pool* __conn_pool = NULL;
static acl_pthread_pool_t* __thr_pool = NULL;

// ��ʼ������
static void init(const char* addr, int count)
{
	// ���� HTTP �������ӳض���
	__conn_pool = new memcache_pool(addr, count);

	// �����̳߳�
	__thr_pool = acl_thread_pool_create(count, 60);
}

// �����˳�ǰ������Դ
static void end(void)
{
	// �����̳߳�
	acl_pthread_pool_destroy(__thr_pool);

	// �������ӳ�
	delete __conn_pool;
}

// MEMCACHE ������̣�����������������ӷ�������ȡ��Ӧ����
static bool memcache_get(memcache* conn, const char* key)
{
	const char* keypre = "test";
	string buf;

	conn->set_prefix(keypre);
	if (conn->get(key, buf) == false)
	{
		printf("get key: %s error\r\n", key);
		return false;
	}

	return true;
}

// �̴߳������
static void thread_main(void* ctx)
{
	char* key = (char*) ctx;

	for (int i = 0; i < __loop_count; i++)
	{
		// �����ӳ��л�ȡһ�� HTTP ����
		memcache* conn = (memcache*) __conn_pool->peek();
		if (conn == NULL)
		{
			printf("peek connect failed\r\n");
			break;
		}

		// ��ʼ�µ� HTTP �������
		if (memcache_get(conn, key) == false)
		{
			printf("one request failed, close connection\r\n");
			// ����������Ҫ�ر�
			__conn_pool->put(conn, false);
		}
		else
			__conn_pool->put(conn, true);
		if (i % 1000 == 0)
			printf("%lu--%d: read body over\r\n", acl_pthread_self(), i);
	}
}

static void run(int cocurrent, char* key)
{
	// ���̳߳����������
	for (int i = 0; i < cocurrent; i++)
		acl_pthread_pool_add(__thr_pool, thread_main, key);
}

static void usage(const char* procname)
{
	printf("usage: %s -h [help]\r\n"
		"	-s memcache_server_addr [127.0.0.1:11211]\r\n"
		"	-k key\r\n"
		"	-c cocurrent [default: 10]\r\n"
		"	-n loop_count[default: 10]\r\n", procname);
}

int main(int argc, char* argv[])
{
	int   ch, cocurrent = 10;
	string addr("127.0.0.1:11211");
	string  key;

	// ��ʼ�� acl ��
	acl::acl_cpp_init();

	while ((ch = getopt(argc, argv, "hs:n:c:k:")) > 0)
	{
		switch (ch)
		{
		case 'h':
			usage(argv[0]);
			return 0;
		case 's':
			addr = optarg;
			break;
		case 'c':
			cocurrent = atoi(optarg);
			break;
		case 'n':
			__loop_count = atoi(optarg);
			break;
		case 'k':
			key = optarg;
			break;
		default:
			usage(argv[0]);
			return 0;
		}
	}

	if (key.empty())
	{
		usage(argv[0]);
		return 0;
	}

	init(addr, cocurrent);
	run(cocurrent, key.c_str());
	end();

#ifdef WIN32
	printf("enter any key to exit...\r\n");
	getchar();
#endif

	return 0;
}
