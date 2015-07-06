#pragma once

class http_servlet : public acl::HttpServlet
{
public:
	http_servlet(acl::ostream& out, acl::polarssl_conf* conf);
	~http_servlet();

protected:
	virtual bool doGet(acl::HttpServletRequest& req,
		acl::HttpServletResponse& res);
	virtual bool doPost(acl::HttpServletRequest& req,
		acl::HttpServletResponse& res);
	virtual bool doUnknown(acl::HttpServletRequest&,
		acl::HttpServletResponse& res);
	virtual bool doConnect(acl::HttpServletRequest& req,
		acl::HttpServletResponse& res);
	virtual bool doPut(acl::HttpServletRequest& req,
		acl::HttpServletResponse& res);
	virtual bool doDelete(acl::HttpServletRequest& req,
		acl::HttpServletResponse& res);
	virtual bool doHead(acl::HttpServletRequest& req,
		acl::HttpServletResponse& res);
	virtual bool doOption(acl::HttpServletRequest& req,
		acl::HttpServletResponse& res);

private:
	acl::string url_;
	acl::ostream& out_;
	acl::polarssl_conf* client_ssl_conf_;

	void logger_request(acl::HttpServletRequest& req);
};
