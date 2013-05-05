#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <list>
#include "acl_cpp/http/http_type.hpp"

struct HTTP_HDR_RES;
struct HTTP_HDR_ENTRY;

namespace acl {

class string;
class HttpCookie;

/**
	* HTTP ͷ�࣬���Թ�������ͷ����Ӧͷ
	*/
class ACL_CPP_API http_header
{
public:
	http_header(void);

	/**
	 * HTTP ����ͷ���캯��
	 * @param url {const char*} ����� url��
	 * �� url ���ܰ��� ? �Լ� ? ����Ĳ������֣��������Ӹ� url �Ĳ�����
	 * Ӧ��ͨ������ add_param ��ɣ�url ��ʽ
	 * ��: http://www.test.com/, /cgi-bin/test.cgi,
	 *     http://www.test.com/cgi-bin/test.cgi
	 * ����Ϊ: http://www.test.com/cgi-bin/test.cgi?name=value ��
	 * /cgi-bin/test.cgi?name=value����Ϊ���е� name=value ����
	 * ������ add_param �����
	 */
	http_header(const char* url);

	/**
	 * HTTP ��Ӧͷ���캯��
	 * @param status {int} ״̬���磺1xx, 2xx, 3xx, 4xx, 5xx
	 */
	http_header(int status);
	virtual ~http_header(void);

	/**
	 * ���� HTTP ͷ��Ϣͬʱ���ϴε���ʱ��Դ�ͷ�
	 */
	void reset(void);

	//////////////////////////////////////////////////////////////////////////
	//            HTTP ������ HTTP ��Ӧͨ�õķ�������
	//////////////////////////////////////////////////////////////////////////

	/**
	 * ���� HTTP ͷ�ǿͻ��˵�����ͷ���Ƿ���������Ӧͷ
	 * @param onoff {bool} true ��ʾ������ͷ�������ʾ��Ӧͷ
	 * @return {http_header&} ���ر���������ã������û���������
	 */
	http_header& set_request_mode(bool onoff);

	/**
	 * �� HTTP ͷ������ֶ�
	 * @param name {const char*} �ֶ���
	 * @param value {const char*} �ֶ�ֵ
	 * @return {http_header&} ���ر���������ã������û���������
	 */
	http_header& add_entry(const char* name, const char* value);
	
	/**
	 * ���� HTTP ͷ�е� Content-Length �ֶ�
	 * @param n {int64} ����ֵ
	 * @return {http_header&} ���ر���������ã������û���������
	 */
#ifdef WIN32
	http_header& set_content_length(__int64 n);
#else
	http_header& set_content_length(long long int n);
#endif

	/**
	 * ���� HTTP ͷ�е� Content-Type �ֶ�
	 * @param value {const char*} ����ֵ
	 * @return {http_header&} ���ر���������ã������û���������
	 */
	http_header& set_content_type(const char* value);

	/**
	 * ���� HTTP ͷ�е� Connection �ֶΣ��Ƿ񱣳ֳ�����
	 * ������Ŀǰ��δ����֧�ֳ����ӣ���ʹ�����˸ñ�־λ��
	 * ��õ���Ӧ���ݺ�Ҳ�������ر�����
	 * @param on {bool} �Ƿ񱣳ֳ�����
	 * @return {http_header&} ���ر���������ã������û���������
	 */
	http_header& set_keep_alive(bool on);

	/**
	 * �� HTTP ͷ����� cookie
	 * @param name {const char*} cookie ��
	 * @param value {const char*} cookie ֵ
	 * @param domain {const char*} ������
	 * @param path {const char*} �洢·��
	 * @param expires {time_t} ����ʱ�䣬����ֵΪ 0 ʱ��ʾ�����ڣ�
	 *  > 0 ʱ����������������� expires ��Ϊ����ʱ�䣬��λΪ��
	 * @return {http_header&} ���ر���������ã������û���������
	 */
	http_header& add_cookie(const char* name, const char* value,
		const char* domain = NULL, const char* path = NULL,
		time_t expires = 0);

