#include "stdafx.h"
#include "access_list.h"
#include "server_manager.h"
#include "collect_client.h"
#include "message.h"
#include "message_manager.h"
#include "http_servlet.h"

http_servlet::http_servlet(const char* domain, int port)
: port_(port)
{
	if (domain && *domain)
		domain_ = domain;
}

http_servlet::~http_servlet(void)
{

}

bool http_servlet::reply(acl::HttpServletRequest& req,
	acl::HttpServletResponse& res, const char* fmt, ...)
{
	acl::string buf;

	va_list ap;
	va_start(ap, fmt);
	buf.vformat(fmt, ap);
	va_end(ap);

	res.setStatus(200)
		.setKeepAlive(req.isKeepAlive())
		.setContentLength(buf.length());

	return res.write(buf) && req.isKeepAlive();
}

bool http_servlet::reply_status(acl::HttpServletRequest& req,
	acl::HttpServletResponse& res, int status, const char* fmt, ...)
{
	acl::string buf;

	va_list ap;
	va_start(ap, fmt);
	buf.vformat(fmt, ap);
	va_end(ap);

	res.setStatus(status)
		.setKeepAlive(req.isKeepAlive())
		.setContentLength(buf.length());

	return res.write(buf) && req.isKeepAlive();
}

bool http_servlet::show_page(acl::HttpServletRequest& req,
	acl::HttpServletResponse& res, const char* fmt, ...)
{
	acl::string buf;
	va_list ap;
	va_start(ap, fmt);
	buf.vformat(fmt, ap);
	va_end(ap);

	logger_error("%s", buf.c_str());

	acl::string page_buf;
	if (acl::ifstream::load(var_cfg_index_page, &page_buf) == false)
	{
		logger_error("load %s error: %s", var_cfg_index_page,
			acl::last_serror());
		return reply(req, res, "load html page failed!");
	}

	res.setStatus(200)
		.setKeepAlive(req.isKeepAlive())
		.setContentLength(page_buf.length());
	return res.write(page_buf) && req.isKeepAlive();
}

bool http_servlet::show_login(acl::HttpServletRequest& req,
	acl::HttpServletResponse& res)
{
	acl::string page_buf;
	if (acl::ifstream::load(var_cfg_login_page, &page_buf) == false)
	{
		logger_error("load %s error: %s", var_cfg_login_page,
			acl::last_serror());
		return reply(req, res, "load html page failed!");
	}

	res.setStatus(200)
		.setKeepAlive(req.isKeepAlive())
		.setContentLength(page_buf.length());
	return res.write(page_buf) && req.isKeepAlive();
}

bool http_servlet::doUnknown(acl::HttpServletRequest&,
	acl::HttpServletResponse& res)
{
	res.setStatus(400);
	res.setContentType("text/html; charset=utf-8");
	// 发送 http 响应头
	if (res.sendHeader() == false)
		return false;
	// 发送 http 响应体
	acl::string buf("<root error='unkown request method' />\r\n");
	(void) res.getOutputStream().write(buf);
	return false;
}

bool http_servlet::doError(acl::HttpServletRequest&, acl::HttpServletResponse&)
{
	logger_error("error happend");
	return false;
}

bool http_servlet::get_servers()
{
	servers_.clear();

	// 先从配置中取得所有需要查询的地址
	const std::vector<acl::string>& addrs =
		server_manager::get_instance().get_addrs();
	std::vector<acl::string>::const_iterator cit = addrs.begin();
	for (; cit != addrs.end(); ++cit)
		servers_.push_back(*cit);

	// 再根据域名进行查询
	lookup_dns();

	if (servers_.empty())
	{
		logger_error("no result for domain: %s", domain_.c_str());
		return false;
	}
	else
		return true;
}

void http_servlet::lookup_dns()
{
	if (domain_.empty())
	{
		logger("domain null, don't need to search dns");
		return;
	}

	int  err = 0;
	ACL_DNS_DB* res = acl_gethostbyname(domain_.c_str(), &err);
	if (res == NULL)
	{
		logger_error("gethostbyname failed, domain: %s, err: %d",
			domain_.c_str(), err);
		return;
	}

	ACL_ITER iter;

	char  addr[256];
	acl_foreach(iter, res)
	{
		const ACL_HOST_INFO* info = (const ACL_HOST_INFO*) iter.data;
		acl::snprintf(addr, sizeof(addr), "%s:%d", info->ip, port_);
		servers_.push_back(addr);
	}

	acl_netdb_free(res);
}

