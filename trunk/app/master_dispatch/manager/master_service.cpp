#include "stdafx.h"
#include "access_list.h"
#include "http_servlet.h"
#include "server_manager.h"
#include "master_service.h"

//////////////////////////////////////////////////////////////////////////////
// ����������

char *var_cfg_servers;
char *var_cfg_index_page;
char *var_cfg_login_page;
char *var_cfg_server_domain;
char *var_cfg_html_charset;
char *var_cfg_pop_server;
char *var_cfg_allow_users;
char *var_cfg_memcache_addr;
char *var_cfg_session_key;
char *var_cfg_path_info;
acl::master_str_tbl var_conf_str_tab[] = {
	{ "server_list", "", &var_cfg_servers },
	{ "index_page", "index.htm", &var_cfg_index_page },
	{ "login_page", "login.htm", &var_cfg_login_page },
	{ "server_domain", "", &var_cfg_server_domain },
	{ "html_charset", "utf-8", &var_cfg_html_charset },
	{ "pop_server", "pop.263.net:110", &var_cfg_pop_server },
	{ "allow_users", "all", &var_cfg_allow_users },
	{ "memcache_addr", "127.0.0.1:11211", &var_cfg_memcache_addr },
	{ "session_key", "dispatch_manager_id", &var_cfg_session_key },
	{ "path_info", "/dispatch_collect", &var_cfg_path_info },

	{ 0, 0, 0 }
};

int   var_cfg_auth_enable;
acl::master_bool_tbl var_conf_bool_tab[] = {
	{ "auth_enable", 0, &var_cfg_auth_enable },

	{ 0, 0, 0 }
};

int   var_cfg_conn_timeout;
int   var_cfg_rw_timeout;
int   var_cfg_dns_ttl;
int   var_cfg_server_port;
int   var_cfg_session_ttl;
acl::master_int_tbl var_conf_int_tab[] = {
	{ "conn_timeout", 30, &var_cfg_conn_timeout, 0, 0 },
	{ "rw_timeout", 300, &var_cfg_rw_timeout, 0, 0 },
	{ "dns_ttl", 30, &var_cfg_dns_ttl, 0, 0 },
	{ "server_port", 10081, &var_cfg_server_port, 0, 0 },
	{ "session_ttl", 3600, &var_cfg_session_ttl, 0, 0 },

	{ 0, 0 , 0 , 0, 0 }
};

acl::master_int64_tbl var_conf_int64_tab[] = {

	{ 0, 0 , 0 , 0, 0 }
};

//////////////////////////////////////////////////////////////////////////

master_service::master_service()
{
}

master_service::~master_service()
{
}

#define	STATIC_SERVLET

bool master_service::thread_on_read(acl::socket_stream* conn)
{
#ifdef	STATIC_SERVLET
	http_servlet servlet(var_cfg_server_domain, var_cfg_server_port);
	return servlet.doRun(var_cfg_memcache_addr, conn);
#else
	http_servlet* servlet = (http_servlet*) conn->get_ctx();
	if (servlet == NULL)
		logger_fatal("servlet null!");

	return servlet->doRun(var_cfg_memcache_addr, conn);
#endif
}

bool master_service::thread_on_accept(acl::socket_stream* conn)
{
	logger_debug(DEBUG_CONN, 2, "connect from %s, fd: %d",
		conn->get_peer(true), conn->sock_handle());

	conn->set_rw_timeout(5);

#ifndef	STATIC_SERVLET
	http_servlet* servlet = new http_servlet(var_cfg_server_domain,
			var_cfg_server_port);
	conn->set_ctx(servlet);
#endif

	return true;
}

bool master_service::thread_on_timeout(acl::socket_stream* conn)
{
	logger_debug(DEBUG_CONN, 2, "read timeout from %s, fd: %d",
		conn->get_peer(), conn->sock_handle());

	return false;
}

void master_service::thread_on_close(acl::socket_stream* conn)
{
	logger_debug(DEBUG_CONN, 2, "disconnect from %s, fd: %d",
		conn->get_peer(), conn->sock_handle());

	http_servlet* servlet = (http_servlet*) conn->get_ctx();
	delete servlet;
}

void master_service::thread_on_init()
{
}

void master_service::thread_on_exit()
{
}

void master_service::proc_on_init()
{
	// ��ʹ�������ļ��еķ�������
	if (server_manager::get_instance().init(var_cfg_servers) == false)
		logger("Add static server: none!");

	// ��ʼ�� DNS ��ѯ���� acl ���еĻ���ʱ��
	acl_netdb_cache_init(var_cfg_dns_ttl, 1);

	// ��ʼ���û����ʰ�����
	access_list::get_instance().init(var_cfg_allow_users);
}

void master_service::proc_on_exit()
{
}