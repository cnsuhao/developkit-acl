#pragma once

namespace acl
{

class connect_client
{
public:
	connect_client() : when_(0) {}
	virtual ~connect_client() {}

	/**
	 * ��ø����Ӷ������һ�α�ʹ�õ�ʱ���
	 * @return {time_t}
	 */
	time_t get_when()
	{
		return when_;
	}

	/**
	 * ���ø����Ӷ���ǰ��ʹ�õ�ʱ���
	 */
	void set_when(time_t when)
	{
		when_ = when;
	}

	/**
	 * ���麯�����������ʵ�ִ˺����������ӷ�����
	 * @param addr {const char*} ��������ַ����ʽ��ip:port(domain:port)
	 * @param conn_timeout {int} ���ӷ������ĳ�ʱʱ��(�룩
	 * @param rw_timeout {int} ������������ IO ��д��ʱʱ��(��)
	 * @return {bool} �Ƿ����ӳɹ�
	 */
	virtual bool open(const char* addr, int conn_timeout = 30,
		int rw_timemout = 60) = 0;
private:
	time_t when_;
};

} // namespace acl
