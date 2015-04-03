#pragma once

class mymonitor : public acl::connect_monitor, public acl::aio_callback
{
public:
	mymonitor(acl::connect_manager& manager,
		int check_inter = 1, int conn_timeout = 10);
	~mymonitor(void);

protected:
	/**
	 * ���� connect_monitor �麯�������ر�����������һ���жϸ������Ƿ��Ǵ���
	 * @param checker {check_client&} ��������ӵļ����󣬿���ͨ��
	 *  check_client ���еķ������£�
	 *  1) get_conn ��÷��������Ӿ��
	 *  2) get_addr ��÷���˵�ַ
	 *  3) set_alive ���������Ƿ���
	 *  4) close �ر�����
	 * @return {bool} ���� true ��ʾ���໹��Ҫ��һ�����м�⣬���� false
	 *  ����������Ѿ�������
	 */
	bool on_open(acl::check_client& checker);

protected:
	// ���ظ��� aio_callback �е��麯��

	/**
	 * �ͻ������Ķ��ɹ��ص�����
	 * @param data {char*} ���������ݵ�ַ
	 * @param len {int} ���������ݳ���
	 * @return {bool} ���� true ��ʾ����������ϣ���رո��첽��
	 */
	bool read_callback(char* data, int len);

	/**
	 * �ͻ������ĳ�ʱ�ص�����
	 * @return {bool} ���� true ��ʾ����������ϣ���رո��첽��
	 */
	bool timeout_callback();

	/**
	 * �ͻ������ĳ�ʱ�ص�����
	 */
	void close_callback();

private:
	acl::check_client* checker_;
};
