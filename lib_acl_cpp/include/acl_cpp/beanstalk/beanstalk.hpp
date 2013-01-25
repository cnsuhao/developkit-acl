#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/stream/socket_stream.hpp"

namespace acl
{

class ACL_CPP_API beanstalk
{
public:
	/**
	 * ���캯��
	 * @param addr {const char*} beanstalkd ��ַ����ʽ��ip:port/domain:port
	 * @param conn_timeout {int} ���ӷ������ĳ�ʱʱ��(��)
	 * @param retry {bool} ������Ӷ����Ƿ��Զ�����
	 */
	beanstalk(const char* addr, int conn_timeout, bool retry = true);
	~beanstalk();

	/////////////////////////////////////////////////////////////////////
	// �����ߵ��õĽӿ�

	/**
	 * ѡ�����õķ��͹ܵ�
	 * @param tube {const char*} �ܵ�����
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool use(const char* tube);

	/**
	 * ����ѡ�ܵ���ȱʡ�����з�����Ϣ
	 * @param data {const void*} ��Ϣ���ݵ�ַ�������Ƕ�����
	 * @param n {size_t} data �����峤��
	 * @param id {unsigned int*} ����ȷ���һ����Ϣ������ָ��ǿգ�
	 *  �������洢�������Ϣ��ID��
	 * @param pri {unsigned int} ���ȼ���ֵԽС�����ȼ�Խ��
	 * @param delay {int} ��ʾ��job����ready������Ҫ�ȴ�������
	 * @param ttr {int} ��ʾ����һ��workerִ�и���Ϣ������
	 * @return {bool} �Ƿ�ɹ�������ɹ��������ͨ�� get_id()
	 *  ����ö��з�����������Ϣ�������Ϣ��
	 */
	bool put(const void* data, size_t n, unsigned int* id = NULL,
		unsigned int pri = 1024, int delay = 0, int ttr = 60);

	/////////////////////////////////////////////////////////////////////
	// �����ߵ��õĽӿ�

	/**
	 * ѡ���ȡ��Ϣ�Ĺܵ�����������ع����б��У�
	 * �����ñ���������ʹ��ȱʡ�Ĺܵ�(default)
	 * @param tube {const char*} ��������
	 * @param n {int*} �����ָ��ǿգ�������ɹ����غ�õ�ַ�洢
	 *  ��ǰ��������ע����Ϣ�ܵ�(tube)������
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool watch(const char* tube, int* n = NULL);

	/**
	 * ȡ����ע(watch)һ��������Ϣ�Ĺܵ�(tube)
	 * @param tube {const char*} ��������
	 * @param n {int*} �����ָ��ǿգ�������ɹ����غ�õ�ַ�洢
	 *  ��ǰ��������ע����Ϣ�ܵ�(tube)������
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool ignore(const char* tube, int* n = NULL);

	/**
	 * ����Ϣ����ܵ��л�ȡһ����Ϣ��������ɾ����Ϣ����������
	 * �ȴ���ʱ�������Ϊ 0 
	 * @param buf {string&} �洢��õ�һ����Ϣ
	 * @param id {unsigned int*} ����ȷ���һ����Ϣ������ָ��ǿգ�
	 *  �������洢�������Ϣ��ID��
	 * @param timeout {int} �ȴ����з�����������Ϣ�ĳ�ʱֵ����Ϊ -1
	 *  ʱ���������ڵȴ����� > 0 ʱ�����ڸ�ʱ������û����Ϣ���򷵻أ�
	 *  �� == 0 ʱ������������һ����Ϣ�򷵻س�ʱ
	 * @return {bool} �Ƿ���һ����Ϣ
	 */
	bool reserve(string& buf, unsigned int* id = NULL, int timeout = -1);

	/**
	 * �Ӷ��з�������ɾ��ָ�� ID �ŵ���Ϣ
	 * @param id {unsigned int} ��Ϣ��
	 * @return {bool} �Ƿ�ɹ�ɾ��
	 */
	bool delete_id(unsigned int id);

	/**
	 * ��һ���Ѿ�����ȡ����Ϣ���·Ż�ready����(����job״̬��Ϊ "ready")��
	 * �ø�job���Ա������ͻ���ִ��
	 * @param id {unsigned int} ��Ϣ��
	 * @param pri {int} ���ȼ���
	 * @param delay {int} ��ʾ�ڸ���Ϣ������ready����֮ǰ��Ҫ�ȴ�������
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool release(unsigned int id, int pri = 1024, int delay = 0);

	/**
	 * ���һ����Ϣ��״̬��Ϊ "buried", Buried ��Ϣ������һ��FIFO�������У�
	 * �ڿͻ��˵���kick����֮ǰ����Щ��Ϣ�����ᱻ����˴���
	 * @param id {unsigned int} ��Ϣ��
	 * @param pri {int} ���ȼ���
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool bury(unsigned int id, int pri = 1024);

	/**
	 * ��������һ��worker������һ����Ϣ��ȡ����ִ�е�ʱ�䡣�������Щ��Ҫ
	 * ��ʱ����ɵ���Ϣ�Ƿǳ����õģ���ͬʱҲ��������TTR�����ƽ�һ����Ϣ
	 * ��һ���޷���ɹ�����worker�����ߡ�һ��worker����ͨ��������������
	 * �����������ִ�и�job (���磺���յ�DEADLINE_SOON�ǿ��Է���������)
	 * @param id {unsigned int} ��Ϣ��
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool touch(unsigned int id);

	/////////////////////////////////////////////////////////////////////
	// �����ӿ�

	/**
	 * ���� beanstalkd ��������ͨ������²���Ҫ��ʾ�ص��øú�������������
	 * ���Զ�������Ҫ�Զ����ñ�����
	 * @return {bool}  ��ɹ�
	 */
	bool open();

