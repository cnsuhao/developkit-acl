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

	// 因为本类实例是单例，会在程序 main 之前被调用，
	// 所以需要在此类中打开日志
	logger_open("gui_rpc.log", "gui_rpc");
	// 创建 rpc 服务对象
	service_ = new acl::rpc_service(max_threads);
	// 打开消息服务
	if (service_->open(handle_, rpc_addr) == false)
		logger_fatal("open service error: %s", acl::last_serror());
}

void rpc_manager::fork(acl::rpc_request* req)
{
	service_->rpc_fork(req);
}
