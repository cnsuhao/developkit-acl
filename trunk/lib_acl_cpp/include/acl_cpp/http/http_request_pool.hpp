#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/connpool/connect_pool.hpp"

namespace acl
{

/**
 * http �ͻ������ӳ��࣬���ุ��Ϊ connect_pool������ֻ��ʵ�ָ����е��麯��
 * create_connect ��ӵ�������ӳظ��� connect_pool �Ĺ��ܣ����⣬���ഴ��
 * �����Ӷ����� http_reuqest ���������ڵ��� connect_pool::peek ʱ����
 * �ı��� http_request �࣬��������Ҫ�� peek ���ص������ǿ��תΪ http_request
 * ����󣬱����ʹ�� http_request �������й��ܣ����� http_reuqest ��Ϊ
 * connect_client ������
 */
class http_reuqest_pool : public connect_pool
{
public:
	/**
	 * ���캯��
	 * @param addr {const char*} ������������ַ����ʽ��ip:port(domain:port)
	 * @param count {int} ���ӳ�������Ӹ�������
	 * @param retry_inter {int} �����ӶϿ��������ٴδ����ӵ�ʱ����(��)��
	 *  ����ֵ <= 0 ʱ��ʾ�������ӶϿ����������������������볬����ʱ�������
	 *  ����Ͽ�����
	 */
	http_reuqest_pool(const char* addr, int count, int retry_inter = 0);
	~http_reuqest_pool();

	/**
	 * �����������ӳ�ʱʱ�估���� IO ��д��ʱʱ��(��)
	 * @param conn_timeout {int} ���ӳ�ʱʱ��
	 * @param rw_timeout {int} ���� IO ��д��ʱʱ��(��)
	 * @return {http_request_pool&}
	 */
	http_reuqest_pool& set_timeout(int conn_timeout = 30, int rw_timeout = 60);

protected:
	// ���ി�麯��
	virtual connect_client* create_connect();

private:
	int   conn_timeout_;
	int   rw_timeout_;
};

} // namespace acl
