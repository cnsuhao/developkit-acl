#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/http/http_header.hpp"

namespace acl {

class http_client;
class http_pipe;
class socket_stream;
class xml;
class json;

class ACL_CPP_API http_response
{
public:
	/**
	 * ���캯����ͨ���ù��캯������� socket_stream ������
	 * ���ᱻ�رգ���Ҫ�������Լ��ر�
	 * @param client {socket_stream*} �������������ǿ�
	 * ע������ʵ���ڳ�����ʱ���Ա����ʹ�ã���һ����ע��ʹ��
	 * ˳��get_body->response
	 */
	http_response(socket_stream* client);
	virtual ~http_response(void);

	//////////////////////////////////////////////////////////////////////////
	// ���ȡ����������صķ���

	/**
	 * ��ȡ HTTP ����ͻ��˵� HTTP ����ͷ���ڵ��ñ�������ſ��Ե��� get_body
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool read_header();

	/**
	 * ��ȡ xml ��ʽ�� HTTP ���������壬������˺�����Ӧ�õ���
	 * response ���ͻ��˷�����Ӧ���ݣ��ú���ÿ�α�����ʱ���ڲ���
	 * ����ᱻ��ʼ�������Ըú��������ڶ���Ự�б���ε���
	 * @param out {xml&} �� HTTP ���������ݴ洢�ڸ� xml ������
	 * @param to_charset {const char*} ������ǿգ��ڲ��Զ�
	 *  ������ת�ɸ��ַ����洢�� xml ������
	 * @return {bool} �Ƿ�����
	 *  ע���������ȵ��� read_header ����ܵ��ñ�����
	 *      �������������ر��ʱ��Ӧ�ô˺����������ڴ�Ĺ�
	 */
	bool get_body(xml& out, const char* to_charset = NULL);

	/**
	 * ��ȡ json ��ʽ�� HTTP ���������壬������˺�����Ӧ�õ���
	 * response ���ͻ��˷�����Ӧ���ݣ��ú���ÿ�α�����ʱ���ڲ���
	 * ����ᱻ��ʼ�������Ըú��������ڶ���Ự�б���ε���
	 * @param out {json&} �� HTTP ���������ݴ洢�ڸ� json ������
	 * @param to_charset {const char*} ������ǿգ��ڲ��Զ�
	 *  ������ת�ɸ��ַ����洢�� json ������
	 * @return {bool} �Ƿ�����
	 *  ע���������ȵ��� read_header ����ܵ��ñ�����
	 *      �������������ر��ʱ��Ӧ�ô˺����������ڴ�Ĺ�
	 */
	bool get_body(json& out, const char* to_charset = NULL);

	/*
	 * ��ȡ HTTP ȫ�����������ݲ��洢������Ļ�������
	 * @param out {string&} �洢����������
	 * @param to_charset {const char*} ������ǿգ��ڲ��Զ�
	 *  ������ת�ɸ��ַ����洢�� out ������
	 * ע���������������ر��ʱ��Ӧ�ô˺����������ڴ�Ĺ�
	 *     �������ȵ��� read_header ����ܵ��ñ�����
	 */
	bool get_body(string& out, const char* to_charset = NULL);

	/*
	 * ��ȡ HTTP ���������ݲ��洢������Ļ������У�����ѭ��
	 * ���ñ�������ֱ�����ݶ����ˣ�
	 * @param buf {char*} �洢��������������
	 * @param size {size_t} buf ��������С
	 * @return {int} ����ֵ == 0 ��ʾ��������ϣ�< 0 ��ʾ�ͻ���
	 *  �ر����ӣ�> 0 ��ʾ�Ѿ����������ݣ��û�Ӧ��һֱ������ֱ��
	 *  ����ֵ <= 0 Ϊֹ
	 *  ע���ú�����������ԭʼ HTTP ���������ݣ�������ѹ���ַ���
	 *      ���룬�û��Լ�������Ҫ���д���
	 *      �������ȵ��� read_header ����ܵ��ñ�����
	 */
	int get_body(char* buf, size_t size);

	//////////////////////////////////////////////////////////////////////////
	// ��������Ӧ��صķ�������

	/**
	 * ��� HTTP ��Ӧͷ����Ȼ���ڷ��ص� HTTP ��Ӧͷ���������
	 * �Լ�����Ӧͷ�ֶλ� http_header::reset()������Ӧͷ״̬��
	 * �ο���http_header ��
	 * @return {http_header&}
	 */
	http_header& response_header(void);

	/**
	 * �� HTTP �ͻ��˷�����Ӧ���ݣ��ڵ��ñ�����ǰ�������ȵ���
	 * get_body ��� HTTP �ͻ��˵���������
	 * @param body {const char*} �������ַ
	 * @param len {size_t} body �ǿ�ʱ���� body ���ݳ���
	 * @param status {int} HTTP ��Ӧ�룺1xx, 2xx, 3xx, 4xx, 5xx
	 * @param keep_alive {bool} �Ƿ���ͻ��˱��ֳ�����
	 * @return {bool} �����Ƿ�ɹ�
	 */
	bool response(const char* body, size_t len,
		int status = 200, bool keep_alive = true);

	//////////////////////////////////////////////////////////////////////////

	/**
	 * ��� http_client HTTP ������������ͨ�����صĶ�����
	 * �ͻ�������ͷ�Ĳ������ݣ��ο���http_client ��
	 * @return {http_client*} �����ؿ�ʱ��ʾ��������
	 */
	http_client* get_client(void) const;

	/**
	 * �ر� HTTP ������
	 */
	void close(void);
protected:
private:
	bool debug_;
	bool header_ok_;
	http_client* client_;
	http_header  header_;
	http_pipe* get_pipe(const char* to_charset);
};

} // namespace acl
