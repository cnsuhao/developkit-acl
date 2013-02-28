#include "StdAfx.h"
#include "upload.h"

upload::upload(upload_callback* callback, const char* pingDb,
	const char* dnsDb, const char* smtpAddr, int connectTimeout,
	int rwTimeout)
	: callback_(callback)
{

}

upload::~upload()
{

}

//////////////////////////////////////////////////////////////////////////
// 主线程中运行

void upload::rpc_onover()
{

}

void upload::rpc_wakeup(void* ctx)
{

}

//////////////////////////////////////////////////////////////////////////
// 子线程中运行

void upload::rpc_run()
{

}
