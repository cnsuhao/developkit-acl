#include "stdafx.h"
#include "ServerConnection.h"
#include "ClientConnection.h"
#include "ManagerTimer.h"
#include "StatusTimer.h"
#include "master_service.h"

//////////////////////////////////////////////////////////////////////////////
// ����������

char *var_cfg_backend_service;
char *var_cfg_status_server;
acl::master_str_tbl var_conf_str_tab[] = {
	{ "backend_service", "dispatch.sock", &var_cfg_backend_service },
	{ "status_server", "", &var_cfg_status_server },

	{ 0, 0, 0 }
};

acl::master_bool_tbl var_conf_bool_tab[] = {
	{ 0, 0, 0 }
};

int   var_cfg_manage_timer;
int   var_cfg_conn_expired;
int   var_cfg_status_timer;
acl::master_int_tbl var_conf_int_tab[] = {
	{ "manager_timer", 1, &var_cfg_manage_timer, 0, 0 },
	{ "conn_expired", 10, &var_cfg_conn_expired, 0, 0 },
	{ "status_timer", 1, &var_cfg_status_timer, 0, 0 },

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

	// ���ݿͻ������ӷ���˿ںŵĲ�ͬ�����ֲ�ͬ�ķ���Ӧ��Э��
	const char* local = client->get_local(true);
	if (acl_strrncasecmp(local, var_cfg_backend_service,
		strlen(var_cfg_backend_service)) == 0)
	{
		// ������������������ں�˷���ģ�������
		IConnection* conn = new ServerConnection(client);

		conn->run();
		return true;
	}
	else
	{
		// ����������������ǰ�˿ͻ���ģ�������
		IConnection* conn =
			new ClientConnection(client, var_cfg_conn_expired);

		conn->run();
		return true;
	}

	return true;
}

void master_service::proc_on_init()
{
	if (var_cfg_manage_timer <= 0)
		var_cfg_manage_timer = 1;

	// ������̨��ʱ������������δ�����ǰ�˿ͻ�������
	manage_timer_ = new ManagerTimer();
	manage_timer_->keep_timer(true);
	manage_timer_->set_task(1, var_cfg_manage_timer * 1000000);

	// ���û��෽�����ö�ʱ������
	proc_set_timer(manage_timer_);

	// ���������״̬��������������״̬�㱨��ʱ������ʱ��״̬������
	// �㱨����״̬
	if (var_cfg_status_server && *var_cfg_status_server
		&& var_cfg_status_timer > 0)
	{
		// ����������״̬�㱨��ʱ��
		status_timer_ = new StatusTimer();
		status_timer_->keep_timer(true);
		status_timer_->set_tack(1, var_cfg_status_timer * 1000000);

		// ���û��෽�����ö�ʱ������
		proc_set_timer(status_timer_);
	}
}

void master_service::proc_on_exit()
{
}
