#include "acl_stdafx.hpp"
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/stream/socket_stream.hpp"
#include "acl_cpp/master/master_threads.hpp"

namespace acl
{

	static master_threads* __mt = NULL;

	master_threads::master_threads(void)
	{
		// ȫ�־�̬����
		acl_assert(__mt == NULL);
		__mt = this;
	}

	static bool has_called = false;

	void master_threads::run_daemon(int argc, char** argv)
	{
#ifndef WIN32
		// ÿ������ֻ����һ��ʵ��������
		acl_assert(has_called == false);
		has_called = true;
		daemon_mode_ = true;

		// ���� acl ��������ܵĶ��߳�ģ��
		acl_ioctl_app_main(argc, argv, service_main, NULL,
			ACL_APP_CTL_ON_ACCEPT, service_on_accept,
			ACL_APP_CTL_ON_TIMEOUT, service_on_timeout,
			ACL_APP_CTL_ON_CLOSE, service_on_close,
			ACL_APP_CTL_PRE_JAIL, service_pre_jail,
			ACL_APP_CTL_INIT_FN, service_init,
			ACL_APP_CTL_EXIT_FN, service_exit,
			ACL_APP_CTL_THREAD_INIT, thread_init,
			ACL_APP_CTL_THREAD_EXIT, thread_exit,
			ACL_APP_CTL_CFG_BOOL, conf_.get_bool_cfg(),
			ACL_APP_CTL_CFG_INT64, conf_.get_int64_cfg(),
			ACL_APP_CTL_CFG_INT, conf_.get_int_cfg(),
			ACL_APP_CTL_CFG_STR, conf_.get_str_cfg(),
			ACL_APP_CTL_END);
#else
		logger_fatal("no support win32 yet!");
#endif
	}

	bool master_threads::run_alone(const char* addr, const char* path /* = NULL */,
		unsigned int count /* = 1 */, int threads_count /* = 1 */)
	{
		// ÿ������ֻ����һ��ʵ��������
		acl_assert(has_called == false);
		has_called = true;
		daemon_mode_ = false;
		acl_assert(addr && *addr);

#ifdef WIN32
		acl_init();
#endif

		ACL_VSTREAM* sstream = acl_vstream_listen(addr, 128);
		if (sstream == NULL)
		{
			logger_error("listen %s error(%s)",
				addr, acl_last_serror());
			return false;
		}

		// ��ʼ�����ò���
		conf_.load(path);

		service_pre_jail(NULL);
		service_init(NULL);

		if (count == 1)
		{
			thread_init(NULL);
			run_once(sstream);
		}
		else if (threads_count > 1)
			run_parallel(sstream, count, threads_count);
		else
		{
			thread_init(NULL);
			run_serial(sstream, count);
			thread_exit(NULL);
		}

		acl_vstream_close(sstream);
		service_exit(NULL);
		return true;
	}

	void master_threads::do_serivce(ACL_VSTREAM* client)
	{
		if (service_on_accept(client) == 0)
		{
			while (true)
			{
				// ���������� 1 ʱ��ʾ client �Ѿ����ر���
				if (service_main(client, NULL) == 1)
					break;
			}
		}
	}

	void master_threads::run_once(ACL_VSTREAM* sstream)
	{
		ACL_VSTREAM* client = acl_vstream_accept(sstream, NULL, 0);
		if (client == NULL)
			logger_error("accept error(%s)", acl_last_serror());
		else
			do_serivce(client);  // �ú����ڲ��Զ��ر�����
	}

	void master_threads::thread_run(void* arg)
	{
		ACL_VSTREAM* client = (ACL_VSTREAM*) arg;
		if (service_on_accept(client) != 0)
			return;
		while (true)
		{
			if (ACL_VSTREAM_BFRD_CNT(client) > 0)
			{
				// ���������� 1 ʱ��ʾ client �Ѿ����ر���
				if (service_main(client, NULL) == 1)
					break;
				continue;
			}

			// acl_read_wait �� timeout Ϊ -1 ʱ������ȫ����
			// �ȴ����������ݿɶ�����Ϊ 0 ʱ����������أ���
			// > 0 ʱ��ȴ����ָ����ʱʱ��
			if(acl_read_wait(ACL_VSTREAM_SOCK(client),
				client->rw_timeout > 0 ?
				client->rw_timeout : -1) == 0)
			{
				client->sys_read_ready = 1;
			}

			// ���������� 1 ʱ��ʾ client �Ѿ����ر���
			if (service_main(client, NULL) == 1)
				break;
		}
	}

	int master_threads::thread_begin(void* arg)
	{
		thread_init(arg);
		return 0;
	}

	void master_threads::thread_finish(void* arg)
	{
		thread_exit(arg);
	}