bool http_servlet::doGet(acl::HttpServletRequest& req,
	acl::HttpServletResponse& res)
{
	return doPost(req, res);
}

bool http_servlet::doLogin(const char* user, const char* pass)
{
	acl::socket_stream conn;
	if (conn.open(var_cfg_pop_server, var_cfg_conn_timeout,
		var_cfg_rw_timeout) == false)
	{
		logger_error("connect pop server: %s error, user: %s",
			var_cfg_pop_server, user);
		return false;
	}

	acl::string line;
	if (conn.gets(line) == false)
	{
		logger_error("gets banner from pop server: %s error, user: %s",
			var_cfg_pop_server, user);
		return false;
	}

	if (line.ncompare("+OK", 3, false) != 0)
	{
		logger_error("pop server: %s, banner: %s error, user: %s",
			var_cfg_pop_server, line.c_str(), user);
		return false;
	}

	//////////////////////////////////////////////////////////////////////
	// for USER cmd
	//
	if (conn.format("USER %s\r\n", user) < 0)
	{
		logger_error("write USER cmd to pop server: %s error, user: %s",
			var_cfg_pop_server, user);
		return false;
	}

	line.clear();
	if (conn.gets(line) == false)
	{
		logger_error("gets USER's reply error, pop: %s, user: %s",
			var_cfg_pop_server, user);
		return false;
	}
	if (line.ncompare("+OK", 3, false) != 0)
	{
		logger_error("USER's reply: %s error, pop: %s, user: %s",
			line.c_str(), var_cfg_pop_server, user);
		return false;
	}

	//////////////////////////////////////////////////////////////////////
	// for PASS cmd
	//
	if (conn.format("PASS %s\r\n", pass) < 0)
	{
		logger_error("write PASS cmd error, pop: %s, user: %s",
			var_cfg_pop_server, user);
		return false;
	}
	line.clear();
	if (conn.gets(line) == false)
	{
		logger_error("gets PASS's reply error, pop: %s, user: %s",
			var_cfg_pop_server, user);
		return false;
	}
	if (line.ncompare("+OK", 3, false) != 0)
	{
		logger_error("PASS's reply: %s error, pop: %s, user: %s",
			line.c_str(), var_cfg_pop_server, user);
		return false;
	}

	return true;
}

bool http_servlet::doPost(acl::HttpServletRequest& req,
	acl::HttpServletResponse& res)
{
	bool keep_alive = req.isKeepAlive();
	res.setContentType("text/html; charset=utf-8")	// 设置响应字符集
		.setKeepAlive(keep_alive);		// 设置是否保持长连接

	const char* path = req.getPathInfo();
	if (path == NULL || *path == 0)
	{
		logger_error("getPathInfo null");
		return show_login(req, res);
	}

	if (strcasecmp(path, var_cfg_path_info) != 0
		&& strcasecmp(path, "/") != 0)
	{
		logger_error("invalid path: %s", path);
		return reply_status(req, res, 404, "not found: %s", path);
	}

	const char* user = req.getSession().getAttribute(var_cfg_session_key);
	if (user && *user)
	{
		if (access_list::get_instance().check(user) == false)
		{
			(void) reply(req, res, "Permission denied!");
			logger_warn("user: %s was denied!", user);
			return false;
		}

		logger("user: %s session login, sid: %s", user,
			req.getSession().getSid());
		return doAction(req, res);
	}
	else
	{
		const char* ptr = req.getCookieValue("ACL_SESSION_ID");
		logger("no session for user, ACL_SESSION_ID cookie: %s",
			ptr ? ptr : "null");
	}

	user = req.getParameter("user");
	if (user == NULL || *user == 0)
		return show_login(req, res);

	const char* pass = req.getParameter("pass");
	if (pass == NULL || *pass == 0)
		return show_login(req, res);

	if (access_list::get_instance().check(user) == false)
	{
		(void) reply(req, res, "Permission denied!");
		logger_warn("user: %s was denied!", user);
		return false;
	}

	if (doLogin(user, pass) == false)
	{
		logger_error("login error, user: %s, pass: %s", user, pass);
		return show_login(req, res);
	}

	logger("user: %s pop login", user);

	req.getSession().setMaxAge(var_cfg_session_ttl);

	if (req.getSession().setAttribute(var_cfg_session_key, user) == false)
	{
		logger_error("set session failed, user: %s, memcache: %s",
			user, var_cfg_memcache_addr);
		return show_login(req, res);
	}
	else
		logger("set session ok, user: %s, memcache: %s, sid: %s",
			user, var_cfg_memcache_addr, req.getSession().getSid());

	return doAction(req, res);
}

