#pragma once
#include "acl_cpp/acl_cpp_define.hpp"

struct ACL_VSTREAM;

namespace acl {

	class ACL_CPP_API stream
	{
	public:
		stream(void);
		virtual ~stream(void) = 0;

		/**
		* ���麯��, ���಻��ֱ�ӱ�ʵ����, Ҫ���������ʵ�����رպ���
		* @return {bool} true: �رճɹ�; false: �ر�ʧ��
		*/
		virtual bool close(void) = 0;

		/**
		* �ж����Ƿ��Ѿ�����
		* @return {bool} true: ���Ѿ�����; false: ��δ����
		*/
		bool eof(void) const;

		/**
		* ��ǰ���Ƿ����״̬
		* @return {bool} true: ���Ѿ���; false: ��δ��
		*/
		bool opened(void) const;

		/**
		 * ��õ�ǰ���� ACL_VSTREAM ������
		 * @return {ACL_VSTREAM*}
		 */
		ACL_VSTREAM* get_vstream() const;

		/**
		 * ��� ACL_VSTREAM ��������İ󶨹�ϵ��ͬʱ�� ACL_VSTREAM ����
		 * ���û��������� ACL_VSTREAM�Ĺ���Ȩ�����û��������������ͷ�ʱ
		 * ����رո� ACL_VSTREAM �����û��ӹܸ� ACL_VSTREAM �������
		 * ���뽫��رգ�������� close/open �ĵ����������⣬�����ĵ���
		 * (�����������д����)��������
		 * @return {ACL_VSTREAM} ���� NULL ��ʾ�������Ѿ��� ACL_VSTREAM ���
		 */
		ACL_VSTREAM* unbind();

		/**
		 * ��������������Ѿ��򿪣��򲻻��ظ���
		 */
		void open_stream(void);

		/**
		 * ���´�������������Ѿ��������ͷ��������ٴ�
		 */
		void reopen_stream(void);

		/**
		 * �������İ󶨶���
		 * @param ctx {void*}
		 */
		void set_ctx(void* ctx);

		/**
		 * ��������󶨵Ķ���
		 * @return {void*}
		 */
		void* get_ctx() const;

		/**
		 * �������Ķ�д��ʱʱ��
		 * @param n {int} ��ʱʱ��(��λ: ��)
		 */
		void set_rw_timeout(int n);

		/**
		 * ��õ�ǰ���Ķ�д��ʱʱ��
		 * @return {int} ������Ķ�д��ʱʱ��(��)
		 */
		int get_rw_timeout(void) const;
	protected:
		ACL_VSTREAM *m_pStream;
		bool m_bEof;
		bool m_bOpened;

		void* ctx_;
	};

} // namespace acl
