#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/stream/socket_stream.hpp"

typedef struct _ssl_session ssl_session;
typedef struct _ssl_context ssl_context;

namespace acl
{

	class ACL_CPP_API ssl_stream : public socket_stream
	{
	public:
		ssl_stream(void);
		virtual ~ssl_stream(void);

		/**
		 * �����׽��ִ򿪵�һ��������
		 * @param fd �׽���
		 * @param use_ssl {bool} �Ƿ���� SSL ���ӷ�ʽ
		 * @return {bool} �����Ƿ�ɹ�
		 */
#ifdef	WIN32
		bool open_ssl(SOCKET fd, bool use_ssl = true);
#else
		bool open_ssl(int fd, bool use_ssl = true);
#endif

		/**
		 * ���� ACL_VSTREAM ����������
		 * @param vstream {ACL_VSTREAM*}
		 * @param use_ssl {bool} �Ƿ���� SSL ���ܴ��䷽ʽ
		 * @return {bool} �����Ƿ�ɹ�
		 */
		bool open_ssl(ACL_VSTREAM* vstream, bool use_ssl = true);

		/**
		 * ����Զ�̷�������������������
		 * @param addr {const char*} Զ�̷�����������ַ, ��ʽ: IP:PORT,
		 *  ��UNIXƽ̨, �õ�ַ������Ϊ���׽ӿڵ�ַ, ��: /tmp/mysock
		 * @param conn_timeout {int} ���ӳ�ʱʱ��(��)
		 * @param rw_timeout {int} ��д��ʱʱ��(��)
		 * @param use_ssl {bool} �Ƿ���� SSL ���ܴ��䷽ʽ
		 * @return {bool} �����Ƿ�ɹ�
		 */
		bool open_ssl(const char* addr, int conn_timeout,
			int rw_timeout, bool use_ssl = true);

		/**
		 * �ú����������Ѿ��򿪵������в�������������ڽ�����Ϊ SSL ģʽ
		 * ��� SSL ģʽ
		 * @param on {bool} �Ƿ����� SSL ģʽ�����ò���Ϊ false ʱ�����
		 *  ��ǰ���Ѿ��� SSL ģʽ����ر� SSL ģʽ�������ǰ��Ϊ�� SSL
		 *  ģʽ����ֱ�ӷ��أ����ò���Ϊ true ʱ�������ǰ���Ѿ��� SSL
		 *  ģʽ����ֱ�ӷ��أ������ǰ��Ϊ�� SSL ģʽ����� SSL ģʽ
		 * @return {bool}
		 */
		bool open_ssl(bool on);

	protected:
	private:
		ssl_context* ssl_;
		ssl_session* ssn_;
		void* hs_;

		bool ssl_client_init(void);

		static int __read(void *ctx, unsigned char *buf, size_t len);
		static int __send(void *ctx, const unsigned char *buf, size_t len);

#ifdef WIN32
		static int __ssl_read(SOCKET fd, void *buf, size_t len,
			int timeout, void *ctx);
		static int __ssl_send(SOCKET fd, const void *buf, size_t len,
			int timeout, void *ctx);
#else
		static int __ssl_read(int fd, void *buf, size_t len,
			int timeout, void *ctx);
		static int __ssl_send(int fd, const void *buf, size_t len,
			int timeout, void *ctx);
#endif

		void clear(void);
	};

}
