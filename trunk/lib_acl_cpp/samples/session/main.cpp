#include "lib_acl.hpp"
#include <getopt.h>

using namespace acl;

static void usage(const char* procname)
{
	printf("usage: %s -h[help] -s memcahced_addr[example: 127.0.0.1:11211] -n count\r\n", procname);
}

int main(int argc, char* argv[])
{
	char  addr[32];
	int   ch, n = 10;

	snprintf(addr, sizeof(addr), "127.0.0.1:11211");

	while ((ch = getopt(argc, argv, "hs:n:")) > 0)
	{
		switch (ch)
		{
		case 'h':
			usage(argv[0]);
			return 0;
		case 's':
			snprintf(addr, sizeof(addr), "%s", optarg);
			break;
		case 'n':
			n = atoi(optarg);
			if (n <= 0)
				n = 10;
			break;
		default:
			usage(argv[0]);
			return 0;
		}
	}

	memcache_session s(addr);

	char  name[32], value[32];

	printf(">>>>>>>>>>>>> set session <<<<<<<<<<<<<<<<<<<\r\n");
	for (int i = 0; i < n; i++)
	{
		snprintf(name, sizeof(name), "name-%d", i);
		snprintf(value, sizeof(value), "value-%d", i);
		if (s.set(name, value) == 0)
		{
			printf("set error, name: %s, value: %s\r\n", name, value);
			return 1;
		}
		printf("set ok, name: %s, value: %s\r\n", name, value);
	}

	printf("\r\n>>>>>>>>>>>>> get session <<<<<<<<<<<<<<<<<<<\r\n");
	for (int i = 0; i < n; i++)
	{
		snprintf(name, sizeof(name), "name-%d", i);
		snprintf(value, sizeof(value), "value-%d", i);
		const char* ptr = s.get(name);
		if (ptr == NULL || *ptr == 0 || strcmp(ptr, value) != 0)
		{
			printf("get error, name: %s\r\n", name);
			return 1;
		}
		printf("get ok, name: %s, value: %s\r\n", name, ptr);
	}

	printf("\r\n>>>>>>>>>>>>> del session <<<<<<<<<<<<<<<<<<<\r\n");
	for (int i = 0; i < n; i++)
	{
		snprintf(name, sizeof(name), "name-%d", i);
		if (s.del(name) == false)
		{
			printf("del error, name: %s\r\n", name);
			return 1;
		}
		printf("del ok, name: %s\r\n", name);
	}

	printf("\r\n>>>>>>>>>>>>> get session <<<<<<<<<<<<<<<<<<<\r\n");
	for (int i = 0; i < n; i++)
	{
		snprintf(name, sizeof(name), "name-%d", i);
		const char* ptr = s.get(name);
		if (ptr == NULL || *ptr == 0)
			printf("get ok, name: %s no exist\r\n", name);
		else
		{
			printf("get error, name: %s exist, value: %s\r\n",
				name, value);
			return 1;
		}
	}

	printf("\r\n------------ test session ok now -------------\r\n");
	return 0;
}
