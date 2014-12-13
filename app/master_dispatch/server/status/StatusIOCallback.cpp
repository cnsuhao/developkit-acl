#include "stdafx.h"
#include "rpc_manager.h"
#include "status/HttpServerRpc.h"
#include "status/StatusConnection.h"
#include "status/StatusIOCallback.h"

StatusIOCallback::StatusIOCallback(StatusConnection* conn)
: conn_(conn)
{
	rpc_ = new HttpServerRpc(conn_->get_conn());
}

StatusIOCallback::~StatusIOCallback()
{
	delete conn_;
	delete rpc_;
}

// ���������������ݿɶ�ʱ
bool StatusIOCallback::read_wakeup()
{
	// �Ƚ�ֹ�첽�����
	conn_->get_conn()->disable_read();
	// ����һ�� http ����˻Ự���̣���֮�������߳�ȥ����
	rpc_manager::get_instance().fork(rpc_);
	return true;
}

// �������������ر�ʱ���ûص�������������
void StatusIOCallback::close_callback()
{
	// ɾ���Լ�
	delete this;
}

bool StatusIOCallback::timeout_callback()
{
	logger_error("read timeout from: %s", conn_->get_peer());
	return false;
}
