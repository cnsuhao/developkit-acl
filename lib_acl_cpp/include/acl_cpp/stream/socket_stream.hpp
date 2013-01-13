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
	protected:
	private:
		char  dummy_[1];
		char  peer_ip_[33];
		char  local_ip_[33];
		const char* get_ip(const char* addr, char* buf, size_t size);
	};

} // namespace acl
