#include "acl_stdafx.hpp"
#include <map>
#include "acl_cpp/connpool/connect_manager.hpp"
#include "acl_cpp/connpool/connect_monitor.hpp"
#include "acl_cpp/connpool/check_client.hpp"
#include "check_timer.hpp"

namespace acl
{

connect_monitor::connect_monitor(connect_manager& manager,
	int check_inter /* = 1 */, int conn_timeout /* = 10 */)
: stop_(false)
, stop_graceful_(true)
, handle_(ENGINE_KERNEL)
, manager_(manager)
, check_inter_(check_inter)
, conn_timeout_(conn_timeout)
{
}

connect_monitor::~connect_monitor()
{
}

void connect_monitor::stop(bool graceful)
{
	stop_ = true;
	stop_graceful_ = graceful;
}

void* connect_monitor::run()
{
	// 检查服务端连接状态定时器
	check_timer timer(*this, handle_, conn_timeout_);

	timer.keep_timer(true);  // 保持定时器
	handle_.set_timer(&timer, check_inter_ * 1000000);

	while (!stop_)
		handle_.check();

	// 等待定时器结束
	while (!timer.finish(stop_graceful_))
		handle_.check();

	return NULL;
}

bool connect_monitor::on_open(check_client& checker)
{
	// 设置状态表明该连接是存活的
	checker.set_alive(true);

	// 不必显式调用 close，因为下面直接返回 false 通讯框架会自动关闭，
	// 虽然在此处调用一次 close 也是无害的
	// checker.close();

	// 返回 false 以使上级调用者可以关闭异步连接
	return false;
}

} // namespace acl
