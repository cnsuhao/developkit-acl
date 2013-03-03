#include "StdAfx.h"
#include "global/global.h"
#include "nslookup.h"
#include "dns_store.h"

dns_store::dns_store(std::vector<domain_info*>* domain_list,
	nslookup_callback& callback)
: domain_list_(domain_list)
, callback_(callback)
, ok_(false)
{

}

dns_store::~dns_store()
{
	delete domain_list_;
}

//////////////////////////////////////////////////////////////////////////
// ���̹߳���

void dns_store::rpc_onover()
{
	logger("store domain lookup results %s!", ok_ ? "OK" : "Failed");
	callback_.enable_nslookup(dbpath_.empty() ? NULL : dbpath_.c_str());
	delete this;
}

//////////////////////////////////////////////////////////////////////////
// ���̹߳���

const char* CREATE_TBL =
"create table dns_tbl\r\n"
"(\r\n"
"domain varchar(128) not null,\r\n"
"ip varchar(32) not null default '',\r\n"
"ttl integer not null default 0,\r\n"
"spent integer not null default 0,\r\n"
"primary key(domain, ip)\r\n"
")";

void dns_store::rpc_run()
{
	const char* path = global::get_instance().get_path();
	dbpath_.format("%s/dns_store_%ld.db", path, time(NULL));

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

bool dns_store::create_tbl(acl::db_handle& db)
{
	if (db.tbl_exists("dns_tbl"))
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
		logger("create table dns_tbl ok");
		return (true);
	}
}

void dns_store::insert_tbl(acl::db_handle& db)
{
	std::vector<domain_info*>::const_iterator cit = domain_list_->begin();
	for (; cit != domain_list_->end(); ++cit)
		insert_one(db, *cit);
}

void dns_store::insert_one(acl::db_handle& db, const domain_info* info)
{
	const std::vector<IP_INFO*>& ip_list = info->get_ip_list();
	std::vector<IP_INFO*>::const_iterator cit = ip_list.begin();
	acl::string sql;
	for (; cit != ip_list.end(); ++cit)
	{
		sql.format("insert into dns_tbl(domain, ip, ttl, spent)"
			" values('%s', '%s', '%d', '%d')",
			info->get_domain(), (*cit)->ip, (*cit)->ttl,
			(int) (info->end_time() - info->begin_time()));
		if (db.sql_update(sql.c_str()) == false)
			logger_error("sql(%s) error", sql.c_str());
		else
			logger("add ok, domain: %s, ip: %s, ttl: %d, spent: %d",
				info->get_domain(), (*cit)->ip, (*cit)->ttl,
				(int)(info->end_time() - info->begin_time()));
	}
}