	void master_threads::run_parallel(ACL_VSTREAM* sstream,
		unsigned int count, int threads_count)
	{
		acl_assert(threads_count > 1);
		acl_pthread_pool_t* thrpool = acl_thread_pool_create(threads_count, 120);
		acl_pthread_pool_atinit(thrpool, thread_begin, NULL);
		acl_pthread_pool_atfree(thrpool, thread_finish, NULL);

		unsigned int i = 0;

		while (true)
		{
			ACL_VSTREAM* client = acl_vstream_accept(sstream, NULL, 0);
			if (client == NULL)
			{
				logger_error("accept error(%s)", acl_last_serror());
				break;
			}
			acl_pthread_pool_add(thrpool, thread_run, client);
			i++;
			if (count > 0 && i >= count)
				break;
		}
		acl_pthread_pool_destroy(thrpool);
	}

	void master_threads::run_serial(ACL_VSTREAM* sstream, unsigned int count)
	{
		unsigned int i = 0;
		while (true)
		{
			run_once(sstream);
			i++;
			if (count > 0 && i >= count)
				break;
		}
	}

	void master_threads::proc_set_timer(void (*callback)(int, void*),
		void* ctx, int delay)
	{
#ifdef WIN32
		logger_error("can't support on win32");
#else
		if (__mt->proc_inited_)
			acl_ioctl_server_request_timer(callback, ctx, delay);
		else
			logger_error("please call me in proc_on_init");
#endif
	}

	void master_threads::proc_del_timer(void (*callback)(int, void*), void* ctx)
	{
#ifdef WIN32
		logger_error("can't support on win32");
#else
		if (__mt->proc_inited_)
			acl_ioctl_server_cancel_timer(callback, ctx);
#endif
	}

	//////////////////////////////////////////////////////////////////////////

	void master_threads::service_pre_jail(void*)
	{
		acl_assert(__mt != NULL);
		__mt->proc_pre_jail();
	}

	void master_threads::service_init(void*)
	{
		acl_assert(__mt != NULL);
		__mt->proc_inited_ = true;
		__mt->proc_on_init();
	}

	void master_threads::service_exit(void*)
	{
		acl_assert(__mt != NULL);
		__mt->proc_on_exit();
	}

	void master_threads::thread_init(void*)
	{
		acl_assert(__mt != NULL);
		__mt->thread_on_init();
	}

	void master_threads::thread_exit(void*)
	{
		acl_assert(__mt != NULL);
		__mt->thread_on_exit();
	}

	int master_threads::service_main(ACL_VSTREAM *client, void*)
	{
		acl_assert(__mt != NULL);

		// client->context �� service_on_accept �б�����
		socket_stream* stream = (socket_stream*) client->context;
		if (stream == NULL)
			logger_fatal("client->context is null!");

		// ����������麯��ʵ�֣�������� true ��ʾ�ÿ�ܼ�����ظ�������
		// ������Ҫ�رո���
		if (__mt->thread_on_read(stream) == true)
			return 0;  // ������ӱ��ֳ�����
		else
		{
			__mt->thread_on_close(stream);
			delete stream;
			return 1;  // �������Ѿ��ر�
		}
	}

	int master_threads::service_on_accept(ACL_VSTREAM* client)
	{
		// client->context ��Ӧ��ռ��
		if (client->context != NULL)
			logger_fatal("client->context not null!");

		socket_stream* stream = NEW socket_stream();
		if (stream->open(client) == false)
		{
			logger_error("open stream error(%s)", acl_last_serror());
			delete stream;
			return -1;
		}
		// ���� client->context Ϊ������
		client->context = stream;

		acl_assert(__mt != NULL);

		if (__mt->thread_on_accept(stream) == false)
		{
			client->context = NULL;
			// �������������İ󶨹�ϵ���������Է�ֹ��ɾ��������ʱ
			// �����ر�������������Ϊ����������Ҫ�ڱ��������غ���
			// ����Զ��ر�
			(void) stream->unbind();
			// ɾ��������
			delete stream;
			// �ÿ�ܹر�������
			return -1;
		}
		return 0;
	}

	int master_threads::service_on_timeout(ACL_VSTREAM* client, void*)
	{
		socket_stream* stream = (socket_stream*) client->context;
		if (stream == NULL)
			logger_fatal("client->context is null!");

		acl_assert(__mt != NULL);
		return __mt->thread_on_timeout(stream) == true ? 0 : -1;
	}

	void master_threads::service_on_close(ACL_VSTREAM* client, void*)
	{
		socket_stream* stream = (socket_stream*) client->context;
		if (stream == NULL)
			logger_fatal("client->context is null!");

		acl_assert(__mt != NULL);
		__mt->thread_on_close(stream);
	}

}  // namespace acl
