#include "stdafx.h"
#include "file_copy.h"
#include "master_creator.h"

static const char* src_path_ = "tmpl/master";
static const char* dst_path_ = "master_demo";

static bool create_master_threads()
{
	const char* name = "master_threads";
	const FILE_FROM_TO tab[] = {
		{ "Makefile", "Makefile" },
		{ "master_service.sln", "master_service.sln" },
		{ "master_service.vcproj", "master_service.vcproj" },
		{ "master_threads.cf", "master_service.cf" },
		{ "main_threads.cpp", "main.cpp" },
		{ "stdafx.h", "stdafx.h" },
		{ "stdafx.cpp", "stdafx.cpp" },
		{ "master_threads.h", "master_service.h" },
		{ "master_threads.cpp", "master_service.cpp" },
		{ NULL, NULL }
	};

	return files_copy(name, tab, src_path_, dst_path_);
}

static bool create_master_proc()
{
	const char* name = "master_proc";
	const FILE_FROM_TO tab[] = {
		{ "Makefile", "Makefile" },
		{ "master_service.sln", "master_service.sln" },
		{ "master_service.vcproj", "master_service.vcproj" },
		{ "master_proc.cf", "master_service.cf" },
		{ "main_proc.cpp", "main.cpp" },
		{ "stdafx.h", "stdafx.h" },
		{ "stdafx.cpp", "stdafx.cpp" },
		{ "master_proc.h", "master_service.h" },
		{ "master_proc.cpp", "master_service.cpp" },
		{ NULL, NULL }
	};

	return files_copy(name, tab, src_path_, dst_path_);
}

static bool create_master_aio()
{
	const char* name = "master_proc";
	const FILE_FROM_TO tab[] = {
		{ "Makefile", "Makefile" },
		{ "master_service.sln", "master_service.sln" },
		{ "master_service.vcproj", "master_service.vcproj" },
		{ "master_aio.cf", "master_service.cf" },
		{ "main_aio.cpp", "main.cpp" },
		{ "stdafx.h", "stdafx.h" },
		{ "stdafx.cpp", "stdafx.cpp" },
		{ "master_aio.h", "master_service.h" },
		{ "master_aio.cpp", "master_service.cpp" },
		{ NULL, NULL }
	};

	return files_copy(name, tab, src_path_, dst_path_);
}

static bool create_master_trigger()
{
	const char* name = "master_trigger";
	const FILE_FROM_TO tab[] = {
		{ "Makefile", "Makefile" },
		{ "master_service.sln", "master_service.sln" },
		{ "master_service.vcproj", "master_service.vcproj" },
		{ "master_trigger.cf", "master_service.cf" },
		{ "main_trigger.cpp", "main.cpp" },
		{ "stdafx.h", "stdafx.h" },
		{ "stdafx.cpp", "stdafx.cpp" },
		{ "master_trigger.h", "master_service.h" },
		{ "master_trigger.cpp", "master_service.cpp" },
		{ NULL, NULL }
	};

	return files_copy(name, tab, src_path_, dst_path_);
}

void create_master()
{
	acl_make_dirs(dst_path_, 0755);

	while (true)
	{
		printf("choose master_service type:\r\n");
		printf("t: for master_threads; p: for master_proc; "
			"a: for master_aio; g: for master_trigger; s: skip choose\r\n");
		printf(">"); fflush(stdout);

		char buf[256];
		int n = acl_vstream_gets_nonl(ACL_VSTREAM_IN, buf, sizeof(buf));
		if (n == ACL_VSTREAM_EOF)
			break;
		else if (strcasecmp(buf, "t") == 0)
		{
			create_master_threads();
			break;
		}
		else if (strcasecmp(buf, "p") == 0)
		{
			create_master_proc();
			break;
		}
		else if (strcasecmp(buf, "a") == 0)
		{
			create_master_aio();
			break;
		}
		else if (strcasecmp(buf, "g") == 0)
		{
			create_master_trigger();
			break;
		}
		else if (strcasecmp(buf, "s") == 0)
			break;
		else
			printf("unknown ch: %s\r\n", buf);
	}
	for (int i = 0; i < 78; i++)
		putchar('-');
	putchar('\r');
	putchar('\n');
}
