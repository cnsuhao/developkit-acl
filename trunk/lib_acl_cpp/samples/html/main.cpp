// xml.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include <sys/time.h>
#include "util.h"

static void get_html(const char* file)
{
	acl::string buf;
	if (acl::ifstream::load(file, &buf) == false)
	{
		printf("load %s error %s\r\n", file, acl::last_serror());
		return;
	}

	struct timeval begin;
	gettimeofday(&begin, NULL);

	acl::xml xml(buf.c_str());
	const acl::string& text = xml.getText();

	struct timeval end;
	gettimeofday(&end, NULL);

	double spent = util::stamp_sub(&end, &begin);
	printf("text:{%d}, spent: %.2f\r\n", (int) text.length(), spent);
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("usage: %s filepath\r\n", argv[0]);
		return 0;
	}

	get_html(argv[1]);
	return 0;
}
