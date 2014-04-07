#include "stdafx.h"
#include "http_servlet.h"

http_servlet::http_servlet(void)
{

}

http_servlet::~http_servlet(void)
{

}

bool http_servlet::doUnknown(acl::HttpServletRequest&,
	acl::HttpServletResponse& res)
{
	res.setStatus(400);
	res.setContentType("text/html; charset=$<CHARSET>");
	// ���� http ��Ӧͷ
	if (res.sendHeader() == false)
		return false;
	// ���� http ��Ӧ��
	acl::string buf("<root error='unkown request method' />\r\n");
	(void) res.getOutputStream().write(buf);
	return false;
}

bool http_servlet::doGet(acl::HttpServletRequest& req,
	acl::HttpServletResponse& res)
{
	return doPost(req, res);
}

bool http_servlet::doPost(acl::HttpServletRequest& req,
	acl::HttpServletResponse& res)
{
	const char* sid = req.getSession().getAttribute("sid");
	if (*sid == 0)
		req.getSession().setAttribute("sid", "xxxxxx");
	sid = req.getSession().getAttribute("sid");

	/*
	$<GET_COOKIES>
	*/

	// �����ַ���
	res.setContentType("text/xml; charset=utf-8");

	const char* param1 = req.getParameter("name1");
	const char* param2 = req.getParameter("name2");

	// ���� xml ��ʽ��������
	acl::xml body;
	body.get_root()
		.add_child("root", true)
			.add_child("sessions", true)
				.add_child("session", true)
					.add_attr("sid", sid ? sid : "null")
				.get_parent()
			.get_parent()
			/*
			.add_child("cookies", true)
				.add_child("cookie", true)
					.add_attr("name1", cookie1 ? cookie1 : "null")
				.get_parent()
			.get_parent()
				*/
			.add_child("params", true)
				.add_child("param", true)
					.add_attr("name1", param1 ? param1 : "null")
				.get_parent()
				.add_child("param", true)
					.add_attr("name2", param2 ? param2 : "null");
	acl::string buf;
	body.build_xml(buf);

	// ���� http ��Ӧͷ
	if (res.sendHeader() == false)
		return false;
	// ���� http ��Ӧ��
	if (res.getOutputStream().write(buf) == -1)
		return false;
	return true;
}
