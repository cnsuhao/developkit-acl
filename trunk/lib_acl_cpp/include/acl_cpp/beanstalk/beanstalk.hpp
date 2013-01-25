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
	beanstalk(const char* addr, int conn_timeout, bool retry = true);
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
	 * @param id {unsigned int*} 当正确添加一条消息后，若该指针非空，
	 *  则用来存储所添加消息的ID号
	 * @param pri {unsigned int} 优先级，值越小，优先级越高
	 * @param delay {int} 表示将job放入ready队列需要等待的秒数
	 * @param ttr {int} 表示允许一个worker执行该消息的秒数
	 * @return {bool} 是否成功，如果成功，则可以通过 get_id()
	 *  来获得队列服务器给本消息分配的消息号
	 */
	bool put(const void* data, size_t n, unsigned int* id = NULL,
		unsigned int pri = 1024, int delay = 0, int ttr = 60);

	/////////////////////////////////////////////////////////////////////
	// 消费者调用的接口

	/**
	 * 选择读取消息的管道，将其加入监控管理列表中，
	 * 不调用本函数，则使用缺省的管道(default)
	 * @param tube {const char*} 管理名称
	 * @param n {int*} 如果该指针非空，则当命令成功返回后该地址存储
	 *  当前连接所关注的消息管道(tube)的数量
	 * @return {bool} 是否成功
	 */
	bool watch(const char* tube, int* n = NULL);

	/**
	 * 取消关注(watch)一个接收消息的管道(tube)
	 * @param tube {const char*} 管理名称
	 * @param n {int*} 如果该指针非空，则当命令成功返回后该地址存储
	 *  当前连接所关注的消息管道(tube)的数量
	 * @return {bool} 是否成功
	 */
	bool ignore(const char* tube, int* n = NULL);

	/**
	 * 从消息输出管道中获取一条消息，但并不删除消息，可以设置
	 * 等待超时，如果设为 0 
	 * @param buf {string&} 存储获得的一条消息
	 * @param id {unsigned int*} 当正确获得一条消息后，若该指针非空，
	 *  则用来存储所获得消息的ID号
	 * @param timeout {int} 等待队列服务器返回消息的超时值，当为 -1
	 *  时，则无限期等待，当 > 0 时，则在该时间内若没有消息，则返回，
	 *  当 == 0 时，则立即返回一条消息或返回超时
	 * @return {bool} 是否获得一条消息
	 */
	bool reserve(string& buf, unsigned int* id = NULL, int timeout = -1);

	/**
	 * 从队列服务器中删除指定 ID 号的消息
	 * @param id {unsigned int} 消息号
	 * @return {bool} 是否成功删除
	 */
	bool delete_id(unsigned int id);

	/**
	 * 将一个已经被获取的消息重新放回ready队列(并将job状态置为 "ready")，
	 * 让该job可以被其他客户端执行
	 * @param id {unsigned int} 消息号
	 * @param pri {int} 优先级别
	 * @param delay {int} 表示在该消息被放入ready队列之前需要等待的秒数
	 * @return {bool} 是否成功
	 */
	bool release(unsigned int id, int pri = 1024, int delay = 0);

	/**
	 * 命令将一个消息的状态置为 "buried", Buried 消息被放在一个FIFO的链表中，
	 * 在客户端调用kick命令之前，这些消息将不会被服务端处理
	 * @param id {unsigned int} 消息号
	 * @param pri {int} 优先级别
	 * @return {bool} 是否成功
	 */
	bool bury(unsigned int id, int pri = 1024);

	/**
	 * 命令允许一个worker请求在一个消息获取更多执行的时间。这对于那些需要
	 * 长时间完成的消息是非常有用的，但同时也可能利用TTR的优势将一个消息
	 * 从一个无法完成工作的worker处移走。一个worker可以通过该命令来告诉
	 * 服务端它还在执行该job (比如：在收到DEADLINE_SOON是可以发生给命令)
	 * @param id {unsigned int} 消息号
	 * @return {bool} 是否成功
	 */
	bool touch(unsigned int id);

	/////////////////////////////////////////////////////////////////////
	// 其它接口

	/**
	 * 连接 beanstalkd 服务器，通常情况下不需要显示地调用该函数，上述命令
	 * 会自动根据需要自动调用本函数
	 * @return {bool}  否成功
	 */
	bool open();

	/**
	 * 显示关闭与 beanstalkd 的连接，当该类实例析构时会尝试调用关闭过程
	 */
	void close();

	/**
	 * 显示通知 beanstalkd 服务器退出连接(服务器收到此命令后会立即关闭连接)
	 */
	void quit();

	/**
	 * 获取消息队列中指定的消息号的数据
	 * @param buf {string&} 如果消息存在则存储该条消息
	 * @param id {unsigned int} 指定的消息号
	 * @return {bool} 是否获得指定消息号的消息
	 */
	bool peek(string& buf, unsigned int id);

	/**
	 * 获得当前关注 (watch) 管道中的一条 ready 状态消息，
	 * 如果消息不存在也立即返回
	 * @param buf {string&} 如果消息存在则存储该条消息
	 * @param id {unsigned int*} 当正确获得一条消息后，若该指针非空，
	 *  则用来存储所获得消息的ID号
	 * @return {bool} 是否获得消息
	 */
	bool peek_ready(string& buf, unsigned int* id);

	/**
	 * 获得当前关注 (watch) 管道中的一条 delayed 状态消息，
	 * 如果消息不存在也立即返回
	 * @param buf {string&} 如果消息存在则存储该条消息
	 * @param id {unsigned int*} 当正确获得一条消息后，若该指针非空，
	 *  则用来存储所获得消息的ID号
	 * @return {bool} 是否获得消息
	 */
	bool peek_delayed(string& buf, unsigned int* id);

	/**
	 * 获得当前关注 (watch) 管道中的一条 buried 状态消息，
	 * 如果消息不存在也立即返回
	 * @param buf {string&} 如果消息存在则存储该条消息
	 * @param id {unsigned int*} 当正确获得一条消息后，若该指针非空，
	 *  则用来存储所获得消息的ID号
	 * @return {bool} 是否获得消息
	 */
	bool peek_buried(string& buf, unsigned int* id);

	/**
	 * 该命令只能针对当前正在使用的tube执行；它将 buried
	 * 或者 delayed 状态的消息移动到 ready 队列
	 * @param n {int} 表示每次 kick 消息的上限，服务端将最多 kick
	 *  的消息数量
	 * @return {int} 表示本次kick操作作用消息的数目，返回 -1 表示出错
	 */
	int  kick(int n);

	/**
	 * 获得客户当前正在使用的消息管道
	 * @param buf {string&} 存储当前使用的消息管道
	 * @return {bool} 是否成功获得
	 */
	bool list_tube_used(string&buf);

	/**
	 * 获得已经存在的所有消息管道(tube)的列表集合
	 * @param buf {string&} 存储结果
	 * @return {bool} 是否成功获得
	 */
	bool list_tubes(string& buf);

	/**
	 * 获得当前关注(watch)的消息管道的集合
	 * @param buf {string&} 存储结果
	 * @return {bool} 是否成功获得
	 */
	bool list_tubes_watched(string& buf);

	/**
	 * 给定时间内暂停从指定消息管道(tube)中获取消息
	 * @param tube {const char*} 消息管道名
	 * @param delay {int} 指定时间段
	 * @return {bool} 是否成功
	 */
	bool pause_tube(const char* tube, int delay);

	/////////////////////////////////////////////////////////////////////
	// 公共接口
	const char* get_error() const
	{
		return errbuf_;
	}

private:
	char* addr_;
	int   timeout_;
	bool  retry_;
	char  errbuf_[128];
	acl::socket_stream conn_;
	bool peek_fmt(string& buf, unsigned int* id, const char* fmt, ...);
	bool list_tubes_fmt(string& buf, const char* fmt, ...);
};

} // namespace acl
