#include "stdafx.h"
#include "mymonitor.h"

mymonitor::mymonitor(acl::connect_manager& manager,
	int check_inter, int conn_timeout)
	: acl::connect_monitor(manager, check_inter, conn_timeout)
{
}

mymonitor::~mymonitor(void)
{
}

bool mymonitor::on_open(acl::check_client& checker)
{
	checker_ = &checker;
	acl::aio_socket_stream& conn = checker_->get_conn();

	// 注册非阻塞 IO 处理过程的回调过程
	conn.add_close_callback(this);
	conn.add_read_callback(this);
	conn.add_timeout_callback(this);

	int timeout = 10;

	// 异步读取一行数据，同时要求不保留 \r\n

	conn.gets(timeout);

	// 返回 true 表示该检测过程继续
	return true;
}

bool mymonitor::read_callback(char* data, int len)
{
	// 因为 acl 的异步 IO 读到的数据肯定会在所读到的数据最后添加 \0，所以直接
	// 当字符串比较在此处是安全的

	printf(">>>get: %s, len: %d\r\n", data, len);

	if (strncasecmp(data, "+OK", 3) == 0)
	{
		// 将服务端连接置为存活状态
		checker_->set_alive(true);

		// 主动关闭该检测连接
		checker_->close();

		// 此处返回 true 或 false 都可以，因为上面已经主动要求关闭检测连接
		printf(">>> SERVER(%s) OK <<<\r\n", checker_->get_addr());
		return true;
	}

	// 将服务端置为不可用状态
	checker_->set_alive(false);

	printf(">>> SERVER(%s) ERROR <<<\r\n", checker_->get_addr());

	// 返回 false 通知框架自动关闭该连接
	return false;
}

bool mymonitor::timeout_callback()
{
	// 读超时，所以直接将连接置为不可用
	checker_->set_alive(false);

	// 返回 false 通过框架自动关闭该检测连接
	return false;
}

void mymonitor::close_callback()
{

}
