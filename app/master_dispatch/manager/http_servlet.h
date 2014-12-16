#pragma once

class message;

class http_servlet : public acl::HttpServlet
{
public:
	http_servlet(const char* domain, int port);
	~http_servlet();

protected:
	virtual bool doUnknown(acl::HttpServletRequest&,
		acl::HttpServletResponse& res);
	virtual bool doGet(acl::HttpServletRequest& req,
		acl::HttpServletResponse& res);
	virtual bool doPost(acl::HttpServletRequest& req,
		acl::HttpServletResponse& res);
	virtual bool doError(acl::HttpServletRequest& req,
		acl::HttpServletResponse&);

private:
	acl::string domain_;
	int port_;
	std::vector<acl::string> servers_;

	bool doLogin(const char* user, const char* pass);
	bool doAction(acl::HttpServletRequest& req,
		acl::HttpServletResponse& res);
	bool get_servers();
	void lookup_dns();
	bool show_page(acl::HttpServletRequest& req,
		acl::HttpServletResponse& res, const char* fmt, ...);
	bool show_login(acl::HttpServletRequest& req,
		acl::HttpServletResponse& res);
	bool reply(acl::HttpServletRequest& req,
		acl::HttpServletResponse& res, const char* fmt, ...);
	bool reply_status(acl::HttpServletRequest& req,
		acl::HttpServletResponse& res, int status, const char* fmt, ...);
	int reply(acl::HttpServletRequest& req, acl::HttpServletResponse& res,
		message& msg);
};
