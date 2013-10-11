#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////

class mythread : public acl::thread
{
public:
	mythread() {}
	~mythread() {}

protected:
	virtual void* run()
	{
		const char* myname = "run";
		printf("%s: thread id: %lu, %lu\r\n",
			myname, thread_id(), acl::thread::thread_self());
		sleep(1);
		return NULL;
	}

private:

};

//////////////////////////////////////////////////////////////////////////

class mythread_pool : public acl::thread_pool
{
public:
	mythread_pool() {}
	~mythread_pool()
	{
		printf("thread pool destroy now, tid: %lu\r\n", acl_pthread_self());
	}

protected:
	virtual bool thread_on_init()
	{
		const char* myname = "thread_on_init";
		printf("%s: curr tid: %lu\r\n", myname,
			acl::thread::thread_self());
		return true;
	}

	virtual void thread_on_exit()
	{
		const char* myname = "thread_on_exit";
		printf("%s: curr tid: %lu\r\n", myname,
			acl::thread::thread_self());
	}
private:

};

//////////////////////////////////////////////////////////////////////////

static void test_thread_pool(int n)
{
	if (n == 0)
	{
		mythread_pool* threads = new mythread_pool;
		threads->start();

		mythread thread1, thread2;

		threads->execute(&thread1);
		threads->execute(&thread2);

		threads->stop();
		delete threads;
	}
	else if (n == 1)
	{
		mythread_pool threads;
		threads.start();

		mythread *thread1 = new mythread, *thread2 = new mythread;
		threads.execute(thread1);
		threads.execute(thread2);

		threads.stop();

		delete thread1;
		delete thread2;
	}
	else {
		mythread_pool threads;
		threads.start();

		mythread thread1, thread2;
		threads.execute(&thread1);
		threads.execute(&thread2);

		threads.stop();
	}
}

//////////////////////////////////////////////////////////////////////////

static void usage(const char* proc)
{
	printf("usage: %s -h [help]\r\n"
		"	-c which_case [0, 1, 2]\r\n",
		proc);
}

int main(int argc, char* argv[])
{
	int   ch, n = 0;

	// ³õÊ¼»¯ acl ¿â
	acl::acl_cpp_init();

	while ((ch = getopt(argc, argv, "hc:")) > 0)
	{
		switch (ch)
		{
		case 'h':
			usage(argv[0]);
			return 0;
		case 'c':
			n = atoi(optarg);
			break;
		default:
			break;
		}
	}

	test_thread_pool(n);
	return 0;
}
