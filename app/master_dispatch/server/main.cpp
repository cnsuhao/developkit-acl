#include "stdafx.h"
#include "master_service.h"

int main(int argc, char* argv[])
{
	// ��ʼ�� acl ��
	acl::acl_cpp_init();

	master_service& ms = acl::singleton2<master_service>::get_instance();

	// �������ò�����
	ms.set_cfg_int(var_conf_int_tab);
	ms.set_cfg_int64(var_conf_int64_tab);
	ms.set_cfg_str(var_conf_str_tab);
	ms.set_cfg_bool(var_conf_bool_tab);

	// ��ʼ����

	if (argc >= 3 && strcmp(argv[1], "alone") == 0)
	{
		// �������з�ʽ

		acl::log::stdout_open(true);  // ��־�������׼���
		const char* addr = argv[2];

		printf("listen on: %s\r\n", addr);

		if (argc >= 4)
			ms.run_alone(addr, argv[3], acl::ENGINE_SELECT); 
		else
			ms.run_alone(addr, NULL, acl::ENGINE_SELECT);

		printf("Enter any key to exit now\r\n");
		getchar();
	}
	else
		ms.run_daemon(argc, argv);  // acl_master ����ģʽ����

	return 0;
}