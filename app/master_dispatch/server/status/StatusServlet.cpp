#include "stdafx.h"
#include "server/ServerManager.h"
#include "status/StatusServlet.h"

StatusServlet::StatusServlet()
	: keep_alive_(false)
{
}

StatusServlet::~StatusServlet()
{
}

bool StatusServlet::doGet(acl::HttpServletRequest& req,
	acl::HttpServletResponse& res)
{
	return doPost(req, res);
}

bool StatusServlet::doPost(acl::HttpServletRequest& req,
	acl::HttpServletResponse& res)
{
	res.setContentType("text/json; charset=gb2312");

	// 调用单例服务器状态方法获得后端服务子进程实例的状态
	acl::string buf;
	ServerManager::get_instance().statusToString(buf);

	return res.write(buf) && req.isKeepAlive();
}
