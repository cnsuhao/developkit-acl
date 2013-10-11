#pragma once

namespace acl
{

class thread_pool;

/**
 * 线程纯虚类，该类的接口定义类似于 Java 的接口定义，子类需要实现
 * 基类的纯虚函数，使用者通过调用 thread::start() 启动线程过程
 */
class thread
{
public:
	thread();
	virtual ~thread();

	/**
	 * 开始启动线程过程
	 * @return {bool} 是否成功创建线程
	 */
	bool start();

	/**
	 * 当创建线程时为非 detachable 状态，则可以调用此函数
	 * 等待线程结束；否则，若创建线程时为 detachable 状态
	 * 在调用本函数时将会报错
	 * @param out {void**} 当该参数非空指针时，该参数用来存放
	 *  线程退出前返回的参数
	 * @return {bool} 是否成功
	 */
	bool wait(void** out);

	/**
	 * 在调用 start 前调用此函数可以设置所创建线程是否为
	 * 分离 (detachable) 状态；如果未调用此函数，则所创建
	 * 的线程默认为分离状态
	 * @param yes {bool} 是否为分离状态
	 * @return {thread&}
	 */
	thread& set_detachable(bool yes);

	/**
	 * 在调用 start 前调用此函数可以设置所创建线程的堆栈大小
	 * @param size {size_t} 线程堆栈大小，当该值为 0 或未
	 *  调用此函数，则所创建的线程堆栈大小为系统的默认值
	 * @return {thread&}
	 */
	thread& set_stacksize(size_t size);

	/**
	 * 在调用 start 后调用此函数可以获得所创建线程的 id 号
	 * @return {unsigned long}
	 */
	unsigned long thread_id() const;

	/**
	 * 当前调用者所在线程的线程 id 号
	 * @return {unsigned long}
	 */
	static unsigned long thread_self();

protected:
	friend class thread_pool;

	/**
	 * 纯虚函数，子类必须实现此函数，当用户调用 start 函数后，
	 * 该虚函数将被立即调用，用以启动一个子线程
	 * @return {void*} 线程退出前返回的参数
	 */
	virtual void* run() = 0;

private:
	bool detachable_;
	size_t stack_size_;
	unsigned long thread_id_;
#ifdef WIN32
	void* thread_;
#endif
	void* return_arg_;
	static void* thread_run(void* arg);
};

} // namespace acl
