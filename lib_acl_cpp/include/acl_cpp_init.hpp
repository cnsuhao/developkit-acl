#pragma once
#include "acl_cpp_define.hpp"

namespace acl
{

	/**
	 * �� WIN32 dos �����£������Ҫʹ���׽ӿڲ�����
	 * ����Ҫ�ȵ��ô˺������г�ʼ��
	 */
	ACL_CPP_API void acl_cpp_init(void);

#ifdef WIN32
	ACL_CPP_API void open_dos(void);
#endif

}
