#include "acl_stdafx.hpp"
#include "acl_cpp/acl_cpp_init.hpp"

namespace acl
{

	void acl_cpp_init(void)
	{
		acl_init();
	}

#ifdef WIN32
	void open_dos(void)
	{
		// �� DOS ����
		AllocConsole();
		FILE* fp = freopen("CONOUT$","w+t",stdout);
	}
#endif

}
