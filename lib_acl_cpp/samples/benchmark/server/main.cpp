#include "stdafx.h"
#include "util.h"

static void usage(const char* procname)
{
	printf("usage: %s -h [help]\r\n"
		"\t-s server_addr[127.0.0.1:8888]\r\n", procname);
}

int main(int argc, char* argv[])
{
	acl::acl_cpp_init();

	acl::string addr("127.0.0.1:8888");
	int   ch;

	while ((ch = getopt(argc, argv, "hs:")) > 0 )
	{
		switch (ch)
		{
		case 'h':
			usage(argv[0]);
			return 0;
		case 's':
			addr = optarg;
			break;
		default:
			usage(argv[0]);
			return 0;
		}
	}

	acl::server_socket server;
	if (server.open(addr) == false)
	{
		printf("listen %s error\r\n", addr.c_str());
		return -1;
	}
	else
		printf("listen %s ok\r\n", addr.c_str());

	// 接收一个客户端连接
	acl::socket_stream* conn = server.accept();
	if (conn == NULL)
	{
		printf("accept error %s\r\n", acl::last_serror());
		return -1;
	}
	else
		printf("accept on client: %s\r\n", conn->get_peer());

	acl::string buf;
	int   i = 0;
	while (true)
	{
		if (conn->gets(buf, false) == false)
		{
			printf("readline from client over!\r\n");
			break;
		}

		if (conn->write(buf) == -1)
		{
			printf("write to client error\r\n");
			break;
		}

		if (++i < 10)
			printf("readline: %s", buf.c_str());
	}

	delete conn;
	return 0;
}
