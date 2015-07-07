// DnsGateway.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "configure.h"
#include "service_main.h"

static SERVICE *__service;

static char *__dns_ip = "221.239.90.234";
//static char *__dns_ip = "10.0.90.234";
static short __dns_port = 53;
static char *__conf_file = "dgate.cf";

static void init(void)
{
	acl_socket_init();
	conf_load(__conf_file);
	acl_msg_open("dgate.log", "dgate");
	__service = service_create("0.0.0.0", 53, __dns_ip, __dns_port);
	printf("dns_ip: %s, dns_port: %d\r\n", __dns_ip, __dns_port);
}

static void run(void)
{
	service_start(__service);
}

static void usage(const char *procname)
{
	printf("usage: %s -h [help] -i dns_ip -p dns_port\r\n", procname);
}

int main(int argc, char* argv[])
{
	char  ch;

	while ((ch = getopt(argc, argv, "hi:p:f:")) > 0) {
		switch (ch) {
		case 'h':
			usage(argv[0]);
			exit (0);
		case 'i':
			__dns_ip = acl_mystrdup(optarg);
			break;
		case 'p':
			__dns_port = atoi(optarg);
			break;
		case 'f':
			__conf_file = acl_mystrdup(optarg);
			break;
		default:
			break;
		}
	}

	init();
	run();

	while (1)
	{
		sleep(1);
	}
	return 0;
}

