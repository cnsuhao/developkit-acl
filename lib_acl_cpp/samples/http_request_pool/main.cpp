#include "stdafx.h"

using namespace acl;

static int __loop_count = 10;
static connect_pool* __conn_pool = NULL;
static acl_pthread_pool_t* __thr_pool = NULL;

static void init(const char* addr, int count)
{
	__conn_pool = new http_reuqest_pool(addr, count);
	__thr_pool = acl_thread_pool_create(count, 60);
}

static void end(void)
{
	acl_pthread_pool_destroy(__thr_pool);
	delete __conn_pool;
}

static bool http_get(http_request* conn, int n)
{
	http_header& header = conn->request_header();
	header.set_url("/")
		.set_keep_alive(true)
		.set_method(HTTP_METHOD_GET)
		.accept_gzip(false);

	printf("%lu--%d: begin send request\r\n", acl_pthread_self(), n);
	if (conn->request(NULL, 0) == false)
	{
		printf("%lu--%d: send GET request error\r\n",
			acl_pthread_self(), n);
		return false;
	}

	printf("%lu--%d: begin read body\r\n", acl_pthread_self(), n);
	char  buf[8192];
	int   ret, length = 0;
	while (true)
	{
		ret = conn->get_body(buf, sizeof(buf));
		if (ret == 0)
			break;
		else if (ret < 0)
			return false;
		length += ret;
		printf("%lu--%d: read length: %d, %d\r\n",
			acl_pthread_self(), n, length, ret);
	}
	printf("%lu--%d: read body over, length: %d\r\n",
		acl_pthread_self(), n, length);
	return true;
}

static void thread_main(void*)
{
	for (int i = 0; i < __loop_count; i++)
	{
		http_request* conn = (http_request*) __conn_pool->peek();
		if (conn == NULL)
		{
			printf("peek connect failed\r\n");
			break;
		}
		else
			conn->reset();
		if (http_get(conn, i) == false)
		{
			printf("one request failed, close connection\r\n");
			__conn_pool->put(conn, false);
		}
		else
			__conn_pool->put(conn, true);
	}
}

static void run(int count)
{
	for (int i = 0; i < count; i++)
		acl_pthread_pool_add(__thr_pool, thread_main, NULL);
}

static void usage(const char* procname)
{
	printf("usage: %s -h [help]\r\n"
		"	-s http_server_addr [www.sina.com.cn:80]\r\n"
		"	-c connect_pool_count\r\n"
		"	-n loop_count\r\n", procname);
}

int main(int argc, char* argv[])
{
	int   ch, count = 10;
	string addr("www.sina.com.cn:80");

	// ³õÊ¼»¯ acl ¿â
	acl::acl_cpp_init();

	while ((ch = getopt(argc, argv, "hs:n:c:")) > 0)
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
			count = atoi(optarg);
			break;
		case 'n':
			__loop_count = atoi(optarg);
			break;
		default:
			break;
		}
	}

	init(addr, count);
	run(count);
	end();

#ifdef WIN32
	printf("enter any key to exit...\r\n");
	getchar();
#endif

	return 0;
}
