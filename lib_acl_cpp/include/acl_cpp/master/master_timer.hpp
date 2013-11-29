#pragma once
#include "acl_cpp/acl_cpp_define.hpp"

namespace acl
{

/**
 * 需要被延迟释放的类继承此类后，可以调用 aio_handle:delay_free 来到达
 * 延迟销毁的目的，避免了在递归过程中被立即释放时的对象被提前释放的问题
 */
class ACL_CPP_API aio_delay_free
{
public:
	aio_delay_free(void);
	virtual ~aio_delay_free(void);

	/**
	 * 判定定时器是否正处于锁定状态，处于锁定状态的定时器是
	 * 不能被删除的，否则会造成内存严重错误
	 * @return {bool} 是否处于锁定状态，处于锁定状态的对象是
	 *  不允许被销毁的
	 */
	bool locked(void) const;

	/**
	 * 允许子类设置子类对象的锁定对象，这样在定时器处理过程中就不会
	 * 自动调用子类对象的销毁过程
	 */
	void set_locked(void);

	/**
	 * 允许子类取消类对象的锁定状态
	 */
	void unset_locked(void);

	/**
	 * 销毁函数，在内部类 aio_timer_delay_free 对象中对需要做延迟释放
	 * 的类进行销毁
	 */
	virtual void destroy(void) {}
private:
	bool locked_;
	bool locked_fixed_;
};

class master_timer_task;

class ACL_CPP_API master_timer
{
public:
	/**
	 * 构造函数
	 * @param keep {bool} 该定时器是否允许自动重启
	 */
	master_timer(bool keep = false);
	virtual ~master_timer();

	/**
	 * 定时器里的任务是否为空
	 * @return {bool}
	 */
	bool empty(void) const;

	/**
	 * 定时器里的任务个数
	 * @return {size_t}
	 */
	size_t length(void) const;

	/**
	 * 该定时器是否是自动重启的
	 * @param on {bool}
	 */
	void keep_timer(bool on);

	/**
	 * 判断该定时器是否是自动重启的
	 * @return {bool}
	 */
	bool keep_timer(void) const;

	/**
	 * 清空定时器里的定时任务
	 * @return {int} 被清除的定时任务的个数
	 */
	int clear(void);

protected:
	/**
	 * 子类必须实现此回调函数，注：子类或调用者禁止在
	 * timer_callback 内部调用 master_timer 的析构
	 * 函数，否则将会酿成大祸
	 * @param id {unsigned int} 对应某个任务的 ID 号
	 */
	virtual void timer_callback(unsigned int id) = 0;

	/****************************************************************/
	/*        子类可以调用如下函数添加一些新的定时器任务 ID 号              */
	/****************************************************************/
#ifdef WIN32
	__int64 present_;

	/**
	 * 针对本定时器增加新的任务ID号，这样便可以通过一个定时器启动
	 * 多个定时任务
	 * @param id {unsigned int} 定时器定时任务ID号
	 * @param delay {__int64} 每隔多久自动触发该定时器，同时将对应的定时器定时
	 *  任务ID号传回(微秒级)
	 * @return {__int64} 距离本定时器的第一个将会触发的定时任务ID还多久(微秒级)
	 */
	__int64 set_task(unsigned int id, __int64 delay);

	/**
	 * 删除定时器中某个消息ID对应的定时任务
	 * @param {unsigned int} 定时任务ID
	 * @return {__int64} 距离本定时器的第一个将会触发的定时任务ID还多久(微秒级)
	 */
	__int64 del_task(unsigned int id);
#else
	long long int present_;
	long long int set_task(unsigned int id, long long int delay);
	long long int del_task(unsigned int id);
#endif

private:
	size_t length_;
	std::list<master_timer_task*> tasks_;
	bool keep_;  // 该定时器是否允许自动重启
	bool destroy_on_unlock_;  // 解锁后是否 destroy
#ifdef WIN32
	__int64 set_task(aio_timer_task* task);
	__int64 trigger(void);
#else
	long long int set_task(aio_timer_task* task);
	long long int trigger(void);
#endif
};

}  // namespace acl
