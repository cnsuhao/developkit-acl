#include "stdafx.h"
#include "http_servlet.h"

//////////////////////////////////////////////////////////////////////////

http_servlet::http_servlet(void)
{

}

http_servlet::~http_servlet(void)
{

}

void http_servlet::doUnknown(HttpServletRequest&, HttpServletResponse& res)
{
	res.setStatus(400);
	res.setContentType("text/html; charset=$<CHARSET>");
	// ���� http ��Ӧͷ
	if (res.sendHeader() == false)
		return;
	// ���� http ��Ӧ��
	string buf("<root error='unkown request method' />\r\n");
	(void) res.getOutputStream().write(buf);
}

bool http_servlet::doGet(HttpServletRequest& req, HttpServletResponse& res)
{
	return doPost(req, res);
}

bool http_servlet::doPost(HttpServletRequest& req, HttpServletResponse& res)
{
	const char* sid = req.getSession().getAttribute("sid");
	if (*sid == 0)
		req.getSession().setAttribute("sid", "xxxxxx");
	sid = req.getSession().getAttribute("sid");

	$<GET_COOKIES>

	// �����ַ���
	res.setContentType("text/xml; charset=$<CHARSET>");

	const char* param1 = req.getParameter("name1");
	const char* param2 = req.getParameter("name2");

	// ���� xml ��ʽ��������
	xml body;
	body.get_root()
		.add_child("root", true)
			.add_child("sessions", true)
				.add_child("session", true)
					.add_attr("sid", sid ? sid : "null")
				.get_parent()
			.get_parent()
			.add_child("cookies", true)
				.add_child("cookie", true)
					.add_attr("name1", cookie1 ? cookie1 : "null")
				.get_parent()
				.add_child("cookie", true)
					.add_attr("name2", cookie2 ? cookie2 : "null")
				.get_parent()
			.get_parent()
			.add_child("params", true)
				.add_child("param", true)
					.add_attr("name1", param1 ? param1 : "null")
				.get_parent()
				.add_child("param", true)
					.add_attr("name2", param2 ? param2 : "null");
	string buf;
	body.build_xml(buf);

	// ���� http ��Ӧͷ
	if (res.sendHeader() == false)
		return false;
	// ���� http ��Ӧ��
	if (res.getOutputStream().write(buf) == -1)
		return false;
	return true;
}

//////////////////////////////////////////////////////////////////////////

static void do_run(socket_stream* stream)
{
	memcache_session session("$<MEMCACHED_ADDR>");
	http_servlet servlet;
	servlet.setLocalCharset("$<CHARSET>");
	servlet.doRun(session, stream);
}

// ��������ʽ����ʱ�ķ�����
class master_service : public master_proc
{
public:
	master_service() {}
	~master_service() {}
protected:
	virtual void on_accept(socket_stream* stream)
	{
		do_run(stream);
	}
};

// WEB ����ģʽ
static void do_alone(void)
{
	master_service service;
	printf("listen: 0.0.0.0:8888 ...\r\n");
	service.run_alone("0.0.0.0:8888", NULL, 1);  // �������з�ʽ
}

// WEB CGI ģʽ
static void do_cgi(void)
{
	do_run(NULL);
}

int main(int argc, char* argv[])
{
#ifdef WIN32
	acl::acl_cpp_init();
#endif

	// ��ʼ����
	if (argc >= 2 && strcmp(argv[1], "alone") == 0)
		do_alone();
	else
		do_cgi();

	return 0;
}
