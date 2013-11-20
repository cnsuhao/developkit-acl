#include "stdafx.h"
#include "logger_servlet.h"

logger_servlet::logger_servlet()
{

}

logger_servlet::~logger_servlet()
{

}

bool logger_servlet::doUnknown(acl::HttpServletRequest&,
	acl::HttpServletResponse&)
{
	return false;
}

bool logger_servlet::doGet(acl::HttpServletRequest& req,
	acl::HttpServletResponse& res)
{
	return doPost(req, res);
}

bool logger_servlet::doPost(acl::HttpServletRequest& req,
	acl::HttpServletResponse&)
{
	const char* uri = req.getRequestUri();
	if (uri == NULL || *uri == 0)
	{
		logger_debug(DEBUG_HTTP, 1, "getRequestUri null");
		return false;
	}

	const char* host = req.getRemoteHost();
	if (host == NULL || *host == 0)
	{
		logger_debug(DEBUG_HTTP, 1, "no Host");
		return false;
	}

	acl::string url;
	url.format("http://%s/%s", host, uri);

	return true;
}
