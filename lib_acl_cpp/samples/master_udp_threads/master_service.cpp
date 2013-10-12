#include "stdafx.h"
#include "master_service.h"

////////////////////////////////////////////////////////////////////////////////
// ����������

char *var_cfg_local_addr;
acl::master_str_tbl var_conf_str_tab[] = {
	{ "local_addr", "127.0.0.1:0", &var_cfg_local_addr },

	{ 0, 0, 0 }
};

int  var_cfg_use_threads;
acl::master_bool_tbl var_conf_bool_tab[] = {
	{ "use_threads", 0, &var_cfg_use_threads },

	{ 0, 0, 0 }
};

int  var_cfg_int;
acl::master_int_tbl var_conf_int_tab[] = {
	{ "int", 120, &var_cfg_int, 0, 0 },

	{ 0, 0 , 0 , 0, 0 }
};

long long int  var_cfg_int64;
acl::master_int64_tbl var_conf_int64_tab[] = {
	{ "int64", 120, &var_cfg_int64, 0, 0 },

	{ 0, 0 , 0 , 0, 0 }
};

////////////////////////////////////////////////////////////////////////////////

static __thread acl::socket_stream* __stream = NULL;

class echo_thread : public acl::thread
{
public:
	echo_thread(const char* peer, const char* data, size_t dlen)
	{
		snprintf(peer_addr_, sizeof(peer_addr_), "%s", peer);
		buf_ = (char*) acl_mymemdup(data, dlen);
		len_ = (int) dlen;
	}

	~echo_thread()
	{
		acl_myfree(buf_);
	}

protected:
	virtual void* run()
	{
		// ��д��Ҫ������Զ�����ӵ�ַ
		__stream->set_peer(peer_addr_);
		__stream->write(buf_, len_);

		// ��Ϊ�ö����Ƕ�̬����ģ�������Ҫ�ͷŵ�
		delete this;
		return NULL;
	}

private:
	char  peer_addr_[64];
	char* buf_;
	int   len_;
};

class mythread_pool : public acl::thread_pool
{
public:
	mythread_pool() {}
	~mythread_pool() {}

protected:
	virtual bool thread_on_init()
	{
		__stream = new acl::socket_stream;
		if (__stream->bind_udp(var_cfg_local_addr) == false)
		{
			logger_error("bind %s error %s",
				var_cfg_local_addr, acl::last_serror());
			delete __stream;
			__stream = NULL;
			return false;
		}

		return true;
	}

	virtual void thread_on_exit()
	{
		if (__stream)
		{
			delete __stream;
			__stream = NULL;
		}
	}
};

////////////////////////////////////////////////////////////////////////////////

static acl::thread_pool *__threads = NULL;

master_service::master_service()
{
}

master_service::~master_service()
{
}

void master_service::on_read(acl::socket_stream* stream)
{
	int   n;
	char  buf[4096];

	// ��Զ�����Ӷ�һ����¼
	if ((n = stream->read(buf, sizeof(buf), false)) == -1)
		return;

	if (0)
		logger("read from %s, %d bytes, local: %s",
			stream->get_peer(true), n, stream->get_local(true));

	// ������õ��߳�ģʽ����ֱ�ӻ�д
	if (__threads == NULL)
	{
		stream->write(buf, n);
		return;
	}

	// ���򣬲��ö��߳�ģʽ������д���񽻸��̳߳ش���

	echo_thread* thr = new echo_thread(stream->get_peer(true), buf, n);
	__threads->run(thr);
}

void master_service::proc_on_init()
{
	if (var_cfg_use_threads)
	{
		// �̳߳�ģʽ�£���Ҫ�����̳߳�

		__threads = new mythread_pool;
		__threads->set_limit(100);
		__threads->start();
	}
	else
		__threads = NULL;
}

void master_service::proc_on_exit()
{
	if (__threads)
	{
		// ֹͣ�̳߳ز�����

		__threads->stop();
		delete __threads;
	}
}
