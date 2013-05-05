#pragma once
#include "acl_cpp/acl_cpp_define.hpp"

namespace acl {

class string;
class ostream;
class socket_stream;
class http_header;
class HttpCookie;
class HttpServlet;

/**
 * �� HTTP �ͻ�����Ӧ��ص��࣬���಻Ӧ���̳У��û�Ҳ����Ҫ
 * ����򴴽��������
 */
class ACL_CPP_API HttpServletResponse
{
public:
	/**
	 * ���캯��
	 * @param stream {socket_stream&} ���������ڲ������Զ��ر���
	 */
	HttpServletResponse(socket_stream& stream);
	~HttpServletResponse(void);

	/**
	 * ���� HTTP ��Ӧ������ĳ���
	 * @param n {acl_int64} �����峤��
	 */
#ifdef WIN32
	HttpServletResponse& setContentLength(__int64 n);
#else
	HttpServletResponse& setContentLength(long long int n);
#endif

	/**
	 * ������ HTTP �ͻ��˱�����ϵ������
	 * @param on {bool}
	 */
	HttpServletResponse& setKeepAlive(bool on);

	/**
	 * ���� HTTP ��Ӧ������� Content-Type �ֶ�ֵ�����ֶ�ֵ����Ϊ��
	 * text/html �� text/html; charset=utf8 ��ʽ
	 * @param value {const char*} �ֶ�ֵ
	 */
	HttpServletResponse& setContentType(const char* value);

	/**
	 * ���� HTTP ��Ӧ���������ַ��������Ѿ��� setContentType ����
	 * ���ַ�������Ͳ����ٵ��ñ����������ַ���
	 * @param charset {const char*} ��Ӧ�����ݵ��ַ���
	 */
	HttpServletResponse& setCharacterEncoding(const char* charset);

	/**
	 * ���� HTTP ��Ӧͷ�е����ڸ�ʽ���ֶ�
	 * @param name {const char*} HTTP ��Ӧͷ�е��ֶ���
	 * @param value {time_t} ʱ��ֵ
	 */
	HttpServletResponse& setDateHeader(const char* name, time_t value);

	/**
	 * ���� HTTP ��Ӧͷ�е��ַ�����ʽ�ֶ�
	 * @param name {const char*} HTTP ��Ӧͷ�е��ֶ���
	 * @param value {const char*} �ֶ�ֵ
	 */
	HttpServletResponse& setHeader(const char* name, const char* value);

	/**
	 * ���� HTTP ��Ӧͷ�е�������ʽ�ֶ�
	 * @param name {const char*} HTTP ��Ӧͷ�е��ֶ���
	 * @param value {int} �ֶ�ֵ
	 */
	HttpServletResponse& setHeader(const char* name, int value);

	/**
	 * ���� HTTP ��Ӧͷ�е�״̬�룺1xx, 2xx, 3xx, 4xx, 5xx
	 * @param status {int} HTTP ��Ӧ״̬��, �磺200
	 */
	HttpServletResponse& setStatus(int status);

	/**
	 * ����Ϊ CGI ģʽ���û�һ�㲻���ֹ����ã���Ϊ HttpServlet ��
	 * ���Զ������Ƿ��� CGI ģʽ
	 * @param on {bool} �Ƿ��� CGI ģʽ
	 */
	HttpServletResponse& setCgiMode(bool on);

	/**
	 * ���� HTTP ��Ӧͷ�е��ض��� location �ֶ�
	 * @param location {const char*} URL���ǿ�
	 * @param status {int} HTTP ��Ӧ״̬�룬һ��Ϊ 3xx ��
	 */
	HttpServletResponse& setRedirect(const char* location, int status = 302);

	/**
	 * ��� cookie ���󣬸ö�������Ƕ�̬����ģ����û��Լ�����
	 * ����ʾ�ͷŸö�����Ϊ�ڲ����Զ��ͷ�
	 * @param cookie {HttpCookie*}
	 */
	HttpServletResponse& addCookie(HttpCookie* cookie);

	/**
	 * ��� cookie
	 * @param name {const char*} cookie ��
	 * @param value {const char*} cookie ֵ
	 * @param domain {const char*} cookie �洢��
	 * @param path {const char*} cookie �洢·��
	 * @param expires {time_t} cookie ����ʱ����������ǰʱ���
	 *  ��ֵΪ cookie �Ĺ���ʱ���(��)
	 */
	HttpServletResponse& addCookie(const char* name, const char* value,
		const char* domain = NULL, const char* path = NULL,
		time_t expires = 0);

	/**
	 * �� url ���� url ����
	 * @param out {string&} �洢�����Ľ��
	 * @param url {const char*} δ����ǰԭʼ�� url
	 */
	void encodeUrl(string& out, const char* url);

	/**
	 * ��� HTTP ��Ӧͷ
	 * @return {http_header&}
	 */
	http_header& getHttpHeader(void) const;

	/**
	 * ���� HTTP ��Ӧͷ���û�Ӧ�÷���������ǰ���ô˺����� HTTP
	 * ��Ӧͷ���͸��ͻ���
	 */
	bool sendHeader(void);

	/**
	 * ��� HTTP ��Ӧ���������������û��ڵ��� sendHeader ����
	 * �� HTTP ��Ӧͷ��ͨ��������������� HTTP ������
	 * @return {ostream&}
	 */
	ostream& getOutputStream(void) const;
private:
	socket_stream& stream_;
	http_header* header_;
	char  charset_[32];
	char  content_type_[32];
};

}  // namespace acl