	/**
	 * �� HTTP ͷ����� cookie
	 * @param cookie {http_cookie*} �����Ƕ�̬����� cookie ����
	 * @return {http_header&} ���ر���������ã������û���������
	 */
	http_header& add_cookie(HttpCookie* cookie);

	/**
	 * �����͵�����ת��Ϊ rfc1123 �ַ�����ʽ������
	 */
	static void date_format(char* out, size_t size, time_t t);

	/**
	 * �ж��Ƿ��� HTTP ����ͷ
	 * @return {bool} ���� false ������ HTTP ��Ӧͷ
	 */
	bool is_request(void) const;

	//////////////////////////////////////////////////////////////////////////
	//            HTTP ���󷽷�����
	//////////////////////////////////////////////////////////////////////////
	
	/**
	 * ���� HTTP ����ͷ����
	 * @param buf {string&} �洢�������
	 * @return {bool} ��������ͷ�з�ɹ�
	 */
	bool build_request(string& buf) const;

	/**
	 * ��������� URL���� URL ���ܰ��� ? �Լ� ? ����Ĳ������֣�
	 * �������Ӹ� URL �Ĳ�����Ӧ��ͨ������ add_param ��ɣ�url ��ʽ
	 * ��: http://www.test.com/, /cgi-bin/test.cgi,
	 *     http://www.test.com/cgi-bin/test.cgi
	 * ����Ϊ: http://www.test.com/cgi-bin/test.cgi?name=value ��
	 * /cgi-bin/test.cgi?name=value����Ϊ���е� name=value ���������� add_param
	 * �����
	 * @param url {const char*} ����� url
	 * @return {http_header&} ���ر���������ã������û���������
	 */
	http_header& set_url(const char* url);

	/**
	 * ���� HTTP ����ͷ�� HOST �ֶ�
	 * @param value {const char*} ����ͷ�� HOST �ֶ�ֵ
	 * @return {http_header&} ���ر���������ã������û���������
	 */
	http_header& set_host(const char* value);

	/**
	 * ���� HTTP Э������󷽷�����������ô˺�������Ĭ���� GET ����
	 * @param method {http_method_t} HTTP ���󷽷�
	 * @return {http_header&} ���ر���������ã������û���������
	 */
	http_header& set_method(http_method_t method);

	/**
	 * ���� HTTP ����ͷ�� Range �ֶΣ����ڷֶ��������ݣ�������
	 * ֧�ֶϵ������� WEB ��������
	 * @param from {http_off_t} ��ʼƫ��λ�ã��±�� 0 ��ʼ����
	 *  ֵ�� >= 0 ʱ����Ч
	 * @param to {http_off_t} �������ƫ��λ�ã��±�� 0 ��ʼ��
	 *  ����ֵ���� < 0 ʱ������Ϊ���������ʼλ�ÿ�ʼ�����ճ���
	 *  λ��
	 * @return {http_header&} ���ر���������ã������û���������
	 */
#ifdef WIN32
	http_header& set_range(__int64 from, __int64 to);
#else
	http_header& set_range(long long int from, long long int to);
#endif

	/**
	 * ����� set_range ���õķֶ�����λ��ֵ
	 * @param from {http_off_t*} �ǿ�ʱ�洢��ʼλ��ƫ��
	 * @param to {http_off_t*} �ǿ�ʱ�洢����λ��ƫ��
	 */
#ifdef WIN32
	void get_range(__int64* from, __int64* to);
#else
	void get_range(long long int* from, long long int* to);
#endif

	/**
	 * ���� HTTP ����ͷ���Ƿ��������ѹ�����ݣ���Ӧ�� HTTP ͷ�ֶ�Ϊ��
	 * Accept-Encoding: gzip, deflate����Ŀǰ��֧�� gzip ��ʽ
	 * @param on {bool} ���Ϊ true ���Զ���� HTTP ѹ��ͷ����
	 * @return {http_header&} ���ر���������ã������û���������
	 */
	http_header& accept_gzip(bool on);

