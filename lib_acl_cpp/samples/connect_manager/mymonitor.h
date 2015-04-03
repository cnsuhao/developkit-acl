#pragma once

class mymonitor : public acl::connect_monitor, public acl::aio_callback
{
public:
	mymonitor(acl::connect_manager& manager,
		int check_inter = 1, int conn_timeout = 10);
	~mymonitor(void);

protected:
	/**
	 * 基类 connect_monitor 虚函数，重载本函数用来进一步判断该连接是否是存活的
	 * @param checker {check_client&} 服务端连接的检查对象，可以通过
	 *  check_client 类中的方法如下：
	 *  1) get_conn 获得非阻塞连接句柄
	 *  2) get_addr 获得服务端地址
	 *  3) set_alive 设置连接是否存活
	 *  4) close 关闭连接
	 * @return {bool} 返回 true 表示子类还需要进一步进行检测，返回 false
	 *  则表明子类已经检测完毕
	 */
	bool on_open(acl::check_client& checker);

protected:
	// 重载父类 aio_callback 中的虚函数

	/**
	 * 客户端流的读成功回调过程
	 * @param data {char*} 读到的数据地址
	 * @param len {int} 读到的数据长度
	 * @return {bool} 返回 true 表示继续，否则希望关闭该异步流
	 */
	bool read_callback(char* data, int len);

	/**
	 * 客户端流的超时回调过程
	 * @return {bool} 返回 true 表示继续，否则希望关闭该异步流
	 */
	bool timeout_callback();

	/**
	 * 客户端流的超时回调过程
	 */
	void close_callback();

private:
	acl::check_client* checker_;
};
