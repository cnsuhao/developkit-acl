#include "lib_acl.h"
#include <stdio.h>
#include <stdlib.h>

static ACL_AIO *__aio;
static int __stop = 0;

static int __timeout_callback(ACL_ASTREAM *stream, void *context acl_unused)
{
	printf("timeout, sockfd=%d\r\n", ACL_VSTREAM_SOCK(acl_aio_vstream(stream)));
	__stop = 1;
	return (-1);
}

static int __close_callback(ACL_ASTREAM *stream, void *context)
{
	printf("close, sockfd=%d, %s\r\n", ACL_VSTREAM_SOCK(acl_aio_vstream(stream)), acl_last_serror());
	__stop = 1;
	return (-1);
}

static int __connect_callback(ACL_ASTREAM *stream, void *context acl_unused)
{
	printf("connected, sockfd=%d\r\n", ACL_VSTREAM_SOCK(acl_aio_vstream(stream)));
	__stop = 1;
	return (-1);
}

int main(int argc, char *argv[])
{
	const char *addr = "127.0.0.1:80";
	ACL_ASTREAM *stream;
	int   timeout = 10;

	acl_init();
	__aio = acl_aio_create(ACL_EVENT_SELECT);

	if (argc == 2)
		addr = argv[1];
	else if (argc == 3) {
		addr = argv[1];
		timeout = atoi(argv[2]);
	}

	stream = acl_aio_connect(__aio, addr, timeout);
	if (stream != NULL) {
		acl_aio_ctl(stream,
			ACL_AIO_CTL_TIMEO_HOOK_ADD, __timeout_callback, stream,
			ACL_AIO_CTL_CLOSE_HOOK_ADD, __close_callback, stream,
			ACL_AIO_CTL_CONNECT_HOOK_ADD, __connect_callback, stream,
			ACL_AIO_CTL_TIMEOUT, timeout,
			ACL_AIO_CTL_END);
	} else {
		printf("connect %s error(%s)\r\n", addr, acl_last_serror());
		__stop = 1;
	}

	while (!__stop) {
		acl_aio_loop(__aio);
	}

	printf("enter any key to exit ...\r\n");
	getchar();
	return (0);
}