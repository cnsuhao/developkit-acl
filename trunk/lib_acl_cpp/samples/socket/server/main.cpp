#include "acl_cpp/lib_acl.hpp"
#include <stdlib.h>
#include <stdio.h>

int main(void)
{
	acl::server_socket server;
	acl::string addr = "127.0.0.1:9001";

	if (server.open(addr) == false)
	{
		printf("open %s error\r\n", addr.c_str());
		return 1;
	}
	else
		printf("open %s ok\r\n", addr.c_str());

	while (true)
	{
		acl::socket_stream* client = server.accept();
		if (client == NULL)
		{
			printf("accept failed: %s\r\n", acl::last_serror());
			break;
		}

		if (client->write("hello world\r\n") == false)
		{
			printf("write error\r\n");
			delete client;
			continue;
		}

		delete client;
		printf("close client ok\r\n");
	}

	return (0);
}