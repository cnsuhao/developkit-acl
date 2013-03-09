#include "StdAfx.h"
#include "rpc/rpc_manager.h"
#include "pop3_client.h"
#include "pop3_store.h"

pop3_store::pop3_store(const char* user, const char* pop3_ip,
	const POP3_METER& meter, pop3_callback& callback)
: callback_(callback)
, ok_(false)
{
	user_ = acl_mystrdup(user);
	pop3_ip_ = acl_mystrdup(pop3_ip);

	meter_ = (POP3_METER*) acl_mycalloc(1, sizeof(POP3_METER));
	memcpy(meter_, &meter, sizeof(POP3_METER));
}

pop3_store::~pop3_store()
{
	acl_myfree(user_);
	acl_myfree(pop3_ip_);
	acl_myfree(meter_);
}

//////////////////////////////////////////////////////////////////////////
// 主线程过程

void pop3_store::rpc_onover()
{
	logger("store pop3 test results %s!", ok_ ? "OK" : "Failed");
	callback_.pop3_finish(dbpath_.c_str());
	delete this;
}

//////////////////////////////////////////////////////////////////////////
// 子线程过程

static const char* CREATE_TBL =
"create table pop3_tbl\r\n"
"(\r\n"
"user varchar(128) not null,\r\n"
"pop3_ip varchar(32) not null,\r\n"
"pop3_nslookup_elapsed float(10,2) not null default 0.00,\r\n"
"pop3_connect_elapsed float(10,2) not null default 0.00,\r\n"
"pop3_auth_elapsed float(10,2) not null default 0.00,\r\n"
"pop3_list_elapsed float(10,2) not null default 0.00,\r\n"
"pop3_total_elapsed float(10,2) not null default 0.00\r\n"
");\r\n"
"create index pop3_user_idx on pop3_tbl(user);\r\n";

void pop3_store::rpc_run()
{

}

bool pop3_store::create_tbl(acl::db_handle& db)
{
	if (db.tbl_exists("pop3_tbl"))
	{
		logger("pop3_tbl table exist");
		return (true);
	}
	else if (db.sql_update(CREATE_TBL) == false)
	{
		logger_error("sql(%s) error", CREATE_TBL);
		return (false);
	}
	else
	{
		logger("create table pop3_tbl ok");
		return (true);
	}
}

void pop3_store::insert_tbl(acl::db_handle& db)
{
	acl::string sql;

	sql.format("insert into pop3_tbl(user, pop3_ip, pop3_nslookup_elapsed, "
		"pop3_connect_elapsed, pop3_auth_elapsed, pop3_list_elapsed, "
		"pop3_total_elapsed) "
		"values('%s', '%s', %0.2f, %0.2f, %0.2f, %0.2f, %0.2f)",
		user_, pop3_ip_, meter_->pop3_nslookup_elapsed,
		meter_->pop3_connect_elapsed, meter_->pop3_auth_elapsed,
		meter_->pop3_list_elapsed, meter_->pop3_total_elapsed);

	if (db.sql_update(sql.c_str()) == false)
		logger_error("sql(%s) error", sql.c_str());
	else
		logger("insert into pop3_tbl OK!");
}
