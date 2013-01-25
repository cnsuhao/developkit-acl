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
	beanstalk(const char* addr, int conn_timeout,
		bool retry = true);
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
	 * @param pri {unsigned int} ���ȼ���ֵԽС�����ȼ�Խ��
	 * @param delay {int} ��ʾ��job����ready������Ҫ�ȴ�������
	 * @param ttr {int} ��ʾ����һ��workerִ�и�job������
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool put(const void* data, size_t n, unsigned int pri = 1024,
		int delay = 0, int ttr = 60);

	/////////////////////////////////////////////////////////////////////
	// �����ߵ��õĽӿ�

	/**
	 * ѡ���ȡ��Ϣ�Ĺܵ�����������ع����б��У�
	 * �����ñ���������ʹ��ȱʡ�Ĺܵ�(default)
	 * @param tube {const char*} ��������
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool watch(const char* tube);

	/**
	 * ȥ��һ��������Ϣ�Ĺܵ�
	 * @param tube {const char*} ��������
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool ignore(const char* tube);

	/**
	 * ����Ϣ����ܵ��л�ȡһ����Ϣ��������ɾ����Ϣ����������
	 * �ȴ���ʱ�������Ϊ 0 
	 * @param buf {string&} �洢һ����Ϣ
	 */
	bool reserve(string& buf, int timeout = 0);
	bool delete_id(unsigned int id);
	bool release(unsigned int id, int pri = 1024, int delay = 0);
	bool bury(unsigned int id, int pri = 1024);
	bool touch(unsigned int id);

	/////////////////////////////////////////////////////////////////////
	// �����ӿ�
	bool open();
	void close();
	void quit();

	bool peek(string& buf, unsigned int id);
	bool peek_ready(string& buf);
	bool peek_delayed(string& buf);
	bool peek_buried(string& buf);
	int  kick(int n);
	bool list_tube_used(string&buf);
	bool list_tubes(string& buf);
	bool list_tubes_watched(string& buf);
	bool pause_tube(const char* tube, int delay);

	/////////////////////////////////////////////////////////////////////
	// �����ӿ�
	const char* get_error() const
	{
		return errbuf_;
	}

	unsigned int get_id() const
	{
		return id_;
	}

	unsigned int get_watch_count() const
	{
		return watched_;
	}

private:
	char* addr_;
	int   conn_timeout_;
	bool  retry_;
	char  errbuf_[128];
	unsigned int id_;
	unsigned int watched_;
	acl::socket_stream conn_;
	bool peek_fmt(string& buf, const char* fmt, ...);
	bool list_tubes_fmt(string& buf, const char* fmt, ...);
};

} // namespace acl
