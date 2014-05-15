#include "lib_acl.h"
#ifdef WIN32
#include <io.h>
#define access _access
#endif // WIN32
#include "acl_cpp/lib_acl.hpp"

static bool copy_file(acl::ifstream& in, const acl::string& to_path,
	const acl::string& to_filepath)
{
	if (in.fseek(0, SEEK_SET) < 0)
	{
		printf("fseek from file: %s error: %s\r\n",
			in.file_path(), acl::last_serror());
		return false;
	}

	if (access(to_path.c_str(), 0) != 0)
	{
		if (acl_make_dirs(to_path.c_str(), 0755) == -1)
		{
			printf("create dirs: %s error: %s\r\n",
				to_path.c_str(), acl::last_serror());
			return false;
		}
	}

	acl_int64 length = in.fsize();
	if (length < 0)
	{
		printf("get file(%s)'s size error: %s\r\n", in.file_path(),
			acl::last_serror());
		return false;
	}

	acl::ofstream out;

	if (out.open_trunc(to_filepath.c_str()) == false)
	{
		printf("ope_trunc file: %s error: %s\r\n",
			to_filepath.c_str(), acl::last_serror());
		return false;
	}

	char  buf[4096];
	int   ret;
	acl_int64 nread = 0;

	while (true)
	{
		ret = in.read(buf, sizeof(buf), false);
		if (ret == -1)
		{
			if (nread != length)
			{
				printf("read from file: %s error: %s\r\n",
					in.file_path(), acl::last_serror());
				return false;
			}
			break;
		}

		if (out.write(buf, ret) == -1)
		{
			printf("write to file: %s error: %s\r\n",
				to_filepath.c_str(), acl::last_serror());
			return false;
		}
	}

	return true;
}

static bool cmp_copy(acl::scan_dir& scan, const char* name,
	const acl::string& to_path)
{
	const char* rpath = scan.curr_path();
	if (rpath == NULL)
	{
		printf("get current path error: %s, file: %s",
			acl::last_serror(), name);
		return false;
	}

	acl::string from_filepath;
	from_filepath.format("%s/%s", rpath, name);

	acl::ifstream from_fp;
	if (from_fp.open_read(from_filepath.c_str()) == false)
	{
		printf("open source file: %s error: %s\r\n",
			from_filepath.c_str(), acl::last_serror());
		return false;
	}

	acl::string to_pathbuf;
	acl::string to_filepath;
	to_pathbuf.format("%s/%s", to_path, rpath);
	to_filepath.format("%s/%s/%s", to_path, rpath, name);

	acl::ifstream to_fp;
	if (to_fp.open_read(to_filepath.c_str()) == false)
		return copy_file(from_fp, to_pathbuf, to_filepath);


	acl_int64 length;
	if ((length = to_fp.fsize()) != from_fp.fsize())
	{
		to_fp.close();
		return copy_file(from_fp, to_pathbuf, to_filepath);
	}

	char from_buf[4096], to_buf[4096];
	int from_len, to_len;
	acl_int64 read_len = 0;

	while (true)
	{
		from_len = from_fp.read(from_buf, sizeof(from_buf), false);
		if (from_len == -1)
		{
			if (read_len == length)
				return true;

			printf("read from file(%s) error(%s),"
#ifdef WIN32
				"file size: %I64d read len: %I64d\r\n",
#else
				"file size: %lld, read len: %lld\r\n",
#endif
				from_fp.file_path(), acl::last_serror(),
				length, read_len);
			return false;
		}

		read_len += from_len;
		to_len = to_fp.read(to_buf, from_len, true);
		if (to_len == -1)
		{
			to_fp.close();
			return copy_file(from_fp, to_pathbuf, to_filepath);
		}

		if (memcmp(from_buf, to_buf, to_len) != 0)
		{
			to_fp.close();
			return copy_file(from_fp, to_pathbuf, from_filepath);
		}
	}
}

static bool check_dir(acl::scan_dir& scan, const char* name, const char* to)
{
	const char* rpath = scan.curr_path();
	if (rpath == false)
	{
		printf("get from's path error: %s, name: %s\r\n",
			acl::last_serror(), name);
		return false;
	}

	acl::string to_path;
	to_path.format("%s/%s/%s", to, rpath, name);

	if (access(to_path.c_str(), 0) == 0)
		return true;
	else
		return acl_make_dirs(to_path.c_str(), 0755) == -1
				? false : true;
}

static void do_copy(const acl::string& from, const acl::string& to)
{
	acl::scan_dir scan;
	if (scan.open(from.c_str()) == false)
	{
		printf("open path: %s error: %s\r\n",
			from.c_str(), acl::last_serror());
		return;
	}

	const char* name;
	bool is_file;
	while ((name = scan.next(false, &is_file)) != NULL)
	{
		if (is_file)
		{
			if (cmp_copy(scan, name, to) == false)
				break;
		}
		else if (check_dir(scan, name, to) == false)
			break;
	}
}

static void usage(const char* procname)
{
	printf("usage: %s -h [help] -f from_path -t to_path\r\n", procname);
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
