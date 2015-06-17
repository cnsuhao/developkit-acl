#include "acl_stdafx.hpp"
#include "acl_cpp/stdlib/thread_queue.hpp"

namespace acl
{

thread_queue::thread_queue()
{
	queue_ = (ACL_AQUEUE*) acl_aqueue_new();
}

static void free_qitem(void* item)
{
	thread_qitem* qitem = (thread_qitem *) item;
	delete qitem;
}

thread_queue::~thread_queue()
{
	acl_aqueue_free(queue_, free_qitem);
}

bool thread_queue::push(thread_qitem* item)
{
	return acl_aqueue_push(queue_, item) == -1 ? false : true;
}

thread_qitem* thread_queue::pop(int wait_sec /* = 0 */, int wait_usec /* = 0 */)
{
	return (thread_qitem*) acl_aqueue_pop_timedwait(
				queue_, wait_sec, wait_usec);
}

int thread_queue::qlen() const
{
	return acl_aqueue_qlen(queue_);
}

} // namespace acl
