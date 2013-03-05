#include "StdAfx.h"
#include "global/global.h"
#include "global/passwd_crypt.h"
#include "net_store.h"

net_store::net_store(const char* smtp_addr, const char* pop3_addr,
	const char* user, const char* pass,
	const char* recipients, net_store_callback* callback,
	bool store /* = false */)
	: smtp_addr_(smtp_addr)
	, pop3_addr_(pop3_addr)
	, user_(user)
	, pass_(pass)
	, recipients_(recipients)
	, callback_(callback)
	, store_(store)
{

}

net_store::~net_store()
{

}

//////////////////////////////////////////////////////////////////////////
// 主线程中运行

void net_store::rpc_onover()
{
	callback_->load_db_callback(smtp_addr_.c_str(),
		pop3_addr_.c_str(), user_.c_str(),
		pass_.c_str(), recipients_.c_str(), store_);

	delete this;
}

//////////////////////////////////////////////////////////////////////////
// 子线程中运行

static const char* CREATE_TBL =
"create table mail_tbl\r\n"
"(\r\n"
"smtp_addr varchar(128) not null,\r\n"
"pop3_addr varchar(128) not null default '',\r\n"
"user varchar(128) not null default '',\r\n"
"pass varchar(128) not null default '',\r\n"
"recipients varchar(256) not null default '',\r\n"
"PRIMARY KEY(user)"
")";

void net_store::rpc_run()
{
	const char* path = global::get_instance().get_path();
	dbpath_.format("%s/net_store.db", path);

	acl::db_sqlite db(dbpath_.c_str());
	if (db.open() == false)
		logger_error("open db: %s failed", dbpath_.c_str());
	else if (create_tbl(db) == false)
		logger_error("create table failed for %s", dbpath_.c_str());
	else if (store_)
		save_db(db);
	else
		load_db(db);
}

bool net_store::create_tbl(acl::db_handle& db)
{
	if (db.tbl_exists("mail_tbl"))
	{
		logger("table exist");
		return (true);
	}
	else if (db.sql_update(CREATE_TBL) == false)
	{
		logger_error("sql(%s) error", CREATE_TBL);
		return (false);
	}
	else
	{
		save_db(db);
		logger("create table mail_tbl ok");
		return (true);
	}
}

void net_store::save_db(acl::db_handle& db)
{
	acl::string sql;

	// 先删除原来所有的记录
	sql.format("delete from mail_tbl");
	db.sql_update(sql.c_str());

	acl::string smtp_addr, pop3_addr, user, pass, recipients;
	db.escape_string(smtp_addr_.c_str(), smtp_addr_.length(), smtp_addr);
	db.escape_string(pop3_addr_.c_str(), pop3_addr_.length(), pop3_addr);
	db.escape_string(user_.c_str(), user_.length(), user);
	db.escape_string(pass_.c_str(), pass_.length(), pass);
	db.escape_string(recipients_.c_str(), recipients_.length(), recipients);

	// 密码需要加密存储
	char* pass_crypted = passwd_crypt(pass.c_str());
	sql.format("insert into mail_tbl(smtp_addr, pop3_addr, "
		"user, pass, recipients) values('%s', '%s', '%s', '%s', '%s')",
		smtp_addr.c_str(), pop3_addr.c_str(),
		user.c_str(), pass_crypted, recipients.c_str());
	acl_myfree(pass_crypted);

	const char* ptr = sql.c_str();
	ptr = pop3_addr.c_str();

	if (db.sql_update(sql.c_str()) == false)
		logger_error("sql(%s) error", sql.c_str());
}

void net_store::load_db(acl::db_handle& db)
{
	acl::string sql;
	sql.format("select smtp_addr, pop3_addr, user, pass, recipients from mail_tbl");
	if (db.sql_select(sql.c_str()) == false)
	{
		logger_error("sql(%s) error", sql.c_str());
		return;
	}
	
	const acl::db_row* first_row = db.get_first_row();
	if (first_row == NULL)
		return;
	const char* ptr = (*first_row)["smtp_addr"];
	if (ptr)
		smtp_addr_ = ptr;
	ptr = (*first_row)["pop3_addr"];
	if (ptr)
		pop3_addr_ = ptr;
	ptr = (*first_row)["user"];
	if (ptr)
		user_ = ptr;
	ptr = (*first_row)["pass"];
	if (ptr)
	{
		// 对加密的密码需要先解密
		char* pass_plain = passwd_decrypt(ptr);
		if (pass_plain)
		{
			pass_ = pass_plain;
			acl_myfree(pass_plain);
		}
	}
	ptr = (*first_row)["recipients"];
	if (ptr)
		recipients_ = ptr;
}