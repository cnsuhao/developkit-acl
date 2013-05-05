#pragma once
#include "acl_cpp/acl_cpp_define.hpp"

namespace acl {

class ACL_CPP_API http_utils
{
public:
	http_utils() {}
	~http_utils() {}

	/**
	 * �������� url �л�� WEB ��������ַ����ʽ��ip/domain:port
	 * @param url {const char*} HTTP url���ǿ�
	 * @param out {char*} �洢���
	 * @param size {size_t} out ��������С
	 * @return {bool} �Ƿ�ɹ����
	 */
	static bool get_addr(const char* url, char* out, size_t size);
};

} // namespace acl
