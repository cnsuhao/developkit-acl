#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <map>
#include <vector>
#include "acl_cpp/stream/aio_socket_stream.hpp"                                
#include "acl_cpp/stdlib/string.hpp"

namespace acl
{

class check_timer;
class aio_socket_stream;

/**
 * 异步连接回调函数处理类
 */
class check_client : public aio_open_callback
{
public:
	check_client(check_timer& timer, const char* addr,
		aio_socket_stream& conn, struct timeval& begin);

	/**
	 * 获得输入的非阻塞 IO 句柄
	 * @return {aio_socket_stream&}
	 */
	aio_socket_stream& get_conn() const
	{
		return conn_;
	}

	/**
	 * 获得传入的服务端地址
	 * @return {const char*}
	 */
	const char* get_addr() const
	{
		return addr_.c_str();
	}

	/**
	 * 设置连接是否是存活的
	 * @param yesno {bool}
	 */
	void set_alive(bool yesno);

	/**
	 * 关闭非阻塞 IO 句柄
	 */
	void close();

private:
	// 基类虚函数
	bool open_callback();
	void close_callback();
	bool timeout_callback();

private:
	~check_client() {}

private:
	bool aliving_;
	struct timeval begin_;
	check_timer& timer_;
	aio_socket_stream& conn_;
	string addr_;
};

} // namespace acl
