#include "lib_acl.h"
#include "acl_cpp/lib_acl.hpp"

static void do_copy(const acl::string& from, const acl::string& to)
{

}

int main(int argc, char* argv[])
{
	int  ch;
	acl::string path_from, path_to;

	while ((ch = getopt(argc, argv, "hf:t:")))
	{
		switch (ch)
		{
		case 'h':
			usage(argv[0]);
			return 0;
		case 'f':
			path_from = optarg;
			break;
		case  't':
			path_to = optarg;
			break;
		default:
			break;
		}
	}

	printf("enter any key to exit\r\n");
	getchar();

	return (0);
}
