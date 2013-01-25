#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/stream/socket_stream.hpp"

namespace acl
{

class ACL_CPP_API beanstalk
{
public:
	/**
	 * 构造函数
	 * @param addr {const char*} beanstalkd 地址，格式：ip:port/domain:port
	 * @param conn_timeout {int} 连接服务器的超时时间(秒)
	 * @param retry {bool} 如果连接断了是否自动重连
	 */
	beanstalk(const char* addr, int conn_timeout,
		bool retry = true);
	~beanstalk();

	/////////////////////////////////////////////////////////////////////
	// 生产者调用的接口

	/**
	 * 选择所用的发送管道
	 * @param tube {const char*} 管道名称
	 * @return {bool} 是否成功
	 */
	bool use(const char* tube);

	/**
	 * 向所选管道或缺省管理中发送消息
	 * @param data {const void*} 消息数据地址，可以是二进制
	 * @param n {size_t} data 数据体长度
	 * @param pri {unsigned int} 优先级，值越小，优先级越高
	 * @param delay {int} 表示将job放入ready队列需要等待的秒数
	 * @param ttr {int} 表示允许一个worker执行该job的秒数
	 * @return {bool} 是否成功
	 */
	bool put(const void* data, size_t n, unsigned int pri = 1024,
		int delay = 0, int ttr = 60);

	/////////////////////////////////////////////////////////////////////
	// 消费者调用的接口

	/**
	 * 选择读取消息的管道，将其加入监控管理列表中，
	 * 不调用本函数，则使用缺省的管道(default)
	 * @param tube {const char*} 管理名称
	 * @return {bool} 是否成功
	 */
	bool watch(const char* tube);

	/**
	 * 去除一个接收消息的管道
	 * @param tube {const char*} 管理名称
	 * @return {bool} 是否成功
	 */
	bool ignore(const char* tube);

	/**
	 * 从消息输出管道中获取一条消息，但并不删除消息，可以设置
	 * 等待超时，如果设为 0 
	 * @param buf {string&} 存储一条消息
	 */
	bool reserve(string& buf, int timeout = 0);
	bool delete_id(unsigned int id);
	bool release(unsigned int id, int pri = 1024, int delay = 0);
	bool bury(unsigned int id, int pri = 1024);
	bool touch(unsigned int id);

	/////////////////////////////////////////////////////////////////////
	// 其它接口
	bool open();
	void close();
	void quit();

	bool peek(string& buf, unsigned int id);
	bool peek_ready(string& buf);
	bool peek_delayed(string& buf);
	bool peek_buried(string& buf);
	int  kick(int n);
	bool list_tube_used(string&buf);
	bool list_tubes(string& buf);
	bool list_tubes_watched(string& buf);
	bool pause_tube(const char* tube, int delay);

	/////////////////////////////////////////////////////////////////////
	// 公共接口
	const char* get_error() const
	{
		return errbuf_;
	}

	unsigned int get_id() const
	{
		return id_;
	}

	unsigned int get_watch_count() const
	{
		return watched_;
	}

private:
	char* addr_;
	int   conn_timeout_;
	bool  retry_;
	char  errbuf_[128];
	unsigned int id_;
	unsigned int watched_;
	acl::socket_stream conn_;
	bool peek_fmt(string& buf, const char* fmt, ...);
	bool list_tubes_fmt(string& buf, const char* fmt, ...);
};

} // namespace acl
