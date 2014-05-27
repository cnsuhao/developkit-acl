#include "lib_acl.h"
#include "acl_cpp/stream/ofstream.hpp"
#include "acl_cpp/stdlib/string.hpp"
#include <stdio.h>
#include <string>

//////////////////////////////////////////////////////////////////////////

static void test_main(void)
{
	acl::string s("hello world!\r\n"
		"you're welcome\r\n"
		"what's your name\r\n"
		"happy new year");
	acl::string line;

	while (true)
	{
		if (s.scan_line(line, true, NULL) == true)
		{
			printf(">>line: %s\r\n", line.c_str());
			line.clear();
		}
		else
		{
			if (s.empty())
				break;
			printf(">>last: %s\r\n", s.c_str());
			break;
		}
	}
}

int main(void)
{
	test_main();

#ifdef WIN32
	printf("enter any key to exit\r\n");
	getchar();
#endif
	return (0);
}