	/**
	 * ��ʾ�ر��� beanstalkd �����ӣ�������ʵ������ʱ�᳢�Ե��ùرչ���
	 */
	void close();

	/**
	 * ��ʾ֪ͨ beanstalkd �������˳�����(�������յ��������������ر�����)
	 */
	void quit();

	/**
	 * ��ȡ��Ϣ������ָ������Ϣ�ŵ�����
	 * @param buf {string&} �����Ϣ������洢������Ϣ
	 * @param id {unsigned int} ָ������Ϣ��
	 * @return {bool} �Ƿ���ָ����Ϣ�ŵ���Ϣ
	 */
	bool peek(string& buf, unsigned int id);

	/**
	 * ��õ�ǰ��ע (watch) �ܵ��е�һ�� ready ״̬��Ϣ��
	 * �����Ϣ������Ҳ��������
	 * @param buf {string&} �����Ϣ������洢������Ϣ
	 * @param id {unsigned int*} ����ȷ���һ����Ϣ������ָ��ǿգ�
	 *  �������洢�������Ϣ��ID��
	 * @return {bool} �Ƿ�����Ϣ
	 */
	bool peek_ready(string& buf, unsigned int* id);

	/**
	 * ��õ�ǰ��ע (watch) �ܵ��е�һ�� delayed ״̬��Ϣ��
	 * �����Ϣ������Ҳ��������
	 * @param buf {string&} �����Ϣ������洢������Ϣ
	 * @param id {unsigned int*} ����ȷ���һ����Ϣ������ָ��ǿգ�
	 *  �������洢�������Ϣ��ID��
	 * @return {bool} �Ƿ�����Ϣ
	 */
	bool peek_delayed(string& buf, unsigned int* id);

	/**
	 * ��õ�ǰ��ע (watch) �ܵ��е�һ�� buried ״̬��Ϣ��
	 * �����Ϣ������Ҳ��������
	 * @param buf {string&} �����Ϣ������洢������Ϣ
	 * @param id {unsigned int*} ����ȷ���һ����Ϣ������ָ��ǿգ�
	 *  �������洢�������Ϣ��ID��
	 * @return {bool} �Ƿ�����Ϣ
	 */
	bool peek_buried(string& buf, unsigned int* id);

	/**
	 * ������ֻ����Ե�ǰ����ʹ�õ�tubeִ�У����� buried
	 * ���� delayed ״̬����Ϣ�ƶ��� ready ����
	 * @param n {int} ��ʾÿ�� kick ��Ϣ�����ޣ�����˽���� kick
	 *  ����Ϣ����
	 * @return {int} ��ʾ����kick����������Ϣ����Ŀ������ -1 ��ʾ����
	 */
	int  kick(int n);

	/**
	 * ��ÿͻ���ǰ����ʹ�õ���Ϣ�ܵ�
	 * @param buf {string&} �洢��ǰʹ�õ���Ϣ�ܵ�
	 * @return {bool} �Ƿ�ɹ����
	 */
	bool list_tube_used(string&buf);

	/**
	 * ����Ѿ����ڵ�������Ϣ�ܵ�(tube)���б���
	 * @param buf {string&} �洢���
	 * @return {bool} �Ƿ�ɹ����
	 */
	bool list_tubes(string& buf);

	/**
	 * ��õ�ǰ��ע(watch)����Ϣ�ܵ��ļ���
	 * @param buf {string&} �洢���
	 * @return {bool} �Ƿ�ɹ����
	 */
	bool list_tubes_watched(string& buf);

	/**
	 * ����ʱ������ͣ��ָ����Ϣ�ܵ�(tube)�л�ȡ��Ϣ
	 * @param tube {const char*} ��Ϣ�ܵ���
	 * @param delay {int} ָ��ʱ���
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool pause_tube(const char* tube, int delay);

	/////////////////////////////////////////////////////////////////////
	// �����ӿ�
	const char* get_error() const
	{
		return errbuf_;
	}

private:
	char* addr_;
	int   timeout_;
	bool  retry_;
	char  errbuf_[128];
	acl::socket_stream conn_;
	bool peek_fmt(string& buf, unsigned int* id, const char* fmt, ...);
	bool list_tubes_fmt(string& buf, const char* fmt, ...);
};

} // namespace acl
