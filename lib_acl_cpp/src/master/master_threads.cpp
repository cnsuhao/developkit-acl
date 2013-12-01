#include "acl_stdafx.hpp"
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/stream/socket_stream.hpp"
#include "acl_cpp/event/event_timer.hpp"
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

master_threads::~master_threads(void)
{
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

//////////////////////////////////////////////////////////////////////////

static bool __stop = false;
static int  __count_limit = 1;
static int  __count = 0;
static acl_pthread_pool_t* __thread_pool = NULL;
static ACL_EVENT* __eventp = NULL;

static void close_sstreams(ACL_EVENT* event, std::vector<ACL_VSTREAM*>& streams)
{
	std::vector<ACL_VSTREAM*>::iterator it = streams.begin();
	for (; it != streams.end(); ++it)
	{
		acl_event_disable_readwrite(event, *it);
		acl_vstream_close(*it);
	}

	streams.clear();
}

bool master_threads::run_alone(const char* addrs, const char* path /* = NULL */,
	unsigned int count /* = 1 */, int threads_count /* = 1 */)
{
	// ÿ������ֻ����һ��ʵ��������
	acl_assert(has_called == false);
	has_called = true;
	daemon_mode_ = false;
	acl_assert(addrs && *addrs);

	__count_limit = count;

#ifdef WIN32
	acl_init();
#endif

	std::vector<ACL_VSTREAM*> sstreams;
	__eventp = acl_event_new_select_thr(1, 0);
	ACL_ARGV*  tokens = acl_argv_split(addrs, ";,| \t");
	ACL_ITER   iter;

	acl_foreach(iter, tokens)
	{
		const char* addr = (const char*) iter.data;
		ACL_VSTREAM* sstream = acl_vstream_listen(addr, 128);
		if (sstream == NULL)
		{
			logger_error("listen %s error(%s)",
				addr, acl_last_serror());
			acl_argv_free(tokens);
			close_sstreams(__eventp, sstreams);
			acl_event_free(__eventp);
			return false;
		}
		acl_event_enable_listen(__eventp, sstream, 0,
			listen_callback, sstream);
		sstreams.push_back(sstream);
	}

	acl_argv_free(tokens);

	// ��ʼ�����ò���
	conf_.load(path);

	service_pre_jail(NULL);
	service_init(NULL);

	if (threads_count > 1)
	{
		__thread_pool = acl_thread_pool_create(threads_count, 120);
		acl_pthread_pool_atinit(__thread_pool, thread_begin, NULL);
		acl_pthread_pool_atfree(__thread_pool, thread_finish, NULL);
	}
	else
		thread_init(NULL);

	while (!__stop)
		acl_event_loop(__eventp);

	if (__thread_pool)
		acl_pthread_pool_destroy(__thread_pool);
	else
		thread_exit(NULL);

	service_exit(NULL);

	// �����ڵ��� acl_event_free ǰ���� close_sstreams����Ϊ�ڹر�
	// ����������ʱ��Ȼ�ж� ACL_EVENT �����ʹ��
	close_sstreams(__eventp, sstreams);
	acl_event_free(__eventp);
	__eventp = NULL;

	return true;
}

void master_threads::listen_callback(int, ACL_EVENT*, ACL_VSTREAM* sstream, void*)
{
	ACL_VSTREAM* client = acl_vstream_accept(sstream, NULL, 0);

	if (client == NULL)
	{
		logger_error("accept error(%s)", acl_last_serror());
		__stop = true;
	}
	else if (__thread_pool != NULL)
	{
		acl_pthread_pool_add(__thread_pool, thread_run, client);
		__count++;
	}
	else
	{
		// ���̷߳�ʽ���д���
		run_once(client);
		__count++;
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

void master_threads::thread_run(void* arg)
{
	ACL_VSTREAM* client = (ACL_VSTREAM*) arg;
	run_once(client);
}

void master_threads::run_once(ACL_VSTREAM* client)
{
	if (service_on_accept(client) != 0)
		return;

	socket_stream* stream = (socket_stream*) client->context;
	acl_assert(stream);
	ACL_SOCKET fd = stream->sock_handle();
	int   timeout = stream->get_rw_timeout();

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
		if(acl_read_wait(fd, timeout > 0 ? timeout : -1) == 0)
			client->sys_read_ready = 1;
		else if (service_on_timeout(client, NULL) == 0)
			continue;
		else
		{
			service_on_close(client, NULL);
			// ɾ��������ʱ��ͬʱ�ر��׽���
			delete stream;
			break;
		}

		// ���������� 1 ʱ��ʾ client �Ѿ����ر���
		if (service_main(client, NULL) == 1)
			break;
	}

	if (__count_limit > 0 && __count >= __count_limit)
		__stop = true;
}

//////////////////////////////////////////////////////////////////////////

static void timer_callback(int, ACL_EVENT* event, void* ctx)
{
	event_timer* timer = (event_timer*) ctx;

	// ������ʱ���е����ж�ʱ����
	acl_int64 next_delay = timer->trigger();

	// �����ʱ���е�����Ϊ�ջ�δ���ö�ʱ�����ظ�ʹ�ã���ɾ����ʱ��
	if (timer->empty() || !timer->keep_timer())
	{
		// ɾ����ʱ��
		acl_event_cancel_timer(event, timer_callback, timer);
		timer->destroy();
		return;
	}

	// ��������ظ�ʹ�ö�ʱ���Ҷ�ʱ���е�����ǿգ����ٴ����øö�ʱ��

	//  ��Ҫ���ö�ʱ���ĵ���ʱ���
	acl_event_request_timer(event, timer_callback, timer,
		next_delay < 0 ? 0 : next_delay,
		timer->keep_timer() ? 1 : 0);
}

void master_threads::proc_set_timer(event_timer* timer)
{
	if (__eventp == NULL)
		logger_warn("event NULL!");
	else
		acl_event_request_timer(__eventp, timer_callback, timer,
			timer->min_delay(), timer->keep_timer() ? 1 : 0);
}

void master_threads::proc_del_timer(event_timer* timer)
{
	if (__eventp == NULL)
		logger_warn("event NULL!");
	else
		acl_event_cancel_timer(__eventp, timer_callback, timer);
}

void master_threads::proc_set_timer(void (*callback)(int, ACL_EVENT*, void*),
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

void master_threads::proc_del_timer(void (*callback)(int, ACL_EVENT*, void*),
	void* ctx)
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

#ifndef WIN32
	if (__eventp == NULL && __mt->daemon_mode())
		__eventp = acl_ioctl_server_event();
#endif
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
