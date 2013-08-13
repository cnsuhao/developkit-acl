#pragma once
#include <list>

namespace acl
{

class connect_client;

/**
 * 提供与服务器连接池的映射关系
 */
class connect_pool
{
public:
	connect_pool(const char* addr, int count, int retry_inter = 1);
	virtual ~connect_pool();

	/**
	 * 从该服务器的连接池探测是否可用，对于出错情况进行重试
	 * @return {connect_client*} 如果为空则表示该服务器连接池对象不可用
	 */
	connect_client* peek();

	/**
	 * 释放一个连接，同时将 peek 中创建的互斥锁释放
	 * @param conn {redis_client*}
	 * @param keep {bool} 是否针对该连接保持长连接
	 */
	void put(connect_client* conn, bool keep = true);

	/**
	 * 设置连接池的存活状态
	 * @param ok {bool} 设置该连接是否正常
	 */
	void set_alive(bool ok /* true | false */);

	/**
	 * 获取连接池的服务器地址
	 * @return {const char*} 返回非空地址
	 */
	const char* get_addr() const
	{
		return addr_;
	}

	/**
	 * 获取连接池最大连接数限制
	 * @return {int}
	 */
	int get_count() const
	{
		return count_;
	}

	/**
	 * 重置统计计数器
	 * @param inter {int} 统计的时间间隔
	 */
	void reset_statistics(int inter)
	{
		time_t now = time(NULL);
		lock_.lock();
		if (now - last_ >= inter)
		{
			last_ = now;
			current_used_ = 0;
		}
		lock_.unlock();
	}

	/**
	 * 获取该连接池总共被使用的次数
	 */
	unsigned long long get_total_used() const
	{
		return total_used_;
	}

	/**
	 * 获取该连接池当前的使用次数
	 * @return {unsigned long long}
	 */
	unsigned long long get_current_used() const
	{
		return current_used_;
	}

protected:
	virtual connect_client* create_connect() = 0;
private:
	// 是否属正常
	bool  alive_;
	// 有问题的服务器的可以重试的时间间隔
	int   retry_inter_;
	time_t last_dead_;
	char  addr_[64];
	int   max_;	// 最大连接数
	int   count_;	// 当前的连接数

	locker lock_;
	unsigned long long total_used_;		// 该连接池的所有访问量
	unsigned long long current_used_;	// 某时间段内的访问量
	time_t last_;				// 上次记录的时间截
	std::list<connect_client*> pool_;	// 连接池集合
};

} // namespace acl
