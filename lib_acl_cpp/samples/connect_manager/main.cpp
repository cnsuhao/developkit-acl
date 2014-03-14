#include "stdafx.h"

using namespace acl;

static int __loop_count = 10;
static connect_manager* __conn_manager = NULL;
static acl_pthread_pool_t* __thr_pool = NULL;
static bool __unzip = false;

// 初始化过程
static void init(const char* addrs, int count)
{
	// 创建 HTTP 请求连接池集群管理对象
	__conn_manager = new http_request_manager();

	// 添加服务器集群地址
	__conn_manager->init(addrs, addrs, 100);

	printf(">>>start monitor thread\r\n");

	// 启动后台检测线程
	int  check_inter = 1, conn_timeout = 5;
	__conn_manager->start_monitor(check_inter, conn_timeout);

	printf(">>>sleep 15 seconds\r\n");
	for (int i = 20; i < 15; i++)
	{
		putchar('.');
		fflush(stdout);
		sleep(1);
	}
	printf("\r\n");

	printf(">>>create thread pool\r\n");
	// 创建线程池
	__thr_pool = acl_thread_pool_create(count, 60);
}

// 进程退出前清理资源
static void end(void)
{
	// 销毁线程池
	acl_pthread_pool_destroy(__thr_pool);

	// 停止后台检测线程
	__conn_manager->stop_monitor(true);

	// 销毁连接池
	delete __conn_manager;
}

// HTTP 请求过程，向服务器发送请求后从服务器读取响应数据
static bool http_get(http_request* conn, const char* addr, int n)
{
	printf(">>>check addr: %s, n: %d\r\n", addr, n);

	// 创建 HTTP 请求头数据
	http_header& header = conn->request_header();
	header.set_url("/")
		.set_host(addr)
		.set_keep_alive(true)
		.set_method(HTTP_METHOD_GET)
		.accept_gzip(__unzip);

	printf("%lu--%d: begin send request\r\n",
		(unsigned long) acl_pthread_self(), n);
	// 发送 HTTP 请求数据同时接收 HTTP 响应头
	if (conn->request(NULL, 0) == false)
	{
		printf("%lu--%d: send GET request error\r\n",
			(unsigned long) acl_pthread_self(), n);
		return false;
	}

	char  buf[8192];
	int   ret, length = 0;

	// 接收 HTTP 响应体数据
	while (true)
	{
		ret = conn->read_body(buf, sizeof(buf));
		if (ret == 0)
			break;
		else if (ret < 0)
		{
			printf("%lu--%d: error, length: %d\r\n",
				(unsigned long) acl_pthread_self(),
				n, length);
			return false;
		}
		length += ret;
		if (0)
			printf("%lu--%d: read length: %d, %d\r\n",
				(unsigned long) acl_pthread_self(),
				n, length, ret);
	}
	printf("%lu--%d: read body over, length: %d\r\n",
		(unsigned long) acl_pthread_self(), n, length);
	return true;
}

// 线程处理过程
static void thread_main(void*)
{
	for (int i = 0; i < __loop_count; i++)
	{
		http_request_pool* pool = (http_request_pool*)
			__conn_manager->peek();
		if (pool == NULL)
		{
			printf("peek pool failed\r\n");
			break;
		}

		// 设置连接的超时时间及读超时时间
		pool->set_timeout(2, 2);

		// 从连接池中获取一个 HTTP 连接
		http_request* conn = (http_request*) pool->peek();
		if (conn == NULL)
		{
			printf("peek connect failed\r\n");
			break;
		}

		// 需要对获得的连接重置状态，以清除上次请求过程的临时数据
		else
			conn->reset();

		// 开始新的 HTTP 请求过程
		if (http_get(conn, pool->get_addr(), i) == false)
		{
			printf("one request failed, close connection\r\n");
			// 错误连接需要关闭
			pool->put(conn, false);
		}
		else
			pool->put(conn, true);
	}

	printf(">>>>thread: %lu OVER<<<<", (unsigned long) acl_pthread_self());
}

static void run(int cocurrent)
{
	// 向线程池中添加任务
	for (int i = 0; i < cocurrent; i++)
		acl_pthread_pool_add(__thr_pool, thread_main, NULL);
}

static void usage(const char* procname)
{
	printf("usage: %s -h [help]\r\n"
		"	-s http_server_addrs [www.sina.com.cn:80;www.263.net:80;www.qq.com:80]\r\n"
		"	-z [unzip response body, default: false]\r\n"
		"	-c cocurrent [default: 10]\r\n"
		"	-n loop_count[default: 10]\r\n", procname);
}

int main(int argc, char* argv[])
{
	int   ch, cocurrent = 10;
	string addrs("www.sina.com.cn:80;www.263.net:80;www.qq.com:81");

	// 初始化 acl 库
	acl::acl_cpp_init();

	while ((ch = getopt(argc, argv, "hs:n:c:z")) > 0)
	{
		switch (ch)
		{
		case 'h':
			usage(argv[0]);
			return 0;
		case 's':
			addrs = optarg;
			break;
		case 'c':
			cocurrent = atoi(optarg);
			break;
		case 'n':
			__loop_count = atoi(optarg);
			break;
		case 'z':
			__unzip = true;
			break;
		default:
			usage(argv[0]);
			return 0;
		}
	}

	init(addrs, cocurrent);
	run(cocurrent);
	end();

#ifdef WIN32
	printf("enter any key to exit...\r\n");
	getchar();
#endif

	return 0;
}
