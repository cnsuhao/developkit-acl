#pragma once
#include <vector>
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/stdlib/thread.hpp"
#include "acl_cpp/stream/aio_handle.hpp"

namespace acl
{

class aio_handle;
class check_client;
class connect_manager;

class connect_monitor : public thread
{
public:
	/**
	 * ���캯��
	 * @param manager {connect_manager&}
	 * @param check_inter {int} ����ʱ����(��)
	 * @param conn_timeout {int} ���ӷ������ĳ�ʱʱ��(��)
	 */
	connect_monitor(connect_manager& manager,
		int check_inter = 1, int conn_timeout = 10);

	virtual ~connect_monitor();

	/**
	 * ֹͣ�߳�
	 * @param graceful {bool} �Ƿ������عرռ����̣����Ϊ true
	 *  �������еļ�����ӹرպ����̲߳ŷ��أ�������ֱ�Ӽ���߳�
	 *  ֱ�ӷ��أ����ܻ����һЩ���ڼ�������δ���ͷš�������ˣ����
	 *  ���ӳؼ�Ⱥ��������ǽ�����ȫ�ֵģ����Խ��˲�����Ϊ false�����
	 *  ���ӳؼ�Ⱥ������������й�������Ҫ����δ������ͷţ���Ӧ����Ϊ true
	 */
	void stop(bool graceful);

	/**
	 * ��� connect_manager ���ö���
	 * @return {connect_manager&}
	 */
	connect_manager& get_manager() const
	{
		return manager_;
	}

	/**
	 * �麯��������������ر�����������һ���жϸ������Ƿ��Ǵ���
	 * @param checker {check_client&} ��������ӵļ����󣬿���ͨ��
	 *  check_client ���еķ������£�
	 *  1) get_conn ��÷��������Ӿ��
	 *  2) get_addr ��÷���˵�ַ
	 *  3) set_alive ���������Ƿ���
	 *  4) close �ر�����
	 * @return {bool} ���� true ��ʾ���໹��Ҫ��һ�����м�⣬���� false ��
	 *  ���������Ѿ�������
	 */
	virtual bool on_open(check_client& checker);

protected:
	// ���ി�麯��
	virtual void* run();

private:
	bool stop_;
	bool stop_graceful_;
	aio_handle handle_;			// ��̨����̵߳ķ��������
	connect_manager& manager_;		// ���ӳؼ��Ϲ������
	int   check_inter_;			// ������ӳ�״̬��ʱ����(��)
	int   conn_timeout_;			// ���ӷ������ĳ�ʱʱ��
};

} // namespace acl
