#include "stdafx.h"
#include "http_job.h"
#include "master_service.h"

////////////////////////////////////////////////////////////////////////////////
// 配置内容项

char* var_cfg_http_url;
char* var_cfg_dns_ip;
char* var_cfg_dbpath;
char* var_cfg_dbcharset;
acl::master_str_tbl var_conf_str_tab[] = {
	{ "http_url", "http://www.baidu.com/", &var_cfg_http_url },
	{ "dns_ip", "8.8.8.8", &var_cfg_dns_ip },
	{ "dbpath", "./check.db", &var_cfg_dbpath },
	{ "dbcharset", "utf-8", &var_cfg_dbcharset },

	{ 0, 0, 0 }
};

acl::master_bool_tbl var_conf_bool_tab[] = {

	{ 0, 0, 0 }
};

int   var_cfg_http_cocurrent;
int   var_cfg_dns_port;
int   var_cfg_conn_timeout;
int   var_cfg_rw_timeout;
acl::master_int_tbl var_conf_int_tab[] = {
	{ "http_cocurrent", 10, &var_cfg_http_cocurrent, 0, 0 },
	{ "dns_port", 53, &var_cfg_dns_port, 0, 0 },
	{ "conn_timeout", 60, &var_cfg_conn_timeout, 0, 0 },
	{ "rw_timeout", 60, &var_cfg_rw_timeout, 0, 0 },

	{ 0, 0 , 0 , 0, 0 }
};

acl::master_int64_tbl var_conf_int64_tab[] = {

	{ 0, 0 , 0 , 0, 0 }
};

////////////////////////////////////////////////////////////////////////////////

static acl::thread_pool* __thrpool;
acl::db_pool* var_dbpool;

master_service::master_service()
{
}

master_service::~master_service()
{
}

void master_service::on_trigger()
{
	for (int i = 0; i < var_cfg_http_cocurrent; i++)
	{
		// 创建一个工作线程任务，并交由线程池去执行
		acl::thread_job* job = new http_job(*__thrpool, var_cfg_http_url,
			var_cfg_dns_ip, var_cfg_dns_port);
		__thrpool->execute(job);
	}
}

void master_service::proc_on_init()
{
	// 创建数据库连接池
	var_dbpool = new acl::sqlite_pool(var_cfg_dbpath,
		var_cfg_http_cocurrent * 10);

	// 创建线程池并设置参数
	__thrpool = new acl::thread_pool;
	__thrpool->set_limit(var_cfg_http_cocurrent);
	__thrpool->set_idle(300);

	// 启动线程池过程
	__thrpool->start();
}

void master_service::proc_on_exit()
{
	// 销毁线程池
	__thrpool->stop();
	delete __thrpool;

	// 销毁数据库连接池
	delete var_dbpool;
}
