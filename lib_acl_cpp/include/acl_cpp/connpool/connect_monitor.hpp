#pragma once
#include <vector>
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/stdlib/thread.hpp"
#include "acl_cpp/stream/aio_handle.hpp"

namespace acl
{

class aio_handle;
class connect_manager;

class connect_monitor : public thread
{
public:
	connect_monitor(connect_manager& manager,
		int check_inter = 1, int conn_timeout = 10);
	~connect_monitor();

	/**
	 * 停止线程
	 */
	void stop();

protected:
	// 基类纯虚函数
	virtual void* run();

private:
	bool stop_;
	aio_handle handle_;			// 后台检测线程的非阻塞句柄
	connect_manager& manager_;		// 连接池集合管理对象
	int   check_inter_;			// 检测连接池状态的时间间隔(秒)
	int   conn_timeout_;			// 连接服务器的超时时间
};

} // namespace acl
