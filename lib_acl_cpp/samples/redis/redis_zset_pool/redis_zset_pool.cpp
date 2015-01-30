#include "stdafx.h"

static bool test_zadd(acl::redis_zset& option, int i, const char* key,
	const char* big_data, size_t length, size_t base_length)
{
	// �������ݽ��зָ������ָ������ݿ����
	size_t nmember = length / base_length;
	if (length % base_length != 0)
		nmember++;

	// �����Ӷ����л��ͳһ���ڴ�ط�����󣬷���С�ڴ��
	acl::dbuf_pool* pool = option.get_client()->get_pool();
	// ��̬�������ݿ�ָ�������ڴ�
	const char** members = (const char**)
		pool->dbuf_alloc(nmember * sizeof(char*));
	// ��̬�������ݿ鳤�������ڴ�
	size_t* lens = (size_t*) pool->dbuf_alloc(nmember * sizeof(size_t));
	// ��̬�������ݿ��ֵ�����ڴ�
	double* scores = (double*) pool->dbuf_alloc(nmember * sizeof(double));

	// ���������зֳ�С���ݣ��������ݿ������У�ʹ�õ�����������Ϊ��ֵ
	size_t len;
	const char* ptr = big_data;
	for (size_t j = 0; j < nmember; j++)
	{
		members[j] = ptr;
		len = length > base_length ? base_length : length;
		lens[j] = len;
		scores[j] = j;

		// ʣ�����ݿ鳤��
		length -= len;
	}

	// Ҫ�� redis ���Ӷ�������ڴ���Э����װ��ʽ�������ڲ���װ����Э��ʱ����װ�ɴ��ڴ�
	option.get_client()->set_slice_request(true);

	// ��ʼ�� redis �������
	int ret = option.zadd(key, members, lens, scores, nmember);
	if (ret < 0)
	{
		printf("add key: %s error\r\n", key);
		return false;
	}
	else if (i < 10)
		printf("add ok, key: %s, ret: %d\r\n", key, ret);

	return true;
}

static bool test_zcard(acl::redis_zset& option, int i, const char* key)
{
	// ��Ϊ��Э�����ݱȽ�С����������װ��������ʱ���ز��÷�Ƭ��ʽ
	option.get_client()->set_slice_request(false);

	int ret = option.zcard(key);
	if (ret < 0)
	{
		printf("zcard key: %s error\r\n", key);
		return false;
	}
	else if (i < 10)
		printf("zcard ok, key: %s, count: %d\r\n", key, ret);

	return true;
}

static bool test_zrange(acl::redis_zset& option, int i, const char* key,
	const char* hmac)
{
	int start = 0, end = 100;
	std::vector<acl::string> result;

	// ������������Ƚ�С����������װ����Э��ʱ���ز��÷�Ƭ��ʽ
	option.get_client()->set_slice_request(false);

	int ret = option.zrange(key, start, end, result);
	if (ret <= 0)
		return false;

	// У���õ���������Ƭ�� MD5 ֵ���봫��Ľ��бȽ�
	acl::md5 md5;
	std::vector<acl::string>::const_iterator cit;
	for (cit = result.begin(); cit != result.end(); ++cit)
		md5.update((*cit).c_str(), (*cit).length());
	md5.finish();

	// ����ַ�����ʽ�� MD5 ֵ
	const char* ptr = md5.get_string();
	if (strcmp(ptr, hmac) != 0)
	{
		printf("md5 error, hmac: %s, %s, key: %s\r\n", hmac, ptr, key);
		return false;
	}
	else if (i < 10)
		printf("md5 ok, hmac: %s, %s, key: %s\r\n", hmac, ptr, key);

	return true;
}

/////////////////////////////////////////////////////////////////////////////

static acl::string __keypre("zset_key");
static size_t __base_length = 8192;  // ��׼���ݿ鳤��
static char* __big_data;
static size_t __big_data_length = 10240000;  // �����ݿ鳤�ȣ�Ĭ���� 10 MB
static char __hmac[33];

// ���߳��࣬ÿ���̶߳����� redis-server ֮�佨��һ������
class test_thread : public acl::thread
{
public:
	test_thread(acl::redis_pool& pool, const char* cmd, int n, int id)
		: pool_(pool), cmd_(cmd), n_(n), id_(id) {}

