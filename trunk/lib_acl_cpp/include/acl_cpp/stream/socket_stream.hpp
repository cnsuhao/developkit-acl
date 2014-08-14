#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#ifdef WIN32
#include <WinSock2.h>
#endif
#include "acl_cpp/stream/istream.hpp"
#include "acl_cpp/stream/ostream.hpp"

struct ACL_VSTREAM;

namespace acl {

class ACL_CPP_API socket_stream
	: public istream
	, public ostream
{
public:
	socket_stream();
	virtual ~socket_stream();

	/**
	 * �����׽��ִ򿪵�һ��������
	 * @param fd �׽���
	 * @return {bool} �����Ƿ�ɹ�
	 */
#ifdef	WIN32
	bool open(SOCKET fd);
#else
	bool open(int fd);
#endif

	/**
	 * ���� ACL_VSTREAM ����������
	 * @param vstream {ACL_VSTREAM*}
	 * @return {bool} �����Ƿ�ɹ�
	 */
	bool open(ACL_VSTREAM* vstream);

	/**
	 * ����Զ�̷�������������������
	 * @param addr {const char*} Զ�̷�����������ַ, ��ʽ: IP:PORT,
	 *  ��UNIXƽ̨, �õ�ַ������Ϊ���׽ӿڵ�ַ, ��: /tmp/mysock
	 * @param conn_timeout {int} ���ӳ�ʱʱ��(��)
	 * @param rw_timeout {int} ��д��ʱʱ��(��)
	 * @return {bool} �����Ƿ�ɹ�
	 */
	bool open(const char* addr, int conn_timeout, int rw_timeout);

	/**
	 * �󶨱��� UDP ��ַ������ UDP ����������
	 * @param addr {const char*} ������ַ����ʽ��ip:port
	 * @param rw_timeout {int} ��д��ʱʱ��(��)
	 * @return {bool} ���Ƿ�ɹ�
	 */
	bool bind_udp(const char* addr, int rw_timeout = 0);

	/**
	 * �ر�����������
	 * @return {bool} �ر��Ƿ�ɹ�
	 */
	bool close();

	/**
	 * ����������������׽������Ӿ��
	 * @return {ACL_SOCKET} �������򷵻� - 1(UNIX ƽ̨)
	 *  �� INVALID_SOCKET(win32ƽ̨)
	 */
#ifdef WIN32
	SOCKET sock_handle() const;
#else
	int   sock_handle() const;
#endif

	/**
	 * ����׽�����������İ󶨹�ϵ��ͬʱ���׽��ַ��ظ��û�����
	 * �����׽��ֵĹ���Ȩ�����û��������������ͷ�ʱ����رո���
	 * ���֣����û��ӹܸ��׽��ֺ��������뽫��ر�
	 * ������� close/open �ĵ����������⣬�����ĵ���(��������
	 * ���д����)��������
	 * @return {ACL_SOCKET} ���� ACL_SOCKET_INVALID ��ʾ��������
	 *  �Ѿ����׽��ֽ��
	 */
#ifdef WIN32
	SOCKET unbind_sock();
#else
	int    unbind_sock();
#endif

	/**
	 * ���Զ�����ӵĵ�ַ
	 * @param full {bool} �Ƿ���������ַ������IP:PORT������ò���
	 *  Ϊ false��������� IP�����򷵻� IP:PORT
	 * @return {const char*} Զ�����ӵ�ַ��������ֵ == '\0' ���ʾ
	 *  �޷����Զ�����ӵ�ַ
	 */
	const char* get_peer(bool full = false) const;

	/**
	 * ���Զ�����ӵ� IP ��ַ
	 * @return {const char*} Զ�����ӵ�ַ��������ֵ == '\0' ���ʾ
	 *  �޷����Զ�����ӵ�ַ
	 */
	const char* get_peer_ip() const;

	/**
	 * ����Զ�����Ӷ���ĵ�ַ������ TCP ���䷽ʽ������Ҫ��ʾ���ô˺���
	 * ����Զ�̶����ַ��UDP ���䷽ʽʱ��Ҫ���ô˺�������Զ�̵�ַ��Ȼ��
	 * �ſ�����Զ������д����
	 * @param addr {const char*} Զ�����Ӷ���ĵ�ַ����ʽ��ip:port
	 * @return {bool} ��������δ��ʱ���� false
	 */
	bool set_peer(const char* addr);

	/**
	 * ������ӵı��ص�ַ
	 * @param full {bool} �Ƿ���������ַ������IP:PORT������ò���
	 *  Ϊ false��������� IP�����򷵻� IP:PORT
	 * @return {const char*} �����ӵı��ص�ַ��������ֵ == "" ���ʾ
	 *  �޷���ñ��ص�ַ
	 */
	const char* get_local(bool full = false) const;

	/**
	 * ������ӵı��� IP ��ַ
	 * @return {const char*} �����ӵı��ص�ַ��������ֵ == "" ���ʾ
	 *  �޷���ñ��ص�ַ
	 */
	const char* get_local_ip() const;

	/**
	 * ���ñ��ص�ַ
	 * @param addr {const char*} ��ַ����ʽ��ip:port
	 * @return {bool} ��������δ��ʱ���� false
	 */
	bool set_local(const char* addr);

	/**
	 * ����׽ӿ����ӵĴ��״̬(�ڲ�ʹ���˷��������ķ�ʽ����̽��)
	 * @return {bool} ����������δ�򿪻��Ѿ��ر�ʱ�ú������� false�����
	 *  ���������򷵻� true
	 */
	bool alive() const;

	/////////////////////////////////////////////////////////////////////

	/**
	 * �ͷ� SSL ��ص��ڴ���Դ
	 */
	void close_ssl(void);

	/**
	 * �򿪿ͻ��˷�ʽ SSL ����
	 * @return {bool}
	 */
	bool open_ssl_client(void);

private:
	char  dummy_[1];
	char  peer_ip_[33];
	char  local_ip_[33];
	const char* get_ip(const char* addr, char* buf, size_t size);

	void* ssl_;
	void* ssn_;
	void* hs_;

	static int sock_read(void *ctx, unsigned char *buf, size_t len);
	static int sock_send(void *ctx, const unsigned char *buf, size_t len);

#ifdef WIN32
	static int ssl_read(SOCKET fd, void *buf, size_t len,
		int timeout, ACL_VSTREAM* stream, void *ctx);
	static int ssl_send(SOCKET fd, const void *buf, size_t len,
		int timeout, ACL_VSTREAM* stream, void *ctx);
#else
	static int ssl_read(int fd, void *buf, size_t len,
		int timeout, ACL_VSTREAM* stream, void *ctx);
	static int ssl_send(int fd, const void *buf, size_t len,
		int timeout, ACL_VSTREAM* stream, void *ctx);
#endif
};

} // namespace acl
