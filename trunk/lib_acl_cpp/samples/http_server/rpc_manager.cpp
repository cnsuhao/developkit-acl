#include "stdafx.h"
#include "rpc_manager.h"

rpc_manager::rpc_manager()
: handle_(NULL)
, service_(NULL)
{
}

rpc_manager::~rpc_manager()
{
	if (service_)
		delete service_;
	logger("rpc service destroy ok!");
}


void rpc_manager::init(acl::aio_handle* handle, int max_threads /* = 10 */,
	const char* rpc_addr /* = NULL */)
{
	handle_ = handle;

	// ��Ϊ����ʵ���ǵ��������ڳ��� main ֮ǰ�����ã�
	// ������Ҫ�ڴ����д���־
	logger_open("gui_rpc.log", "gui_rpc");
	// ���� rpc �������
	service_ = new acl::rpc_service(max_threads);
	// ����Ϣ����
	if (service_->open(handle_, rpc_addr) == false)
		logger_fatal("open service error: %s", acl::last_serror());
}

void rpc_manager::fork(acl::rpc_request* req)
{
	service_->rpc_fork(req);
}
