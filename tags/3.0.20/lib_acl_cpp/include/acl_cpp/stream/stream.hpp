#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/stdlib/string.hpp"
#include <map>

struct ACL_VSTREAM;

namespace acl {

class stream;

/**
 * ������ IO ע��ص��࣬������ʵ�ָ����е��鷽�����������ͨ�� stream::setup_hook ע�ᣬ
 * Ȼ�����������е� IO ������Ϊ stream �����ĵײ� IO ���̱�ʹ�ã�
 * ��������� stream::setup_hook ע����̣��� stream �����ĵײ� IO ����ΪĬ�Ϲ���
 * XXX�� �������౻����Ϊ�Ѷ����࣬��������ҲӦ������Ϊ�Ѷ�����
 */
class ACL_CPP_API stream_hook
{
public:
	stream_hook() {}

	/**
	 * �����ݽӿ�
	 * @param buf {void*} ����������ַ�����������ݽ�����ڸû�������
	 * @param len {size_t} buf ��������С
	 * @return {int} �����ֽ�����������ֵ < 0 ʱ��ʾ����
	 */
	virtual int read(void* buf, size_t len) = 0;

	/**
	 * �������ݽӿ�
	 * @param buf {const void*} ���ͻ�������ַ
	 * @param len {size_t} buf �����������ݵĳ���(���� > 0)
	 * @return {int} д������ݳ��ȣ�����ֵ <����ʱ��ʾ����
	 */
	virtual int send(const void* buf, size_t len) = 0;

	/**
	 * �� stream::setup_hook �ڲ�������� stream_hook::open ���̣��Ա����������
	 * ������ʼ��һЩ���ݼ��Ự
	 * @param s {stream*} �� setup_hook �ڲ����ø÷�������������������Ϊ��������
	 * @return {bool} �������ʵ������ false���� setup_hook ����ʧ���һ�ָ�ԭ��
	 */
	virtual bool open(stream* s) = 0;

	/**
	 * �� stream ������ر�ǰ����ص��ú����Ա�������ʵ����һЩ�ƺ���
	 * @param alive {bool} �������Ƿ���Ȼ����
	 * @return {bool}
	 */
	virtual bool on_close(bool alive) { (void) alive; return true; }

	/**
	 * �� stream ������Ҫ�ͷ� stream_hook �������ʱ���ô˷���
	 */
	virtual void destroy() {}

protected:
	virtual ~stream_hook() {}
};

class ACL_CPP_API stream
{
public:
	stream(void);
	virtual ~stream(void) = 0;

	/**
	 * ���ñ������ر�������
	 * @return {bool} true: �رճɹ�; false: �ر�ʧ��
	 */
	bool close(void);

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
	* ��ǰ���Ƿ�����״̬
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
	 * @param replace {bool} ����Ӧ�� KEY ����ʱ�Ƿ���������
	 * @return {bool} �� replace Ϊ false �� key �Ѿ�����ʱ�򷵻� false
	 */
	bool set_ctx(void* ctx, const char* key = NULL, bool replace = true);

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

	/**
	 * ע���д�������ڲ����Զ����� hook->open ���̣�����ɹ����򷵻�֮ǰע��Ķ���
	 * (����ΪNULL)����ʧ���򷵻������������ͬ��ָ�룬Ӧ�ÿ���ͨ���жϷ���ֵ������ֵ
	 * �Ƿ���ͬ���ж�ע���������Ƿ�ɹ�
	 * xxx: �ڵ��ô˷���ǰ���뱣֤�������Ѿ�����
	 * @param hook {stream_hook*} �ǿն���ָ��
	 * @return {stream_hook*} ����ֵ������ֵ��ͬ���ʾ�ɹ�
	 */
	stream_hook* setup_hook(stream_hook* hook);

	/**
	 * ��õ�ǰע�������д����
	 * @return {stream_hook*}
	 */
	stream_hook* get_hook() const;

	/**
	 * ɾ����ǰע�������д���󲢷��ظö��󣬻ָ�ȱʡ�Ķ�д����
	 * @return {stream_hook*}
	 */
	stream_hook* remove_hook();

protected:
	stream_hook* hook_;
	ACL_VSTREAM *stream_;
	bool eof_;
	bool opened_;

	void* default_ctx_;
	std::map<string, void*> ctx_table_;

private:
#ifdef WIN32
	static int read_hook(SOCKET fd, void *buf, size_t len,
		int timeout, ACL_VSTREAM* stream, void *ctx);
	static int send_hook(SOCKET fd, const void *buf, size_t len,
		int timeout, ACL_VSTREAM* stream, void *ctx);

	static int fread_hook(HANDLE fd, void *buf, size_t len,
		int timeout, ACL_VSTREAM* stream, void *ctx);
	static int fsend_hook(HANDLE fd, const void *buf, size_t len,
		int timeout, ACL_VSTREAM* stream, void *ctx);
#else
	static int read_hook(int fd, void *buf, size_t len,
		int timeout, ACL_VSTREAM* stream, void *ctx);
	static int send_hook(int fd, const void *buf, size_t len,
		int timeout, ACL_VSTREAM* stream, void *ctx);

	static int fread_hook(int fd, void *buf, size_t len,
		int timeout, ACL_VSTREAM* stream, void *ctx);
	static int fsend_hook(int fd, const void *buf, size_t len,
		int timeout, ACL_VSTREAM* stream, void *ctx);
#endif
};

} // namespace acl