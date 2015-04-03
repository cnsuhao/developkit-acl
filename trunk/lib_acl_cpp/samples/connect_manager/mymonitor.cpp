#include "stdafx.h"
#include "mymonitor.h"

mymonitor::mymonitor(acl::connect_manager& manager,
	int check_inter, int conn_timeout)
	: acl::connect_monitor(manager, check_inter, conn_timeout)
{
}

mymonitor::~mymonitor(void)
{
}

bool mymonitor::on_open(acl::check_client& checker)
{
	checker_ = &checker;
	acl::aio_socket_stream& conn = checker_->get_conn();

	// ע������� IO ������̵Ļص�����
	conn.add_close_callback(this);
	conn.add_read_callback(this);
	conn.add_timeout_callback(this);

	int timeout = 10;

	// �첽��ȡһ�����ݣ�ͬʱҪ�󲻱��� \r\n

	conn.gets(timeout);

	// ���� true ��ʾ�ü����̼���
	return true;
}

bool mymonitor::read_callback(char* data, int len)
{
	// ��Ϊ acl ���첽 IO ���������ݿ϶����������������������� \0������ֱ��
	// ���ַ����Ƚ��ڴ˴��ǰ�ȫ��

	printf(">>>get: %s, len: %d\r\n", data, len);

	if (strncasecmp(data, "+OK", 3) == 0)
	{
		// �������������Ϊ���״̬
		checker_->set_alive(true);

		// �����رոü������
		checker_->close();

		// �˴����� true �� false �����ԣ���Ϊ�����Ѿ�����Ҫ��رռ������
		printf(">>> SERVER(%s) OK <<<\r\n", checker_->get_addr());
		return true;
	}

	// ���������Ϊ������״̬
	checker_->set_alive(false);

	printf(">>> SERVER(%s) ERROR <<<\r\n", checker_->get_addr());

	// ���� false ֪ͨ����Զ��رո�����
	return false;
}

bool mymonitor::timeout_callback()
{
	// ����ʱ������ֱ�ӽ�������Ϊ������
	checker_->set_alive(false);

	// ���� false ͨ������Զ��رոü������
	return false;
}

void mymonitor::close_callback()
{

}
