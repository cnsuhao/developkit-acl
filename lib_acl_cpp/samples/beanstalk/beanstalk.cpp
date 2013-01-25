// beanstalk.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

static const char* __addr = "122.49.0.200:11300";
static const char* __tube = "zsxxsz";
static int __max = 100;

static void* producer(void* ctx)
{
	(void) ctx;

	acl::beanstalk conn(__addr, 10);

	if (conn.use(__tube) == false)
	{
		printf("use %s error\r\n", __tube);
		return NULL;
	}
	else
		printf("use %s ok\r\n", __tube);

	acl::string data;
	unsigned int id;
	for (int i = 0; i < __max; i++)
	{
		data.format("hello-%d", i);
		if (conn.put(data.c_str(), data.length(), &id) == false)
		{
			printf("put %s failed\r\n", data.c_str());
			return NULL;
		}
		else
			printf("put %s ok, id: %u\r\n", data.c_str(), id);
	}

	return NULL;
}

static void* consumer(void* ctx)
{
	(void) ctx;

	acl::beanstalk conn(__addr, 10);
	if (conn.watch(__tube) == false)
	{
		printf("watch %s faile\r\n", __tube);
		return NULL;
	}

	acl::string buf;
	unsigned int id;
	for (int i = 0; i < __max; i++)
	{
		if (conn.reserve(buf, &id) == false)
		{
			printf("reserve failed\r\n");
			return NULL;
		}
		else
			printf("reserved: %s\r\n", buf.c_str());
		if (conn.delete_id(id) == false)
		{
			printf("delete id %u failed\r\n", id);
			return NULL;
		}
		else
			printf("delete id %u ok\r\n", id);
	}

	return NULL;
}

static void test1()
{
	acl_pthread_attr_t attr;
	acl_pthread_t tid1, tid2;

	acl_pthread_attr_init(&attr);
	acl_pthread_attr_setdetachstate(&attr, 0);

	acl_pthread_create(&tid1, &attr, consumer, NULL);
	acl_pthread_create(&tid2, &attr, producer, NULL);

	acl_pthread_join(tid1, NULL);
	acl_pthread_join(tid2, NULL);
}

int main()
{
#if WIN32
	acl::acl_cpp_init();
#endif
	test1();

#ifdef WIN32
	printf("enter any key to exit\r\n");
	getchar();
#endif
	return 0;
}

