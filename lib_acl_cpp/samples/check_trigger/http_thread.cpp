#include "stdafx.h"
#include "util.h"
#include "http_thread.h"

http_thread::http_thread(const char* domain, const char* ip, int port,
	const char* url, double spent_dns)
: domain_(domain)
, ip_(ip)
, port_(port)
, url_(url)
, spent_dns_(spent_dns)
, spent_connect_(-1)
, spent_http_(-1)
, spent_total_(-1)
, success_(false)
{
	snprintf(addr_, sizeof(addr_), "%s:%d", ip, port);
}

http_thread::~http_thread()
{
	// 线程退出前将结果入库
	dbsave();
}

void* http_thread::run()
{
	char  domain[256];
	unsigned short port;

	if (acl::http_utils::get_addr(url_.c_str(), domain,
		sizeof(domain), &port) == false)
	{
		logger_error("invalid url: %s", url_.c_str());
		return NULL;
	}

	char  host[256], *phost;
	if (port == 80)
		phost = domain;
	else
	{
		snprintf(host, sizeof(host), "%s:%d", domain, port);
		phost = host;
	}

	struct timeval begin0;
	gettimeofday(&begin0, NULL);

	struct timeval begin, end;

	// 连接 HTTP 服务器并计算耗时
	gettimeofday(&begin, NULL);
	acl::socket_stream* conn = connect_server();
	gettimeofday(&end, NULL);
	spent_connect_ = util::stamp_sub(&end, &begin);

	if (conn == NULL)
	{
		delete this;
		return NULL;
	}

	gettimeofday(&begin, NULL);
	if (http_request(conn, phost) == false)
		logger_error("http request failed!");
	gettimeofday(&end, NULL);

	struct timeval finish;
	gettimeofday(&finish, NULL);

	spent_http_ = util::stamp_sub(&end, &begin);

	spent_total_ = spent_dns_ + util::stamp_sub(&finish, &begin0);

	delete conn;
	return NULL;
}

acl::socket_stream* http_thread::connect_server()
{
	// 连接远程 HTTP 服务器
	acl::socket_stream* conn = new acl::socket_stream();
	if (conn->open(addr_, var_cfg_conn_timeout,
		var_cfg_rw_timeout) == false)
	{
		logger_error("connect server %s error %s",
			addr_, acl::last_serror());
		return NULL;
	}

	return conn;
}

bool http_thread::http_request(acl::socket_stream* conn, const char* host)
{
	acl::http_request req(conn);
	acl::http_header& header = req.request_header();

	// 构造 HTTP 请求头
	header.set_url(url_.c_str())
		.set_keep_alive(false)
		.set_host(host)
		.set_method(acl::HTTP_METHOD_GET)
		.accept_gzip(false);

	// 发送 HTTP 请求头，同时读取响应头
	if (req.request(NULL, 0) == false)
	{
		logger_error("send request to %s error %s",
			conn->get_peer(true), acl::last_serror());
		return false;
	}

	acl::http_client* client = req.get_client();
	long long int content_length = client->body_length();
	char  buf[8192];
	int   ret, length = 0;

	// 开始读数据体
	while (true)
	{
		ret = req.get_body(buf, sizeof(buf));
		if (ret == 0)
			break;
		if (ret < 0)
		{
			logger_error("get body from %s error",
				conn->get_peer(true));
			return false;
		}
		length += ret;
	}

	// 如果读到的数据与 HTTP 响应头中的数据不一致，则报错
	if (content_length > 0 && length != content_length)
	{
		logger_error("length: %d != content_length: %lld, server: %s",
			length, content_length, conn->get_peer(true));
		return false;
	}

	logger("ok, url: %s, host: %s, addr: %s", url_.c_str(), host, addr_);
	success_ = true;
	return true;
}

void http_thread::dbsave()
{
	// 获取数据库连接
	acl::db_handle* db = var_dbpool->peek();
	if (db == NULL)
	{
		logger_error("peek db connect failed!");
		return;
	}

	if (db->open(var_cfg_dbcharset) == false)
	{
		logger_error("open db error, charset: %s",
			var_cfg_dbcharset);
		var_dbpool->put(db);
		return;
	}

	// 创建数据库表
	if (dbtbl_create(*db) == true)
		dbtbl_update(*db);  // 更新数据

	// 归还数据库连接
	var_dbpool->put(db);
}

// 数据库表结构
static const char* CREATE_TBL =
	"create table http_tbl\r\n"
	"(\r\n"
	"addr char(128) not null,\r\n"
	"url varchar(256) not null,\r\n"
	"total_spent float(10,2) not null,\r\n"
	"dns_spent float(10, 2) not null,\r\n"
	"connect_spent float(10, 2) not null,\r\n"
	"http_spent float(10, 2) not null,\r\n"
	"success char(64) not null,\r\n"
	"currtime timestamp not null\r\n"
	");"
	"create index addr_idx on http_tbl(addr);\r\n"
	"create index url_idx on http_tbl(url);\r\n";

bool http_thread::dbtbl_create(acl::db_handle& db)
{
	if (db.tbl_exists("http_tbl"))
		return true;

	// 创建数据库表
	if (db.sql_update(CREATE_TBL) == false)
	{
		logger_error("create db table failed, table: %s", CREATE_TBL);
		return false;
	}

	return true;
}

void http_thread::dbtbl_update(acl::db_handle& db)
{
	acl::string sql;
	sql.format("insert into http_tbl(addr, url, total_spent, "
		"dns_spent, connect_spent, http_spent, success, currtime) "
		"values('%s', '%s', %.2f, %.2f, %.2f, %.2f, '%s', CURRENT_TIMESTAMP)",
		addr_, url_.c_str(), spent_total_, spent_dns_,
		spent_connect_, spent_http_, success_ ? "ok" : "error");

	// 更新数据库表字段
	if (db.sql_update(sql.c_str()) == false)
		logger_error("sql(%s) error", sql.c_str());
	printf("sql: %s\r\n", sql.c_str());
}
