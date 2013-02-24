#include "stdafx.h"
#include "rpc.h"

rpc::rpc(int max_threads /* = 10 */)
{
	// ������������ܾ���������� WIN32 ��Ϣģʽ��acl::ENGINE_WINMSG
	handle_ = new acl::aio_handle(acl::ENGINE_WINMSG);
	// ���� rpc �������
	service_ = new acl::rpc_service(max_threads, true);
	// ����Ϣ����
	if (service_->open(handle_) == false)
		logger_fatal("open service error: %s", acl::last_serror());
}

rpc::~rpc()
{
	delete service_;
	handle_->check();
	delete handle_;
	logger("rpc service destroy ok!");
}

void rpc::fork(acl::rpc_request* req)
{
	service_->rpc_fork(req);
}
