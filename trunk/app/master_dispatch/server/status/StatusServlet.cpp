#include "stdafx.h"
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
	(void) req; (void) res;
	return false;
}
