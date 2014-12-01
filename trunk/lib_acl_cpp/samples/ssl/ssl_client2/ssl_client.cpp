// ssl_client.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include "lib_acl.h"
#include "acl_cpp/acl_cpp_init.hpp"
#include "acl_cpp/http/http_header.hpp"
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/stream/socket_stream.hpp"
#include "acl_cpp/stream/polarssl_io.hpp"
#include "acl_cpp/stream/polarssl_conf.hpp"
#include "acl_cpp/http/http_client.hpp"

static acl::polarssl_conf __ssl_conf;

static void test(const char* addr, int i)
{
	acl::socket_stream client;
	if (client.open(addr, 60, 60) == false)
	{
		std::cout << "connect " << addr << " error!" << std::endl;
		return;
	}

	acl::polarssl_io* ssl = new acl::polarssl_io(__ssl_conf, false);
	if (client.setup_hook(ssl) == ssl)
	{
		std::cout << "open ssl " << addr << " error!" << std::endl;
		ssl->destroy();
		return;
	}

	std::cout << "ssl handshake ok" << std::endl;

	char line[1024];
	memset(line, 'x', sizeof(line));
	line[1023] = 0;
	line[1022] = '\n';
	if (client.write(line, strlen(line)) == -1)
	{
		std::cout << "write to " << addr << " error!" << std::endl;
		return;
	}

	size_t n = sizeof(line);
	if (client.gets(line, &n) == false)
	{
		std::cout << "gets from " << addr << " error!"
			<< acl_last_serror() << std::endl;
		return;
	}
	if (i < 10)
		std::cout << ">>gets(" << n << "): " << line << std::endl;
}

static void usage(const char* procname)
{
	printf("usage: %s -h[help]\r\n"
		"-s server_addr[default: 127.0.0.1:9001]\r\n"
		"-n max_loop[default: 10]\r\n", procname);
}

int main(int argc, char* argv[])
{
	int   ch, max = 10;
	acl::string addr("127.0.0.1:9001");

	acl::acl_cpp_init();

	while ((ch = getopt(argc, argv, "hs:n:")) > 0)
	{
		switch (ch)
		{
		case 'h':
			usage(argv[0]);
			return 0;
		case 's':
			addr = optarg;
			break;
		case 'n':
			max = atoi(optarg);
			break;
		default:
			break;
		}
	}

	if (max <= 0)
		max = 100;

	for (int i = 0; i < max; i++)
		test(addr, i);

	printf("Over, enter any key to exit!\n");
	getchar();
	return (0);
}
