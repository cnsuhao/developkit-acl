#include "acl_stdafx.hpp"
#include <map>
#include "acl_cpp/connpool/connect_manager.hpp"
#include "acl_cpp/connpool/connect_monitor.hpp"
#include "acl_cpp/connpool/check_client.hpp"
#include "check_timer.hpp"

namespace acl
{

connect_monitor::connect_monitor(connect_manager& manager,
	int check_inter /* = 1 */, int conn_timeout /* = 10 */)
: stop_(false)
, stop_graceful_(true)
, handle_(ENGINE_KERNEL)
, manager_(manager)
, check_inter_(check_inter)
, conn_timeout_(conn_timeout)
{
}

connect_monitor::~connect_monitor()
{
}

void connect_monitor::stop(bool graceful)
{
	stop_ = true;
	stop_graceful_ = graceful;
}

void* connect_monitor::run()
{
	// �����������״̬��ʱ��
	check_timer timer(*this, handle_, conn_timeout_);

	timer.keep_timer(true);  // ���ֶ�ʱ��
	handle_.set_timer(&timer, check_inter_ * 1000000);

	while (!stop_)
		handle_.check();

	// �ȴ���ʱ������
	while (!timer.finish(stop_graceful_))
		handle_.check();

	return NULL;
}

bool connect_monitor::on_open(check_client& checker)
{
	// ����״̬�����������Ǵ���
	checker.set_alive(true);

	// ������ʽ���� close����Ϊ����ֱ�ӷ��� false ͨѶ��ܻ��Զ��رգ�
	// ��Ȼ�ڴ˴�����һ�� close Ҳ���޺���
	// checker.close();

	// ���� false ��ʹ�ϼ������߿��Թر��첽����
	return false;
}

} // namespace acl