	/**
	 * ������� URL ����Ӳ�����
	 * @param name {const char*} ������
	 * @param value {const char*} ����ֵ
	 * @return {http_header&} ���ر���������ã������û���������
	 */
	http_header& add_param(const char* name, const char* value);

	/**
	 * url �ض���
	 * @param url {const char*} �ض���� URL����ʽΪ��
	 *  http://xxx.xxx.xxx/xxx �� /xxx
	 *  �����ǰ�ߣ����Զ�����ȡ�� HOST �ֶΣ�����Ǻ��ߣ���
	 *  ����֮ǰ�� HOST
	 */
	bool redirect(const char* url);

	/**
	 * �����ض�������������ֵ == 0 �����������ض��򣬷���
	 * �����ض������ض���Ĵ����ɸ�ֵ����
	 * @param n {int} �����ض���Ĵ���
	 * @return {http_header&} ���ر���������ã������û���������
	 */
	http_header& set_redirect(unsigned int n = 5);

	/**
	 * ��ȡͨ�� set_redirect ���õ����������ض������
	 * @return {unsigned int}
	 */
	unsigned int get_redirect(void) const;

	/**
	 * ����Ҫ�ض���ʱ�����������ô˺�������������һЩ���ù���
	 */
	virtual void redicrect_reset(void) {}

	//////////////////////////////////////////////////////////////////////////
	//            HTTP ��Ӧ��������
	//////////////////////////////////////////////////////////////////////////

	/**
	 * ���� HTTP ��Ӧͷ����
	 * @param buf {string&} �洢�������
	 * @return {bool} ������Ӧͷ�з�ɹ�
	 */
	bool build_response(string& buf) const;

	/**
	 * ���� HTTP ��Ӧͷ�е���Ӧ״̬��
	 * @param status {int} ״̬���磺1xx, 2xx, 3xx, 4xx, 5xx
	 * @return {http_header&} ���ر���������ã������û���������
	 */
	http_header& set_status(int status);

	/**
	 * �����Ƿ��������� CGI ��ʽ����Ӧͷ
	 * @param on {bool} �Ƿ� CGI ��ʽ��Ӧͷ
	 * @return {http_header&} ���ر���������ã������û���������
	 */
	http_header& set_cgi_mode(bool on);

protected:
private:
	//char* domain_;  // HTTP ����������
	unsigned short port_;                 // HTTP �������˿�
	char* url_;                           // HTTP ����� URL
	std::list<HTTP_PARAM*> params_;       // �����������
	std::list<HttpCookie*> cookies_;      // cookies ����
	std::list<HTTP_HDR_ENTRY*> entries_;  // HTTP ����ͷ�и��ֶμ���
	http_method_t method_;                // HTTP ����ķ���
	bool keep_alive_;                     // �Ƿ񱣳ֳ�����
	unsigned int nredirect_;              // ����ض���Ĵ�������
	bool accept_compress_;                // �Ƿ����ѹ������
	int  status_;                         // ��Ӧͷ��״̬��
	bool is_request_;                     // ������ͷ������Ӧͷ
	bool cgi_mode_;                       // �Ƿ� CGI ��Ӧͷ
#ifdef WIN32
	__int64 range_from_;                  // ����ͷ�У�range ��ʼλ��
	__int64 range_to_;                    // ����ͷ�У�range ����λ��
#else
	long long int range_from_;            // ����ͷ�У�range ��ʼλ��
	long long int range_to_;              // ����ͷ�У�range ����λ��
#endif

	void init(void);                      // ��ʼ��
	void clear(void);
	void build_common(string& buf) const; // ����ͨ��ͷ
};

}  // namespace acl end