bool http_servlet::doAction(acl::HttpServletRequest& req,
	acl::HttpServletResponse& res)
{
	// 设置块传输方式，则给浏览器以块传输方式回复数据
	res.setChunkedTransferEncoding(true);

	// 启动多线程，向所有日志查询服务器查询日志内容

	if (get_servers() == false)
		return reply(req, res, "get all servers's addrs failed!");

	// 先写 HTML 页面开始部分
	int ret = res.format("<html><head><meta http-equiv=\"Content-Type\" "
		"content=\"text/html; charset=%s\" /></head><body>",
		var_cfg_html_charset);
	if (ret < 0)
	{
		logger_error("write html head failed!");
		return false;
	}

	std::vector<collect_client*> collecters;
	message_manager* manager = new message_manager;
	std::vector<acl::string>::const_iterator cit = servers_.begin();
	int  nthreads = 0;
	for (; cit != servers_.end(); ++cit)
	{
		collect_client* collect = new collect_client(*manager,
			(*cit).c_str());
		collect->set_detachable(false);
		collecters.push_back(collect);
		nthreads++;
		collect->start();
	}

	// 异步等待所有查询线程的结果数据
	int  n = 0, len = 0;
	bool ok = true;

	while (nthreads > 0)
	{
		message* msg = manager->pop();
		if (msg == NULL)
		{
			logger_error("pop message failed!");
			break;
		}
		n++;

		if (ok)
		{
			ret = reply(req, res, *msg);
			if (ret < 0)
				ok = false;
			else
				len += ret;
		}

		// 删除动态分配的日志消息对象
		delete msg;

		// 当结果数量与所启动的线程数量相等时，说明查询完毕
		if (n == nthreads)
		{
			logger("All threads over");
			break;
		}
	}

	std::vector<collect_client*>::iterator it = collecters.begin();
	for (; it != collecters.end(); ++it)
	{
		(*it)->wait();
		delete (*it);
	}
	collecters.clear();

	delete manager;

	if (len == 0 && res.write("empty result!") == false)
	{
		logger_error("write error");
		return false;
	}

	ret = res.write("</body></html>");
	if (ret < 0)
	{
		logger_error("write html end failed");
		return false;
	}

	if (res.write(NULL, 0) == false)
	{
		logger_error("write chunked end failed");
		return false;
	}

	return ok && req.isKeepAlive();
}

int http_servlet::reply(acl::HttpServletRequest&, acl::HttpServletResponse& res,
	message& msg)
{
	const acl::string& server = msg.get_server();
	const std::vector<acl::string*>& lines = msg.get_result();

	if (lines.empty())
		return 0;

	size_t n = 0;

	int ret = res.format("<hr><br>\r\n<b>server: %s</b><br>\r\n",
			server.c_str());

	if (ret < 0)
	{
		logger_error("write error, server: %s", server.c_str());
		return -1;
	}
	n += ret;

	std::vector<acl::string*>::const_iterator cit = lines.begin();
	for (int i = 0; cit != lines.end(); ++cit, i++)
	{
		acl::string* line = *cit;

		if (!line->empty() && res.write(*line) == false)
		{
			logger_error("write error, server: %s", server.c_str());
			return -1;
		}
		n += line->length();
	}

	ret = res.format("<hr><br>\r\n");
	if (ret < 0)
	{
		logger_error("write error, server: %s", server.c_str());
		return -1;
	}
	n += ret;

	return n;
}
