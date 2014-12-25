#include "stdafx.h"
#include "status_manager.h"
#include "status_servlet.h"

status_servlet::status_servlet()
{
}

status_servlet::~status_servlet()
{

}

bool status_servlet::reply(acl::HttpServletRequest& req,
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

bool status_servlet::reply_status(acl::HttpServletRequest& req,
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

bool status_servlet::doUnknown(acl::HttpServletRequest&,
	acl::HttpServletResponse& res)
{
	res.setStatus(400);
	res.setContentType("text/html; charset=utf-8");
	// ���� http ��Ӧͷ
	if (res.sendHeader() == false)
		return false;
	// ���� http ��Ӧ��
	acl::string buf("<root error='unkown request method' />\r\n");
	(void) res.getOutputStream().write(buf);
	return false;
}

bool status_servlet::doError(acl::HttpServletRequest&, acl::HttpServletResponse&)
{
	logger_error("error happend");
	return false;
}

bool status_servlet::doGet(acl::HttpServletRequest& req,
	acl::HttpServletResponse& res)
{
	return doPost(req, res);
}

bool status_servlet::doPost(acl::HttpServletRequest& req,
	acl::HttpServletResponse& res)
{
	bool keep_alive = req.isKeepAlive();
	res.setContentType("text/html; charset=utf-8")	// ������Ӧ�ַ���
		.setKeepAlive(keep_alive);		// �����Ƿ񱣳ֳ�����

	const char* ctype = req.getContentType(false);
	if (ctype == NULL)
	{
		logger_error("getContentType null");
		return false;
	}

	if (strncasecmp(ctype, "text/json", sizeof("text/json") - 1) == 0)
		return doJson(req, res) && keep_alive;
	else if (strncasecmp(ctype, "text/xml", sizeof("text/xml") - 1) == 0)
		return doXml(req, res) && keep_alive;
	else
	{
		logger_error("unknown content-type: %s", ctype);
		return false;
	}
}

bool status_servlet::doJson(acl::HttpServletRequest& req,
	acl::HttpServletResponse& res)
{
	long long int length = req.getContentLength();
	if (length <= 0)
	{
		logger_error("length: %lld invalid", length);
		return false;
	}

	acl::istream& in = req.getInputStream();
	char  buf[8192];
	int   ret;
	size_t n;
	acl::json json;

	while (length > 0)
	{
		n = length > (long long int) sizeof(buf) - 1
			? (long long int) sizeof(buf) -1 : length;
		ret = in.read(buf, n, false);
		if (ret == -1)
		{
			logger_error("read json body error");
			return false;
		}

		buf[ret] = 0;
		length -= ret;
		json.update(buf);
	}

	/*
	 * ���ݸ�ʽ:
	 * {
	 *     server: [
	 *         {
	 *             conns: xxx,
	 *             used: xxx, 
	 *             pid: xxx,
	 *             max_threads: xxx,
	 *             curr_threads: xxx,
	 *             busy_threads: xxx,
	 *             qlen: xxx,
	 *             type: "xxx"
	 *         },
	 *         {
	 *         }
	 *         ...
	 *     ],
	 *     conns: xxx,
	 *     used: xxx,
	 *     qlen: xxx,
	 *     max_threads: xxx,
	 *     curr_threads: xxx,
	 *     busy_threads: xxx,
	 *     addr: "xxxx"
	 *     load: xxx
	 * }
	 */

	// �� JSON ����תΪ XML ���ݵĹ���

	acl::xml xml;
	acl::xml_node& server = xml.create_node("server");
	xml.get_root().add_child(server);

	acl::string key;

	acl::json_node& root = json.get_root();
	acl::json_node* child = root.first_child();

	while (child != NULL)
	{
		const char* name = child->tag_name();
		const char* value = child->get_text();
		if (name != NULL && value != NULL)
		{
			if (strcasecmp(name, "addr") == 0)
				key = value;
			server.add_child(name, false, value);
		}

		child = root.next_child();
	}

	if (key.empty())
	{
		logger_error("no addr in: %s", json.to_string().c_str());
		(void) reply_status(req, res, 400, "no addr node");
		return false;
	}

	acl::string data;
	xml.build_xml(data);
	printf(">>>>>>>%s\n", data.c_str());

	// ���������ӽ�״̬��������
	status_manager::get_instance().set_status(key.c_str(), data.c_str());

	return reply(req, res, "ok!");
}

bool status_servlet::doXml(acl::HttpServletRequest& req,
	acl::HttpServletResponse& res)
{
	long long int length = req.getContentLength();
	if (length <= 0)
	{
		logger_error("length: %lld invalid", length);
		return false;
	}

	acl::istream& in = req.getInputStream();
	char  buf[8192];
	int   ret;
	size_t n;

	acl::string data(length);

	while (length > 0)
	{
		n = length > (long long int) sizeof(buf)
			? (long long int) sizeof(buf) : length;
		ret = in.read(buf, n, false);
		if (ret == -1)
		{
			logger_error("read json body error");
			return false;
		}
		length -= ret;
		data.append(buf, ret);
	}

	/**
	 * ���ݸ�ʽ��
	 * <server connx='xxx' used='xxx' qlen='xxx' max_threads='xxx'
	 *  curr_threads='xxx' busy_threads='xxx' addr='xxx' load='xxx'>
	 *   <proc>
	 *     <conns>xxx</conns>
	 *     <used>xxx</used>
	 *     <pid>xxx</pid>
	 *     <max_threads>xxx</max_threads>
	 *     <curr_threads>xxx</curr_threads>
	 *     <busy_threads>xxx</busy_threads>
	 *     <qlen>xxx</qlen>
	 *     <type>xxx</type>
	 *   </proc>
	 * </server>
	 */

	acl::xml xml(data.c_str());
	acl::xml_node& root = xml.get_root();
	acl::xml_node* child = root.first_child();
	acl::xml_node* server = NULL;

	// ���� server ���
	while (child != NULL)
	{
		const char* name = child->tag_name();
		if (name && strcasecmp(name, "server") == 0)
		{
			server = child;
			break;
		}

		child = root.next_child();
	}

	if (server == NULL)
	{
		logger_error("no server in: %s", data.c_str());
		(void) reply_status(req, res, 400, "no server node");
		return false;
	}

	acl::string key;

	const acl::xml_attr* attr = server->first_attr();
	while (attr)
	{
		const char* name = attr->get_name();
		const char* value = attr->get_value();
		if (name && value && strcasecmp(name, "addr") == 0)
		{
			key = value;
			break;
		}
		attr = server->next_attr();
	}

	if (key.empty())
	{
		logger_error("no addr in: %s", data.c_str());
		(void) reply_status(req, res, 400, "no addr node");
		return false;
	}

	// ���������ӽ�״̬��������
	status_manager::get_instance().set_status(key.c_str(), data.c_str());

	return reply(req, res, "ok!");
}