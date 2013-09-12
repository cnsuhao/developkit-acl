#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/connpool/connect_manager.hpp"

namespace acl
{

/**
 * HTTP �ͻ����������ӳع�����
 */
class ACL_CPP_API http_request_manager : public acl::connect_manager
{
public:
	http_request_manager();
	virtual ~http_request_manager();

protected:
	/**
	 * ���ി�麯���������������ӳض���
	 * @param idx {size_t} �����ӳض����ڼ����е��±�λ��(�� 0 ��ʼ)
	 * @param addr {const char*} ������������ַ����ʽ��ip:port
	 * @param count {int} ���ӳصĴ�С����
	 */
	virtual connect_pool* create_pool(size_t idx, const char* addr, int count);
};

} // namespace acl
