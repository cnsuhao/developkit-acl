#include "acl_stdafx.hpp"
#include "acl_cpp/stdlib/thread.hpp"
#include "acl_cpp/stdlib/thread_pool.hpp"

namespace acl
{

thread_pool::thread_pool()
: stack_size_(0)
, threads_limit_(100)
, thread_idle_(0)
, thr_pool_(NULL)
{
	thr_attr_ = (acl_pthread_pool_attr_t*)
		acl_mycalloc(1, sizeof(acl_pthread_pool_attr_t));
}

thread_pool::~thread_pool()
{
	if (thr_pool_)
		acl_pthread_pool_destroy(thr_pool_);
	acl_myfree(thr_attr_);
}

thread_pool& thread_pool::set_stacksize(size_t size)
{
	stack_size_ = size;
	return *this;
}

thread_pool& thread_pool::set_limit(size_t n)
{
	threads_limit_ = n;
	return *this;
}

thread_pool& thread_pool::set_idle(int ttl)
{
	thread_idle_ = ttl;
	return *this;
}

void thread_pool::start()
{
	if (thr_pool_)
		return;

	acl_pthread_pool_attr_set_stacksize(thr_attr_, stack_size_);
	acl_pthread_pool_attr_set_threads_limit(thr_attr_, threads_limit_);
	acl_pthread_pool_attr_set_idle_timeout(thr_attr_, thread_idle_);

	thr_pool_ = acl_pthread_pool_create(thr_attr_);
	acl_pthread_pool_atinit(thr_pool_, thread_init, this);
	acl_pthread_pool_atfree(thr_pool_, thread_exit, this);
}

void thread_pool::stop()
{
	acl_pthread_pool_stop(thr_pool_);
}

void thread_pool::destroy()
{
	if (thr_pool_)
	{
		acl_pthread_pool_destroy(thr_pool_);
		thr_pool_ = NULL;
	}
}

bool thread_pool::execute(thread* thr)
{
	if (thr == NULL)
	{
		logger_error("thr null!");
		return false;
	}

	printf("thr addr: %p\r\n", thr);
	return acl_pthread_pool_add(thr_pool_, thread_run, thr)
		== 0 ? true : false;
}

int  thread_pool::curr_threads() const
{
	if (thr_pool_ == NULL)
		return -1;

	return acl_pthread_pool_size(thr_pool_);
}

void thread_pool::thread_run(void* arg)
{
	printf("arg addr: %p\r\n", arg);
	thread* thr = (thread*) arg;
	(void*) thr->run();
}

int  thread_pool::thread_init(void* arg)
{
	thread_pool* threads = (thread_pool*) arg;
	return threads->thread_on_init() ? 0 : -1;
}

void thread_pool::thread_exit(void* arg)
{
	thread_pool* threads = (thread_pool*) arg;
	threads->thread_on_exit();
}

} // namespace acl