	~test_thread() {}

protected:
	virtual void* run()
	{
		bool ret;
		acl::redis_client* conn;
		acl::redis_zset option;
		acl::string key;

		for (int i = 0; i < n_; i++)
		{
			// ��ȫ���̳߳��л�ȡһ�� redis ���Ӷ���
			conn = (acl::redis_client*) pool_.peek();
			
			if (conn == NULL)
			{
				printf("peek redis_client failed\r\n");
				break;
			}

			// ÿ���߳�һ�� ID �ţ���Ϊ��ֵ��ɲ���
			key.format("%s_%d_%d", __keypre.c_str(), id_, i);

			// �� redis ���Ӷ����� redis ��������������а󶨹���
			option.set_client(conn);

			if (cmd_ == "zadd")
				ret = test_zadd(option, i, key.c_str(),
					__big_data, __big_data_length,
					__base_length);
			else if (cmd_ == "zcard")
				ret = test_zcard(option, i, key);
			else if (cmd_ == "zrange")
				ret = test_zrange(option, i, key, __hmac);
			else if (cmd_ != "all")
			{
				printf("unknown cmd: %s\r\n", cmd_.c_str());
				ret = false;
			}
			else if (test_zadd(option, i, key.c_str(),
					__big_data, __big_data_length,
					__base_length) == false
				|| test_zcard(option, i, key) == false
				|| test_zrange(option, i, key, __hmac) == false)
			{
				ret = false;
			}
			else
				ret = true;

			// �� redis ���Ӷ���黹�����ӳأ��Ƿ񱣳ָ����ӣ�ͨ���ж�
			// �������Ƿ�Ͽ�����
			pool_.put(conn, !conn->eof());

			if (ret == false)
				break;
		}

		return NULL;
	}

private:
	acl::redis_pool& pool_;
	acl::string cmd_;
	int n_;
	int id_;
};

static void init()
{
	acl::md5 md5;
	unsigned ch;

	__big_data = (char*) malloc(__big_data_length);
	for (size_t i = 0; i < __big_data_length; i++)
	{
		ch = (unsigned char) i % 255;
		__big_data[i] = (char) ch;
		md5.update(&ch, 1);
	}

	md5.finish();
	acl::safe_snprintf(__hmac, sizeof(__hmac), "%s", md5.get_string());
}

static void end()
{
	free(__big_data);
}

static void usage(const char* procname)
{
	printf("usage: %s -h[help]\r\n"
		"-s redis_addr[127.0.0.1:6379]\r\n"
		"-n count[default: 10]\r\n"
		"-C connect_timeout[default: 10]\r\n"
		"-I rw_timeout[default: 10]\r\n"
		"-c max_threads[default: 10]\r\n"
		"-l max_data_length\r\n"
		"-b base_length\r\n"
		"-a cmd\r\n",
		procname);
}

int main(int argc, char* argv[])
{
	int  ch, n = 1, conn_timeout = 10, rw_timeout = 10;
	int  max_threads = 10;
	acl::string addr("127.0.0.1:6379"), cmd;

	while ((ch = getopt(argc, argv, "hs:n:C:I:c:a:l:b:")) > 0)
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
			n = atoi(optarg);
			break;
		case 'C':
			conn_timeout = atoi(optarg);
			break;
		case 'I':
			rw_timeout = atoi(optarg);
			break;
		case 'c':
			max_threads = atoi(optarg);
			break;
		case 'a':
			cmd = optarg;
			break;
		case 'l':
			__big_data_length = (unsigned long) atol(optarg);
			break;
		case 'b':
			__base_length = (size_t) atol(optarg);
			break;
		default:
			break;
		}
	}

	acl::acl_cpp_init();

	init();

	acl::redis_pool pool(addr.c_str(), max_threads);
	pool.set_timeout(conn_timeout, rw_timeout);

	std::vector<test_thread*> threads;

	// ����һ���̣߳�ÿһ���߳��� redis-server ����һ������
	for (int i = 0; i < max_threads; i++)
	{
		test_thread* thread = new test_thread(pool, cmd.c_str(),
			n, i);
		threads.push_back(thread);
		// ȡ���̵߳ķ���ģʽ���Ա�����������̣߳��ȴ��߳��˳�
		thread->set_detachable(false);
		thread->start();
	}

	// ���������߳�
	std::vector<test_thread*>::iterator it = threads.begin();
	for (; it != threads.end(); ++it)
	{
		// �ȴ�ĳ���߳��˳�
		(*it)->wait();
		delete (*it);
	}

	end();

#ifdef WIN32
	printf("enter any key to exit\r\n");
	getchar();
#endif

	return 0;
}
