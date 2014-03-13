#pragma once
#include <vector>
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/stdlib/thread.hpp"
#include "acl_cpp/stream/aio_handle.hpp"

namespace acl
{

class aio_handle;
class connect_manager;

class connect_monitor : public thread
{
public:
	connect_monitor(connect_manager& manager,
		int check_inter = 1, int conn_timeout = 10);
	~connect_monitor();

	/**
	 * ֹͣ�߳�
	 */
	void stop();

protected:
	// ���ി�麯��
	virtual void* run();

private:
	bool stop_;
	aio_handle handle_;			// ��̨����̵߳ķ��������
	connect_manager& manager_;		// ���ӳؼ��Ϲ������
	int   check_inter_;			// ������ӳ�״̬��ʱ����(��)
	int   conn_timeout_;			// ���ӷ������ĳ�ʱʱ��
};

} // namespace acl
