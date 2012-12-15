// wizard.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>

using namespace acl;

static bool file_copy(const char* from, const char* to)
{
	if (strcasecmp(from, to) == 0)
	{
		printf("from(%s) == to(%s)\r\n", from, to);
		return false;
	}

	ifstream in;
	if (in.open_read(from) == false)
	{
		printf("open %s error: %s\r\n", from, last_serror());
		return false;
	}

	ofstream out;
	if (out.open_write(to) == false)
	{
		printf("open %s error: %s\r\n", from, last_serror());
		return false;
	}

	string buf;

	while (!in.eof())
	{
		if (in.gets(buf, false) == false)
			break;
		if (out.write(buf) < 0)
		{
			printf("write to %s error: %s\r\n", to, last_serror());
			return false;
		}
	}

	return true;
}

struct FILE_FROM_TO 
{
	const char* from;
	const char* to;
};

static void create_master_threads()
{
	const FILE_FROM_TO tab[] = {
		{ "tmpl/Makefile.in", "demo/Makefile.in" },
		{ "tmpl/master/Makefile", "demo/Makefile" },
		{ "tmpl/master/master_service.sln", "demo/master_service.sln" },
		{ "tmpl/master/master_service.vcproj", "demo/master_service.vcproj" },
		{ "tmpl/master/master_threads.cf", "demo/master_service.cf" },
		{ "tmpl/master/main_threads.cpp", "demo/main.cpp" },
		{ "tmpl/master/stdafx.h", "demo/stdafx.h" },
		{ "tmpl/master/stdafx.cpp", "demo/stdafx.cpp" },
		{ "tmpl/master/master_threads.h", "demo/master_service.h" },
		{ "tmpl/master/master_threads.cpp", "demo/master_service.cpp" },
		{ NULL, NULL }
	};

	for (size_t i = 0; tab[i].from != NULL; i++)
	{
		if (file_copy(tab[i].from, tab[i].to) == false)
			return;
	}
	printf("create ok!\r\n");
}

static void create_master_proc()
{
	const FILE_FROM_TO tab[] = {
		{ "tmpl/Makefile.in", "demo/Makefile.in" },
		{ "tmpl/master/Makefile", "demo/Makefile" },
		{ "tmpl/master/master_service.sln", "demo/master_service.sln" },
		{ "tmpl/master/master_service.vcproj", "demo/master_service.vcproj" },
		{ "tmpl/master/master_proc.cf", "demo/master_service.cf" },
		{ "tmpl/master/main_proc.cpp", "demo/main.cpp" },
		{ "tmpl/master/stdafx.h", "demo/stdafx.h" },
		{ "tmpl/master/stdafx.cpp", "demo/stdafx.cpp" },
		{ "tmpl/master/master_proc.h", "demo/master_service.h" },
		{ "tmpl/master/master_proc.cpp", "demo/master_service.cpp" },
		{ NULL, NULL }
	};

	for (size_t i = 0; tab[i].from != NULL; i++)
	{
		if (file_copy(tab[i].from, tab[i].to) == false)
			break;
	}
	printf("create ok!\r\n");
}

static void create_master_aio()
{
	const FILE_FROM_TO tab[] = {
		{ "tmpl/Makefile.in", "demo/Makefile.in" },
		{ "tmpl/master/Makefile", "demo/Makefile" },
		{ "tmpl/master/master_service.sln", "demo/master_service.sln" },
		{ "tmpl/master/master_service.vcproj", "demo/master_service.vcproj" },
		{ "tmpl/master/master_aio.cf", "demo/master_service.cf" },
		{ "tmpl/master/main_aio.cpp", "demo/main.cpp" },
		{ "tmpl/master/stdafx.h", "demo/stdafx.h" },
		{ "tmpl/master/stdafx.cpp", "demo/stdafx.cpp" },
		{ "tmpl/master/master_aio.h", "demo/master_service.h" },
		{ "tmpl/master/master_aio.cpp", "demo/master_service.cpp" },
		{ NULL, NULL }
	};

	for (size_t i = 0; tab[i].from != NULL; i++)
	{
		if (file_copy(tab[i].from, tab[i].to) == false)
			break;
	}
	printf("create ok!\r\n");
}

static void create_master_trigger()
{
	const FILE_FROM_TO tab[] = {
		{ "tmpl/Makefile.in", "demo/Makefile.in" },
		{ "tmpl/master/Makefile", "demo/Makefile" },
		{ "tmpl/master/master_service.sln", "demo/master_service.sln" },
		{ "tmpl/master/master_service.vcproj", "demo/master_service.vcproj" },
		{ "tmpl/master/master_trigger.cf", "demo/master_service.cf" },
		{ "tmpl/master/main_trigger.cpp", "demo/main.cpp" },
		{ "tmpl/master/stdafx.h", "demo/stdafx.h" },
		{ "tmpl/master/stdafx.cpp", "demo/stdafx.cpp" },
		{ "tmpl/master/master_trigger.h", "demo/master_service.h" },
		{ "tmpl/master/master_trigger.cpp", "demo/master_service.cpp" },
		{ NULL, NULL }
	};

	for (size_t i = 0; tab[i].from != NULL; i++)
	{
		if (file_copy(tab[i].from, tab[i].to) == false)
			break;
	}
	printf("create ok!\r\n");
}

static void create_master()
{
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
}

static void create_db()
{

}

int main(int argc, char* argv[])
{
	(void) argc, (void) argv;

	acl::acl_cpp_init();

	acl_make_dirs("./demo", 0755);

	while (true)
	{
		printf("select one below:\r\n");
		printf("m: master_service; d: db; q: exit\r\n");
		printf(">"); fflush(stdout);

		char buf[256];
		int n = acl_vstream_gets_nonl(ACL_VSTREAM_IN, buf, sizeof(buf));
		if (n == ACL_VSTREAM_EOF)
			break;
		else if (strcasecmp(buf, "m") == 0)
			create_master();
		else if (strcasecmp(buf, "d") == 0)
			create_db();
		else if (strcasecmp(buf, "q") == 0)
			break;
		else
			printf("unknown %s\r\n", buf);
	}

	printf("Bye!\r\n");
	return 0;
}
