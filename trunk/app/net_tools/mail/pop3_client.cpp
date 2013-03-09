#include "StdAfx.h"
#include "rpc/rpc_manager.h"
#include "pop3_store.h"
#include "pop3_client.h"

pop3_client::pop3_client()
{
	memset(&meter_, 0, sizeof(meter_));
	meter_.pop3_nslookup_elapsed = 0.00;
	meter_.pop3_connect_elapsed = 0.00;
	meter_.pop3_auth_elapsed = 0.00;
	meter_.pop3_list_elapsed = 0.00;
	meter_.pop3_total_elapsed = 0.00;
}

pop3_client::~pop3_client()
{

}

pop3_client& pop3_client::set_callback(pop3_callback* c)
{
	callback_ = c;
	return *this;
}

pop3_client& pop3_client::set_conn_timeout(int n)
{
	connect_timeout_ = n;
	return *this;
}

pop3_client& pop3_client::set_rw_timeout(int n)
{
	rw_timeout_ = n;
	return *this;
}

pop3_client& pop3_client::set_account(const char* s)
{
	auth_account_ = s;
	return *this;
}

pop3_client& pop3_client::set_passwd(const char* s)
{
	auth_passwd_ = s;
	return *this;
}

pop3_client& pop3_client::set_pop3(const char* addr, int port)
{
	pop3_addr_ = addr;
	pop3_port_ = port;
	pop3_ip_ = addr;
	return *this;
}

//////////////////////////////////////////////////////////////////////////

struct UP_CTX
{
	acl::string msg;
	size_t total;
	size_t curr;
};

//////////////////////////////////////////////////////////////////////////
// 主线程中运行

void pop3_client::rpc_onover()
{
	pop3_store* pop3 = new pop3_store(auth_account_.c_str(), pop3_ip_.c_str(),
		meter_, *callback_);
	rpc_manager::get_instance().fork(pop3);
	delete this;
}

void pop3_client::rpc_wakeup(void* ctx)
{
	UP_CTX* up = (UP_CTX*) ctx;

	callback_->pop3_report(up->msg.c_str(),
		up->total, up->curr, meter_);
	delete up;
}

//////////////////////////////////////////////////////////////////////////
// 子线程中运行

void pop3_client::rpc_run()
{
}

//////////////////////////////////////////////////////////////////////////
