#include "acl_stdafx.hpp"
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/stream/aio_handle.hpp"
#include "acl_cpp/stream/aio_socket_stream.hpp"
#include "acl_cpp/master/master_aio.hpp"

namespace acl
{

	static master_aio* __ma = NULL;
	static aio_handle* __handle = NULL;
	static aio_listen_stream* __sstream = NULL;

	master_aio::master_aio()
	{
		// ȫ�־�̬����
		acl_assert(__ma == NULL);
		__ma = this;
	}

	master_aio::~master_aio()
	{
		if (__sstream)
		{
			__sstream->close();
			__sstream = NULL;
		}

		if (__handle)
		{
			__handle->check();
			if (daemon_mode_ == false)
				delete __handle;
			__handle = NULL;
		}
	}

	aio_handle* master_aio::get_handle() const
	{
		acl_assert(__handle);
		return __handle;
	}

	static bool has_called = false;

	void master_aio::run_daemon(int argc, char** argv)
	{
#ifndef WIN32
		// ÿ������ֻ����һ��ʵ��������
		acl_assert(has_called == false);
		has_called = true;
		daemon_mode_ = true;

		// ���� acl ��������ܵĵ��̷߳�����ģ��
		acl_aio_app2_main(argc, argv, service_main, NULL,
			ACL_APP_CTL_PRE_JAIL, service_pre_jail,
			ACL_APP_CTL_INIT_FN, service_init,
			ACL_APP_CTL_EXIT_FN, service_exit,
			ACL_APP_CTL_CFG_BOOL, conf_.get_bool_cfg(),
			ACL_APP_CTL_CFG_INT64, conf_.get_int64_cfg(),
			ACL_APP_CTL_CFG_INT, conf_.get_int_cfg(),
			ACL_APP_CTL_CFG_STR, conf_.get_str_cfg(),
			ACL_APP_CTL_END);
#else
		logger_fatal("no support win32 yet!");
#endif
	}

	bool master_aio::run_alone(const char* addr, const char* path /* = NULL */,
		aio_handle_type ht /* = ENGINE_SELECT */)
	{
		acl_assert(__handle == NULL);
		daemon_mode_ = false;
#ifdef WIN32
		acl_init();
#endif

		// ��ʼ�����ò���
		conf_.load(path);

		__handle = NEW aio_handle(ht);
		__sstream = NEW aio_listen_stream(__handle);
		// ����ָ���ĵ�ַ
		if (__sstream->open(addr) == false)
		{
			logger_error("listen %s error: %s", addr, acl_last_serror());
			__sstream->close();
			// XXX: Ϊ�˱�֤�ܹرռ�������Ӧ�ڴ˴��� check һ��
			__handle->check();
			__sstream = NULL;
			return (false);
		}
		__sstream->add_accept_callback(this);

		service_pre_jail(NULL);
		service_init(NULL);
		while (true)
		{
			// ������� false ���ʾ���ټ�������Ҫ�˳�
			if (__handle->check() == false)
			{
				logger("aio_server stop now ...");
				break;
			}
		}
		__sstream->close();
		__sstream = NULL;
		__handle->check();
		service_exit(NULL);
		return true;
	}

	void master_aio::stop()
	{
		acl_assert(__handle);
		__handle->stop();
	}

	bool master_aio::accept_callback(aio_socket_stream* client)
	{
		return on_accept(client);
	}

	//////////////////////////////////////////////////////////////////////////

	void master_aio::service_pre_jail(void*)
	{
		acl_assert(__ma != NULL);
#ifndef WIN32
		if (__ma->daemon_mode_)
		{
			acl_assert(__handle == NULL);

			ACL_AIO *aio = acl_aio_server_handle();
			acl_assert(aio);
			__handle = NEW aio_handle(aio);
		}
#endif
		__ma->proc_pre_jail();
	}

	void master_aio::service_init(void* ctx acl_unused)
	{
		acl_assert(__ma != NULL);
		__ma->proc_inited_ = true;
		__ma->proc_on_init();
	}

	void master_aio::service_exit(void* ctx acl_unused)
	{
		acl_assert(__ma != NULL);
		__ma->proc_on_exit();
	}

	int master_aio::service_main(ACL_SOCKET fd, void*)
	{
		acl_assert(__handle);
		acl_assert(__ma);

		aio_socket_stream* stream = NEW aio_socket_stream(__handle, fd);
		__ma->on_accept(stream);
		return 0;
	}

}
