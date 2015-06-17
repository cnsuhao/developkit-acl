#include "stdafx.h"

static acl::thread_queue __queue;

//////////////////////////////////////////////////////////////////////////

class queue_item : public acl::queue_item
{
};

class cosumer : public acl::thread
{
public:
	consumer(bool auto_destroy = false) : auto_destroy_(auto_destroy) {}
	~consumer() {}

protected:
	void* run()
	{
		while (true)
		{

		}

		if (auto_destroy_)
			delete this;
		return NULL;
	}

private:
	bool auto_destroy_;
};

class producer : public acl::thread
{
public:
	producer(bool auto_destroy = false) : auto_destroy_(auto_destroy) {}
	~producer() {}

protected:
	void* run()
	{
		if (auto_desotry_)
			delete this;
		return NULL;
	}

private:
	bool auto_destroy_;
};

//////////////////////////////////////////////////////////////////////////

static void test_thread(void)
{
	const char* myname = "test_thread";
	mythread thr;

	thr.set_detachable(false);
	if (thr.start() == false)
	{
		printf("start thread failed\r\n");
		return;
	}

	printf("%s: thread id is %lu, main thread id: %lu\r\n",
		myname, thr.thread_id(), acl::thread::thread_self());

	if (thr.wait(NULL) == false)
		printf("wait thread failed\r\n");
	else
		printf("wait thread ok\r\n");
}

static void test_thread2(void)
{
	mythread* thr = new mythread(true);

	thr->set_detachable(true);
	if (thr->start() == false)
		printf("start thread failed\r\n");
}

int main(void)
{
	// ³õÊ¼»¯ acl ¿â
	acl::acl_cpp_init();

	test_thread();
	test_thread2();

	printf("enter any key to exit ...\r\n");
	getchar();

	return 0;
}
