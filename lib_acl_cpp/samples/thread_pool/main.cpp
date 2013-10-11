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
		return NULL;
	}

private:

};

//////////////////////////////////////////////////////////////////////////

class mythread_pool : public acl::thread_pool
{
public:
	mythread_pool() {}
	~mythread_pool() {}

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

static void test_thread_pool(void)
{
	mythread_pool threads;
	threads.start();

	mythread thread1;//, thread2;

	threads.execute(&thread1);
	//threads.execute(&thread2);
}

int main(int argc, char* argv[])
{
	// ≥ı ºªØ acl ø‚
	acl::acl_cpp_init();

	test_thread_pool();

#ifdef WIN32
	printf("enter any key to exit ...\r\n");
	getchar();
#endif

	return 0;
}
