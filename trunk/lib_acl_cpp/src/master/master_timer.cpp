#include "acl_stdafx.hpp"
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/master/master_timer.hpp"

namespace acl
{

//////////////////////////////////////////////////////////////////////////

class master_timer_task
{
public:
	master_timer_task() {}
	~master_timer_task() {}

private:
	friend class master_timer;

	unsigned int id;
	acl_int64 delay;
	acl_int64 when;
};

//////////////////////////////////////////////////////////////////////////

master_timer::master_timer(bool keep /* = false */)
{
	keep_ = keep;
	length_ = 0;
}

master_timer::~master_timer(void)
{
	// 如果正在触发定时器的回调过程中析构过程被调用则会发生严重问题
	if (locked())
	{
		logger_error("In trigger proccess, you delete me now!");
		acl_assert(0);
	}

	clear();
}

int master_timer::clear()
{
	int  n = 0;
	std::list<master_timer_task*>::iterator it = tasks_.begin();
	for (; it != tasks_.end(); ++it)
	{
		delete (*it);
		n++;
	}
	tasks_.clear();
	length_ = 0;
	return n;
}

bool master_timer::empty() const
{
	return tasks_.empty();
}

size_t master_timer::length() const
{
	return length_;
}

void master_timer::keep_timer(bool on)
{
	keep_ = on;
}

bool master_timer::keep_timer(void) const
{
	return keep_;
}

void master_timer::set_time(void)
{
	struct timeval now;
	gettimeofday(&now, NULL);
	present_ = ((unsigned long) now.tv_sec) * 1000000
		+ ((unsigned long) now.tv_usec);
}

#define TIMER_EMPTY		-1

acl_int64 master_timer::del_task(unsigned int id)
{
	bool ok = false;
	std::list<master_timer_task*>::iterator it = tasks_.begin();
	for (; it != tasks_.end(); ++it)
	{
		if ((*it)->id == id)
		{
			delete (*it);
			tasks_.erase(it);
			length_--;
			ok = true;
			break;
		}
	}

	if (!ok)
		logger_warn("timer id: %u not found", id);

	if (tasks_.empty())
		return TIMER_EMPTY;

	set_time();

	acl_int64 delay = tasks_.front()->when - present_;
	return delay < 0 ? 0 : delay;
}

acl_int64 master_timer::set_task(unsigned int id, acl_int64 delay)
{
	master_timer_task* task = NULL;
	std::list<master_timer_task*>::iterator it = tasks_.begin();
	for (; it != tasks_.end(); ++it)
	{
		if ((*it)->id == id)
		{
			task = (*it);
			tasks_.erase(it);
			length_--;
			break;
		}
	}

	if (task == NULL)
	{
		task = NEW master_timer_task();
		task->delay = delay;
		task->id = id;
	}
	else
		task->delay = delay;

	return set_task(task);
}

acl_int64 master_timer::set_task(master_timer_task* task)
{
	set_time();
	task->when = present_ + task->delay;

	std::list<master_timer_task*>::iterator it = tasks_.begin();
	for (; it != tasks_.end(); ++it)
	{
		if (task->when < (*it)->when)
		{
			tasks_.insert(it, task);
			break;
		}
	}

	if (it == tasks_.end())
		tasks_.push_back(task);

	length_++;

	master_timer_task* first = tasks_.front();
	acl_int64 delay = first->when - present_;
	return delay < 0 ? 0 : delay;
}

acl_int64 master_timer::trigger(void)
{
	// sanity check
	if (tasks_.empty())
		return TIMER_EMPTY;

	acl_assert(length_ > 0);

	set_time();

	std::list<master_timer_task*>::iterator it, next;
	std::list<master_timer_task*> tasks;
	master_timer_task* task;

	// 从定时器中取出到达的定时任务
	for (it = tasks_.begin(); it != tasks_.end(); it = next)
	{
		if ((*it)->when > present_)
			break;
		next = it;
		++next;
		task = *it;
		tasks_.erase(it);
		length_--;
		tasks.push_back(task);
	}

	// 有可能这些到达的定时任务已经被用户提前删除了
	if (tasks.empty())
	{
		acl_assert(!tasks_.empty());

		master_timer_task* first = tasks_.front();
		acl_int64 delay = first->when - present_;
		return delay < 0 ? 0 : delay;
	}

	// 将到达的定时任务重新放回至定时器的任务列表中，
	// 并开始触发所有的到达的定时任务

	// 必须先设置触发器的忙状态，以防止子类在回调过程
	// 中调用了该类对象的析构过程
	set_locked();

	// 设置解锁后销毁标志为 false，因为当前该定时器处于
	// 锁定状态，所以其它类对象不能直接在锁定时销毁本类
	// 对象，当解锁后，如果该标识被置为 true，则本类对象
	// 应该自动销毁
	destroy_on_unlock_ = false;

	for (it = tasks.begin(); it != tasks.end(); ++it)
	{
		set_task(*it);
		timer_callback((*it)->id);
	}

	tasks.clear();

	// 允许之后的操作中被子类调用析构过程
	unset_locked();

	// 子类有可能会在 timer_callback 中删除了所有的定时任务
	if (tasks_.empty())
		return TIMER_EMPTY;

	master_timer_task* first = tasks_.front();
	acl_int64 delay = first->when - present_;

	// 如果在加锁期间外部程序要求释放该对象，则在此处释放
	if (destroy_on_unlock_)
		destroy();
	return delay < 0 ? 0 : delay;
}

}  // namespace acl
