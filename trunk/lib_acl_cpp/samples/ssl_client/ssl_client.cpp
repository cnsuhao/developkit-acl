// ssl_client.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "lib_acl.h"
#include <iostream>
#include "acl_cpp/acl_cpp_init.hpp"
#include "acl_cpp/http/http_header.hpp"
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/stream/socket_stream.hpp"
#include "acl_cpp/http/http_client.hpp"

static void test0(int i)
{
	acl::socket_stream client;
	acl::string addr("127.0.0.1:441");
	if (client.open(addr.c_str(), 60, 60) == false)
	{
		std::cout << "connect " << addr.c_str() << " error!" << std::endl;
		return;
	}
	if (client.open_ssl_client() == false)
	{
		std::cout << "open ssl " << addr.c_str() << " error!" << std::endl;
		return;
	}

	char line[1024];
	memset(line, 'x', sizeof(line));
	line[1023] = 0;
	line[1022] = '\n';
	if (client.write(line, strlen(line)) == -1)
	{
		std::cout << "write to " << addr.c_str() << " error!" << std::endl;
		return;
	}

	size_t n = sizeof(line);
	if (client.gets(line, &n) == false)
	{
		std::cout << "gets from " << addr.c_str() << " error!" << std::endl;
		return;
	}
	if (i < 10)
		std::cout << ">>gets: " << line << std::endl;
}

static void test1(void)
{
	acl::string url("/");
	acl::http_header header;
	header.set_url(url.c_str());
	header.set_host("mail.sina.com.cn:443");
	acl::string request;

	header.build_request(request);

	acl::string addr("mail.sina.com.cn:443");
	acl::socket_stream client;

	if (client.open(addr.c_str(), 60, 60) == false)
	{
		std::cout << "connect " << addr.c_str()
			<< " error!" << std::endl;
		return;
	}
	if (client.open_ssl_client() == false)
	{
		std::cout << "open ssl client " << addr.c_str()
			<< " error!" << std::endl;
		return;
	}

	std::cout << "request(len: " << request.length() << "):" << std::endl;
	std::cout << "----------------------------------------" << std::endl;
	std::cout << request.c_str();
	std::cout << "----------------------------------------" << std::endl;

	if (client.write(request) == false)
	{
		std::cout << "write to " << addr.c_str() <<
			" error!" << std::endl;
		return;
	}

	char  buf[8192];
	size_t size;
	int   ret;

	while (true)
	{
		size = sizeof(buf) - 1;
		if ((ret = client.read(buf, size, false)) == -1)
		{
			std::cout << "read over!" << std::endl;
			break;
		}
		 buf[ret] = 0;
		 std::cout << buf;
	}
}

static void test2(const acl::string& addr, bool gzip_enable, bool ssl_enable)
{
	acl::http_client client;
	acl::string url("/");
	acl::http_header header;

	header.set_url(url.c_str());
	header.set_host(addr);
	header.accept_gzip(gzip_enable);
	header.add_entry("User-Agent", "Mozilla/5.0 (Windows NT 6.1; WOW64; rv:31.0) Gecko/20100101 Firefox/31.0");
	header.add_entry("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
	header.add_entry("Accept-Languge", "zh-cn,en;q=0.5");
	header.add_entry("Pragma", "no-cache");
	header.add_entry("Cache-Control", "no-cache");

	acl::string request;

	header.build_request(request);

	if (client.open(addr.c_str(), 60, 60, gzip_enable, ssl_enable) == false)
	{
		std::cout << "connect " << addr.c_str()
			<< " error!" << std::endl;
		return;
	}

	std::cout << "request:" << std::endl;
	std::cout << "----------------------------------------" << std::endl;
	std::cout << request.c_str();
	std::cout << "----------------------------------------" << std::endl;

	if (client.get_ostream().write(request) == false)
	{
		std::cout << "write to " << addr.c_str()
			<< " error!" << std::endl;
		return;
	}

	if (client.read_head() == false)
	{
		std::cout << "read http respond header error!" << std::endl;
		return;
	}

	client.get_respond_head(&request);
	std::cout << "respond header:" << std::endl;
	std::cout << request.c_str();

	char  buf[8192];
	size_t size;
	int   ret;

	while (true)
	{
		size = sizeof(buf) - 1;
		if ((ret = client.read_body(buf, size)) <= 0)
		{
			std::cout << "read over!" << std::endl;
			break;
		}
		buf[ret] = 0;
		std::cout << buf;
	}
}

int main(int argc, char* argv[])
{
	(void) argc; (void) argv;
	acl::acl_cpp_init();

	int   n = 1;
	if (argc >= 2)
		n = atoi(argv[1]);
	if (n <= 0)
		n = 100;
	ACL_METER_TIME("---------- begin ----------");
	for (int i = 0; i < 0; i++)
		test0(i);
	ACL_METER_TIME("---------- end ----------");

	test1();

	ACL_METER_TIME("---------- begin ----------");
	test2("mail.qq.com:443", false, true);
	test2("mail.126.com:80", false, false);
	test2("mail.sina.com.cn:443", false, true);
	ACL_METER_TIME("---------- end ----------");

	printf("Over, enter any key to exit!\n");
	getchar();
	return (0);
}
