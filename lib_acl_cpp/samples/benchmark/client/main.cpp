#include "stdafx.h"
#include "util.h"

static void usage(const char* procname)
{
	printf("usage: %s -h [help]\r\n"
		"\t-s server_addr[127.0.0.1:8888]\r\n"
		"\t-n max_loop[10]\r\n"
		"\t-l package_length[100]\r\n", procname);
}

int main(int argc, char* argv[])
{
	acl::acl_cpp_init();

	acl::string addr("127.0.0.1:8888");
	int   ch, max = 10, len = 100;

	while ((ch = getopt(argc, argv, "hs:n:l:")) > 0 )
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
			if (max < 1)
				max = 1;
			break;
		case 'l':
			len = atoi(optarg);
			if (len <= 0)
				len = 10;
			break;
		default:
			usage(argv[0]);
			return 0;
		}
	}

	acl::string wbuf(len);
	for (int i = 0; i < len; i++)
		wbuf += 'X';
	wbuf += "\r\n";

	acl::socket_stream conn;
	if (conn.open(addr, 30, 30) == false)
	{
		printf("connect %s error\r\n", addr.c_str());
		return -1;
	}

	acl::string rbuf;
	struct timeval begin;
	gettimeofday(&begin, NULL);

	for (int i = 0; i < max; i++)
	{
		if (conn.write(wbuf) == -1)
		{
			printf("write to server error\r\n");
			break;
		}

		if (conn.gets(rbuf) == false)
		{
			printf("readline from server error\r\n");
			break;
		}

		if (i < 10)
			printf("readline: %s\r\n", rbuf.c_str());
		if (i % 1000 == 0)
		{
			char tmp[64];
			snprintf(tmp, sizeof(tmp), "total: %d, curr: %d", max, i);
			ACL_METER_TIME(tmp);
		}
	}

	struct timeval end;
	gettimeofday(&end, NULL);

	double n = util::stamp_sub(&end, &begin);
	printf("total get: %d, spent: %0.2f ms, speed: %0.2f\r\n",
		max, n, (max * 1000) /(n > 0 ? n : 1));

	return 0;
}
