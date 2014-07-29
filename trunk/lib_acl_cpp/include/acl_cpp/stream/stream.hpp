#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/stdlib/string.hpp"
#include <map>

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
	 * �����������־λ������ eof_ ��־λ��Ϊ false
	 */
	void clear_eof(void);

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
	 * @param key {const char* } ��ʶ�� ctx �ļ�
	 */
	void set_ctx(void* ctx, const char* key = NULL);

	/**
	 * ��������󶨵Ķ���
	 * @param key {const char* key} �ǿ�ʱʹ�ø� key ��ѯ��Ӧ�� ctx ����
	 *  ���򷵻�ȱʡ�� ctx ����
	 * @return {void*}
	 */
	void* get_ctx(const char* key = NULL) const;

	/**
	 * ɾ�����а󶨵Ķ���
	 * @param key {const char*} �ǿ�ʱɾ����Ӧ�� key �� ctx ���󣬷���ɾ��
	 *  ȱʡ�� ctx ����
	 * @return {void*} �����󲻴���ʱ���� NULL���ɹ�ɾ���󷵻ظö���
	 */
	void* del_ctx(const char* key = NULL);

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
	ACL_VSTREAM *stream_;
	bool eof_;
	bool opened_;

	void* default_ctx_;
	std::map<string, void*> ctx_table_;
};

} // namespace acl
