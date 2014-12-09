#include "stdafx.h"
#include "server/ServerConnection.h"
#include "client/ClientConnection.h"
#include "status/StatusConnection.h"
#include "client/ManagerTimer.h"
#include "status/StatusTimer.h"
#include "rpc_manager.h"
#include "master_service.h"

//////////////////////////////////////////////////////////////////////////////
// 配置内容项

char *var_cfg_backend_service;
char *var_cfg_status_server;
char *var_cfg_status_service;
char *var_cfg_session_addr;
char *var_cfg_rpc_addr;
acl::master_str_tbl var_conf_str_tab[] = {
	{ "backend_service", "dispatch.sock", &var_cfg_backend_service },
	{ "status_server", "", &var_cfg_status_server },
	{ "status_service", "1080", &var_cfg_status_service },
	{ "session_addr", "127.0.0.1:11211", &var_cfg_session_addr },
	{ "rpc_addr", "127.0.0.1:0", &var_cfg_rpc_addr },

	{ 0, 0, 0 }
};

acl::master_bool_tbl var_conf_bool_tab[] = {
	{ 0, 0, 0 }
};

int   var_cfg_manage_timer;
int   var_cfg_conn_expired;
int   var_cfg_status_timer;
int   var_cfg_rw_timeout;
int   var_cfg_rpc_nthreads;
acl::master_int_tbl var_conf_int_tab[] = {
	{ "manager_timer", 1, &var_cfg_manage_timer, 0, 0 },
	{ "conn_expired", 10, &var_cfg_conn_expired, 0, 0 },
	{ "status_timer", 1, &var_cfg_status_timer, 0, 0 },
	{ "rw_timeout", 30, &var_cfg_rw_timeout, 0, 0 },
	{ "rpc_nthreads", 100, &var_cfg_rpc_nthreads, 0, 0 },

	{ 0, 0 , 0 , 0, 0 }
};

acl::master_int64_tbl var_conf_int64_tab[] = {
	{ 0, 0 , 0 , 0, 0 }
};

//////////////////////////////////////////////////////////////////////////////

master_service::master_service()
{
}

master_service::~master_service()
{
}

bool master_service::on_accept(acl::aio_socket_stream* client)
{
	if (0)
		logger("connect from %s, fd %d", client->get_peer(true),
			client->sock_handle());

	acl_non_blocking(client->sock_handle(), ACL_BLOCKING);

	// 根据客户端连接服务端口号的不同来区分不同的服务应用协议
	const char* local = client->get_local(true);
	if (acl_strrncasecmp(local, var_cfg_backend_service,
		strlen(var_cfg_backend_service)) == 0)
	{
		// 创建服务对象处理来自于后端服务模块的请求
		IConnection* conn = new ServerConnection(client);

		conn->run();
		return true;
	}
	else if (acl_strrncasecmp(local, var_cfg_status_service,
		strlen(var_cfg_status_service)) == 0)
	{
		// 创建服务对象处理状态汇报的请求
		IConnection* conn = new StatusConnection(client);

		conn->run();
		return true;
	}
	else
	{
		// 创建对象处理来自于前端客户端模块的请求
		IConnection* conn = new ClientConnection(client,
				var_cfg_conn_expired);

		conn->run();
		return true;
	}

	return true;
}

void master_service::proc_on_init()
{
	if (var_cfg_manage_timer <= 0)
		var_cfg_manage_timer = 1;

	// 启动后台定时器，用来处理未处理的前端客户端连接
	manage_timer_ = new ManagerTimer();
	manage_timer_->keep_timer(true);
	manage_timer_->set_task(1, var_cfg_manage_timer * 1000000);

	// 调用基类方法设置定时器任务
	proc_set_timer(manage_timer_);

	// 如果配置了状态服务器，则启动状态汇报定时器，定时向状态服务器
	// 汇报进程状态
	if (var_cfg_status_server && *var_cfg_status_server
		&& var_cfg_status_timer > 0)
	{
		// 启动服务器状态汇报定时器
		status_timer_ = new StatusTimer();
		status_timer_->keep_timer(true);
		status_timer_->set_task(1, var_cfg_status_timer * 1000000);

		// 调用基类方法设置定时器任务
		proc_set_timer(status_timer_);
	}

	// 调用基类函数获得异步引擎句柄
	acl::aio_handle* handle = get_handle();
	assert(handle != NULL);

	// 初始化 RPC 框架
	rpc_manager::get_instance().init(handle, var_cfg_rpc_nthreads,
		var_cfg_rpc_addr);
}

void master_service::proc_on_exit()
{
}
