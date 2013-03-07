#include "StdAfx.h"
#include "global/global.h"
#include "mail.h"
#include "mail_store.h"

mail_store::mail_store(const char* user, const char* smtp_ip,
	const char* pop3_ip, const MAIL_METER& meter,
	mail_callback& callback)
: callback_(callback)
, ok_(false)
{
	user_ = acl_mystrdup(user);
	smtp_ip_ = acl_mystrdup(smtp_ip);
	pop3_ip_ = acl_mystrdup(pop3_ip);

	meter_ = (MAIL_METER*) acl_mycalloc(1, sizeof(MAIL_METER));
	memcpy(meter_, &meter, sizeof(MAIL_METER));
}

mail_store::~mail_store()
{
	acl_myfree(user_);
	acl_myfree(smtp_ip_);
	acl_myfree(pop3_ip_);
	acl_myfree(meter_);
}

//////////////////////////////////////////////////////////////////////////
// 主线程过程

void mail_store::rpc_onover()
{
	logger("store mail test results %s!", ok_ ? "OK" : "Failed");
	callback_.mail_finish(dbpath_.c_str());
	delete this;
}

//////////////////////////////////////////////////////////////////////////
// 子线程过程

static const char* CREATE_TBL =
"create table mail_tbl\r\n"
"(\r\n"
"user varchar(128) not null,\r\n"
"smtp_ip varchar(32) not null,\r\n"
"smtp_nslookup_elapsed float(10,2) not null default 0.00,\r\n"
"smtp_connect_elapsed float(10,2) not null default 0.00,\r\n"
"smtp_envelope_eplased float(10,2) not null default 0.00,\r\n"
"smtp_auth_elapsed float(10,2) not null default 0.00,\r\n"
"smtp_data_elapsed float(10,2) not null default 0.00,\r\n"
"smtp_total_elapsed float(10,2) not null default 0.00,\r\n"
"pop3_ip varchar(32) not null,\r\n"
"pop3_nslookup_elapsed float(10,2) not null default 0.00,\r\n"
"pop3_connect_elapsed float(10,2) not null default 0.00,\r\n"
"pop3_auth_elapsed float(10,2) not null default 0.00,\r\n"
"pop3_list_elapsed float(10,2) not null default 0.00,\r\n"
"pop3_total_elapsed float(10,2) not null default 0.00\r\n"
");\r\n"
"create index user_idx on mail_tbl(user);\r\n";

void mail_store::rpc_run()
{
	const char* path = global::get_instance().get_path();
	dbpath_.format("%s/mail_store_%ld.db", path, time(NULL));

	acl::db_sqlite db(dbpath_.c_str());
	if (db.open() == false)
		logger_error("open db: %s failed", dbpath_.c_str());
	else if (create_tbl(db) == false)
		logger_error("create table failed for %s", dbpath_.c_str());
	else
	{
		logger("open db(%s) ok", dbpath_.c_str());
		insert_tbl(db);
		ok_ = true;
	}
}

bool mail_store::create_tbl(acl::db_handle& db)
{
	if (db.tbl_exists("mail_tbl"))
	{
		logger("mail_tbl table exist");
		return (true);
	}
	else if (db.sql_update(CREATE_TBL) == false)
	{
		logger_error("sql(%s) error", CREATE_TBL);
		return (false);
	}
	else
	{
		logger("create table mail_tbl ok");
		return (true);
	}
}

void mail_store::insert_tbl(acl::db_handle& db)
{
	acl::string sql;

	sql.format("insert into mail_tbl(user, smtp_ip, smtp_nslookup_elapsed, "
		"smtp_connect_elapsed, smtp_envelope_eplased, "
		"smtp_auth_elapsed, smtp_data_elapsed, smtp_total_elapsed, "
		"pop3_ip, pop3_nslookup_elapsed, pop3_connect_elapsed, "
		"pop3_auth_elapsed, pop3_list_elapsed, pop3_total_elapsed) "
		"values('%s', '%s', %0.2f, %0.2f, %0.2f, %0.2f, %0.2f, %0.2f, "
		"'%s', %0.2f, %0.2f, %0.2f, %0.2f, %0.2f)",
		user_, smtp_ip_, meter_->smtp_nslookup_elapsed,
		meter_->smtp_connect_elapsed, meter_->smtp_envelope_eplased,
		meter_->smtp_auth_elapsed, meter_->smtp_data_elapsed,
		meter_->smtp_total_elapsed, pop3_ip_,
		meter_->pop3_nslookup_elapsed, meter_->pop3_connect_elapsed,
		meter_->smtp_auth_elapsed, meter_->pop3_list_elapsed,
		meter_->pop3_total_elapsed);

	if (db.sql_update(sql.c_str()) == false)
		logger_error("sql(%s) error", sql.c_str());
	else
		logger("insert into mail_tbl OK!");
}
