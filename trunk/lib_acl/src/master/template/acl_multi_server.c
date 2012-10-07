/* System library. */
#include "StdAfx.h"
#ifndef ACL_PREPARE_COMPILE

#include "stdlib/acl_define.h"

#ifdef ACL_BCB_COMPILER
#pragma hdrstop
#endif

#endif

#ifdef ACL_UNIX

#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#ifdef STRCASECMP_IN_STRINGS_H
#include <strings.h>
#endif
#include <time.h>

/* Utility library. */

#include "stdlib/acl_msg.h"
#include "stdlib/unix/acl_chroot_uid.h"
#include "stdlib/acl_vstring.h"
#include "stdlib/acl_vstream.h"
#include "stdlib/acl_mymalloc.h"
#include "stdlib/acl_iostuff.h"
#include "stdlib/acl_stringops.h"
#include "stdlib/acl_myflock.h"
#include "stdlib/unix/acl_watchdog.h"
#include "stdlib/acl_split_at.h"
#include "net/acl_listen.h"
#include "event/acl_events.h"

/* Global library. */

#include "../master_flow.h"
#include "../master_params.h"
#include "../master_proto.h"

/* Application-specific */
#include "master/acl_multi_params.h"
#include "master/acl_server_api.h"
#include "template.h"

int   acl_var_multi_pid;
char *acl_var_multi_procname;
char *acl_var_multi_log_file;

int   acl_var_multi_buf_size;
int   acl_var_multi_rw_timeout;
int   acl_var_multi_in_flow_delay;
int   acl_var_multi_idle_limit;
char *acl_var_multi_queue_dir;
char *acl_var_multi_owner;
int   acl_var_multi_delay_sec;
int   acl_var_multi_delay_usec;
int   acl_var_multi_daemon_timeout;
int   acl_var_multi_use_limit;
char *acl_var_multi_pid_dir;
int   acl_var_multi_enable_core;

static ACL_CONFIG_INT_TABLE __conf_int_tab[] = {
        { ACL_VAR_MULTI_BUF_SIZE, ACL_DEF_MULTI_BUF_SIZE, &acl_var_multi_buf_size, 0, 0 },
        { ACL_VAR_MULTI_RW_TIMEOUT, ACL_DEF_MULTI_RW_TIMEOUT, &acl_var_multi_rw_timeout, 0, 0 },
        { ACL_VAR_MULTI_IN_FLOW_DELAY, ACL_DEF_MULTI_IN_FLOW_DELAY, &acl_var_multi_in_flow_delay, 0, 0 },
        { ACL_VAR_MULTI_IDLE_LIMIT, ACL_DEF_MULTI_IDLE_LIMIT, &acl_var_multi_idle_limit, 0, 0 },
        { ACL_VAR_MULTI_DELAY_SEC, ACL_DEF_MULTI_DELAY_SEC, &acl_var_multi_delay_sec, 0, 0 },
        { ACL_VAR_MULTI_DELAY_USEC, ACL_DEF_MULTI_DELAY_USEC, &acl_var_multi_delay_usec, 0, 0 },
        { ACL_VAR_MULTI_DAEMON_TIMEOUT, ACL_DEF_MULTI_DAEMON_TIMEOUT, &acl_var_multi_daemon_timeout, 0, 0 },
        { ACL_VAR_MULTI_USE_LIMIT, ACL_DEF_MULTI_USE_LIMIT, &acl_var_multi_use_limit, 0, 0 },
	{ ACL_VAR_MULTI_ENABLE_CORE, ACL_DEF_MULTI_ENABLE_CORE, &acl_var_multi_enable_core, 0, 0 },
        { 0, 0, 0, 0, 0 },
};

static ACL_CONFIG_STR_TABLE __conf_str_tab[] = {
        { ACL_VAR_MULTI_QUEUE_DIR, ACL_DEF_MULTI_QUEUE_DIR, &acl_var_multi_queue_dir },
        { ACL_VAR_MULTI_OWNER, ACL_DEF_MULTI_OWNER, &acl_var_multi_owner },
	{ ACL_VAR_MULTI_PID_DIR, ACL_DEF_MULTI_PID_DIR, &acl_var_multi_pid_dir },
        { 0, 0, 0 },
};

 /*
  * Global state.
  */
static int client_count;
static int use_count;
static int socket_count = 1;
static int listen_disabled = 0;

static ACL_EVENT *__eventp = NULL;
static ACL_VSTREAM **__listen_streams = NULL;

static void (*multi_server_service) (ACL_VSTREAM *, char *, char **);
static char *multi_server_name;
static char **multi_server_argv;
static void (*multi_server_accept) (int, void *);
static void (*multi_server_onexit) (char *, char **);
static void (*multi_server_pre_accept) (char *, char **);
static ACL_VSTREAM *multi_server_lock;
static int multi_server_in_flow_delay;
static unsigned multi_server_generation;
static void (*multi_server_pre_disconn) (ACL_VSTREAM *, char *, char **);
static int (*multi_server_on_accept)(ACL_VSTREAM *);

/* add by zsx for rw timeout, 2005.9.25*/
static void (*multi_server_rw_timer) (ACL_VSTREAM *);

/* forward declare */
static void multi_server_timeout(int event, void *context);

ACL_EVENT *acl_multi_server_event()
{
	return (__eventp);
}

/* add by zsx for rw timeout, 2005.9.25*/
void acl_multi_server_request_rw_timer(ACL_VSTREAM *stream)
{
	char  myname[] = "acl_multi_server_request_rw_timer";

	if (stream == NULL)
		acl_msg_fatal("%s(%d)->%s: input error",
				__FILE__, __LINE__, myname);
	if (__eventp == NULL)
		acl_msg_fatal("%s(%d)->%s: event has not been inited",
				__FILE__, __LINE__, myname);
}

/* add by zsx for rw timeout, 2005.9.25*/
void acl_multi_server_cancel_rw_timer(ACL_VSTREAM *stream)
{
	char  myname[] = "acl_multi_server_cancel_rw_timer";

	if (stream == NULL)
		acl_msg_fatal("%s(%d)->%s: input error",
				__FILE__, __LINE__, myname);
	if (__eventp == NULL)
		acl_msg_fatal("%s(%d)->%s: event has not been inited",
				__FILE__, __LINE__, myname);
}

static void disable_listen(void)
{
	int   i;

	if (__listen_streams == NULL)
		return;

	for (i = 0; __listen_streams[i] != NULL; i++) {
		acl_event_disable_readwrite(__eventp, __listen_streams[i]);
		acl_vstream_close(__listen_streams[i]);
		__listen_streams[i] = NULL;
	}
	acl_myfree(__listen_streams);
	__listen_streams = NULL;
}

/* multi_server_exit - normal termination */

static void multi_server_exit(void)
{
	if (multi_server_onexit)
		multi_server_onexit(multi_server_name, multi_server_argv);
	exit(0);
}

/* multi_server_abort - terminate after abnormal master exit */

static void multi_server_abort(int event acl_unused, void *context acl_unused)
{
	if (!listen_disabled)
		listen_disabled = 1;

	if (client_count > 0) {
		/* set idle timeout to 1 second */
		acl_var_multi_idle_limit = 1;
		acl_event_request_timer(__eventp,
			multi_server_timeout,
			(void *) 0,
			(acl_int64) acl_var_multi_idle_limit * 1000000, 0);
		return;
	}

	if (acl_msg_verbose)
		acl_msg_info("master disconnect -- exiting");
	multi_server_exit();
}

/* multi_server_timeout - idle time exceeded */

static void multi_server_timeout(int event acl_unused, void *context acl_unused)
{
	if (client_count > 0) {
		/* set idle timeout to 1 second */
		acl_var_multi_idle_limit = 1;
		acl_event_request_timer(__eventp,
			multi_server_timeout,
			(void *) 0,
			(acl_int64) acl_var_multi_idle_limit * 1000000, 0);
		return;
	}

	if (acl_msg_verbose)
		acl_msg_info("idle timeout -- exiting");
	multi_server_exit();
}

/*  acl_multi_server_drain - stop accepting new clients */

int acl_multi_server_drain(void)
{
	int     i;

	switch (fork()) {
		/* Try again later. */
	case -1:
		return (-1);
		/* Finish existing clients in the background, then terminate. */
	case 0:
		if (__listen_streams == NULL)
			acl_msg_fatal("%s(%d): __listen_streams null",
				__FILE__, __LINE__);
		
		for (i = 0; __listen_streams[i] != NULL; i++) {
			acl_event_disable_readwrite(__eventp, __listen_streams[i]);
			acl_vstream_close(__listen_streams[i]);
			__listen_streams[i] = NULL;
		}
		acl_var_multi_use_limit = 1;
		return (0);
		/* Let the master start a new process. */
	default:
		exit(0);
	}
}

/* acl_multi_server_disconnect - terminate client session */

void    acl_multi_server_disconnect(ACL_VSTREAM *stream)
{
	if (acl_msg_verbose)
		acl_msg_info("connection closed fd %d", ACL_VSTREAM_SOCK(stream));
	if (multi_server_pre_disconn)
		multi_server_pre_disconn(stream,
					multi_server_name,
					multi_server_argv);
	acl_event_disable_readwrite(__eventp, stream);
	(void) acl_vstream_fclose(stream);
	client_count--;
}

/* multi_server_execute - in case (char *) != (struct *) */

static void multi_server_execute(int event, void *context)
{
	char  myname[] = "multi_server_execute";
	ACL_VSTREAM *stream = (ACL_VSTREAM *) context;

	if (multi_server_lock != 0
	    && acl_myflock(ACL_VSTREAM_FILE(multi_server_lock),
		    	ACL_INTERNAL_LOCK,
			ACL_MYFLOCK_OP_NONE) < 0)
		acl_msg_fatal("%s(%d)->%s: select unlock: %s",
				__FILE__, __LINE__, myname, strerror(errno));

	/*
	 * Do not bother the application when the client disconnected.
	 */
	if (acl_vstream_peekfd(stream) > 0) {
		if (acl_master_notify(acl_var_multi_pid,
				multi_server_generation,
				ACL_MASTER_STAT_TAKEN) < 0)
			
			multi_server_abort(ACL_EVENT_NULL_TYPE,
					ACL_EVENT_NULL_CONTEXT);
		multi_server_service(stream,
					multi_server_name,
					multi_server_argv);
		if (acl_master_notify(acl_var_multi_pid,
					multi_server_generation,
					ACL_MASTER_STAT_AVAIL) < 0)
	
			multi_server_abort(ACL_EVENT_NULL_TYPE,
						ACL_EVENT_NULL_CONTEXT);
	} else {
		/* add by zsx for rw timeout, 2005.9.25*/
		if (event == ACL_EVENT_RW_TIMEOUT && multi_server_rw_timer)
			multi_server_rw_timer(stream);
		else
			acl_multi_server_disconnect(stream);
	}
	if (client_count == 0 && acl_var_multi_idle_limit > 0)
		acl_event_request_timer(__eventp,
			multi_server_timeout,
			(void *) 0,
			(acl_int64) acl_var_multi_idle_limit * 1000000, 0);
}

/* multi_server_enable_read - enable read events */

static void __multi_server_enable_read(int event acl_unused, void *context)
{
	ACL_VSTREAM *stream = (ACL_VSTREAM *) context;
	int   ret;

	if (multi_server_on_accept != NULL) {
		ret = multi_server_on_accept(stream);
		if (ret < 0) {
			acl_multi_server_disconnect(stream);
			return;
		}
	}

	use_count++;

	acl_event_enable_read(__eventp,
				stream,
				acl_var_multi_rw_timeout,
				multi_server_execute,
				(void *) stream);
}

void acl_multi_server_enable_read(ACL_VSTREAM *stream)
{
	char  myname[] = "acl_multi_server_enable_read";

	if (stream == NULL || ACL_VSTREAM_SOCK(stream) < 0) {
		acl_msg_error("%s(%d)->%s: input error",
				__FILE__, __LINE__, myname);
		return;
	} else {
		client_count++;
		acl_event_enable_read(__eventp,
				stream,
				acl_var_multi_rw_timeout,
				multi_server_execute,
				(void *) stream);
	}
}

/* multi_server_wakeup - wake up application */

static void multi_server_wakeup(int fd)
{
	ACL_VSTREAM *stream;

	if (acl_msg_verbose)
		acl_msg_info("connection established fd %d", fd);
	acl_non_blocking(fd, ACL_BLOCKING);
	acl_close_on_exec(fd, ACL_CLOSE_ON_EXEC);
	client_count++;
	stream = acl_vstream_fdopen(fd,
				O_RDWR,
				acl_var_multi_buf_size,
				acl_var_multi_rw_timeout,
				ACL_VSTREAM_TYPE_SOCK);
	if (multi_server_in_flow_delay && acl_master_flow_get(1) < 0)
		acl_event_request_timer(__eventp,
			__multi_server_enable_read,
			(void *) stream,
			(acl_int64) acl_var_multi_in_flow_delay * 1000000, 0);
	else
		__multi_server_enable_read(0, (void *) stream);
}

/* multi_server_accept_local - accept client connection request */

static void multi_server_accept_local(int event acl_unused, void *context)
{
	ACL_VSTREAM *stream = (ACL_VSTREAM *) context;
	int     listen_fd = ACL_VSTREAM_SOCK(stream);
	int     time_left = -1;
	int     fd;

	/*
	 * Be prepared for accept() to fail because some other process already
	 * got the connection (the number of processes competing for clients
	 * is kept small, so this is not a "thundering herd" problem). If the
	 * accept() succeeds, be sure to disable non-blocking I/O, in order to
	 * minimize confusion.
	 */
	if (client_count == 0 && acl_var_multi_idle_limit > 0)
		time_left = (int) ((acl_event_cancel_timer(__eventp,
					multi_server_timeout,
					(void *) 0) + 999999)/ 1000000);

	if (multi_server_pre_accept)
		multi_server_pre_accept(multi_server_name,
					multi_server_argv);
	fd = acl_unix_accept(listen_fd);
	if (multi_server_lock != 0
	    && acl_myflock(ACL_VSTREAM_FILE(multi_server_lock),
		    	ACL_INTERNAL_LOCK,
			ACL_MYFLOCK_OP_NONE) < 0)
		acl_msg_fatal("select unlock: %s", strerror(errno));
	if (fd < 0) {
		if (errno != EAGAIN)
			acl_msg_fatal("accept connection: %s", strerror(errno));
		if (time_left >= 0)
			acl_event_request_timer(__eventp,
				multi_server_timeout,
				(void *) 0,
				(acl_int64) time_left * 1000000, 0);
		return;
	}
	multi_server_wakeup(fd);
}

#ifdef MASTER_XPORT_NAME_PASS

/* multi_server_accept_pass - accept descriptor */

static void multi_server_accept_pass(int event acl_unused, void *context)
{
	ACL_VSTREAM *stream = (ACL_VSTREAM *) context;
	int     listen_fd = acl_vstream_fileno(stream);
	int     time_left = -1;
	int     fd;

	/*
	 * Be prepared for accept() to fail because some other process already
	 * got the connection (the number of processes competing for clients
	 * is kept small, so this is not a "thundering herd" problem). If the
	 * accept() succeeds, be sure to disable non-blocking I/O, in order to
	 * minimize confusion.
	 */
	if (client_count == 0 && acl_var_multi_idle_limit > 0)
		time_left = (int) ((acl_event_cancel_timer(__evenpt,
					multi_server_timeout,
					(void *) 0) + 999999) / 1000000);

	if (multi_server_pre_accept)
		multi_server_pre_accept(multi_server_name, multi_server_argv);
	fd = PASS_ACCEPT(listen_fd);
	if (multi_server_lock != 0
	    && acl_myflock(ACL_VSTREAM_FILE(multi_server_lock),
		    	ACL_INTERNAL_LOCK,
			ACL_MYFLOCK_OP_NONE) < 0)
		acl_msg_fatal("select unlock: %s", strerror(errno));
	if (fd < 0) {
		if (errno != EAGAIN)
			acl_msg_fatal("accept connection: %s", strerror(errno));
		if (time_left >= 0)
			acl_event_request_timer(__eventp,
				multi_server_timeout,
				(void *) 0,
				(acl_int64) time_left * 1000000, 0);
		return;
	}
	multi_server_wakeup(fd);
}

#endif

/* multi_server_accept_inet - accept client connection request */

static void multi_server_accept_inet(int event acl_unused, void *context)
{
	ACL_VSTREAM	*stream = (ACL_VSTREAM *) context;
	int     listen_fd = ACL_VSTREAM_SOCK(stream);
	int     time_left = -1;
	int     fd;

	/*
	 * Be prepared for accept() to fail because some other process already
	 * got the connection (the number of processes competing for clients
	 * is kept small, so this is not a "thundering herd" problem). If the
	 * accept() succeeds, be sure to disable non-blocking I/O, in order to
	 * minimize confusion.
	 */
	if (client_count == 0 && acl_var_multi_idle_limit > 0)
		time_left = (int) ((acl_event_cancel_timer(__eventp,
					multi_server_timeout,
					(void *) 0) + 9999999) / 1000000);

	if (multi_server_pre_accept)
		multi_server_pre_accept(multi_server_name, multi_server_argv);
	fd = acl_inet_accept(listen_fd);
	if (multi_server_lock != 0
	    && acl_myflock(ACL_VSTREAM_FILE(multi_server_lock),
		  	  	ACL_INTERNAL_LOCK,
				ACL_MYFLOCK_OP_NONE) < 0)
		acl_msg_fatal("select unlock: %s", strerror(errno));
	if (fd < 0) {
		if (errno != EAGAIN)
			acl_msg_fatal("accept connection: %s", strerror(errno));
		if (time_left >= 0)
			acl_event_request_timer(__eventp,
				multi_server_timeout,
				(void *) 0,
				(acl_int64) time_left * 1000000, 0);
		return;
	}
	/* ���ⷢ���ӳ����� */
	acl_tcp_set_nodelay(fd);
	multi_server_wakeup(fd);
}

static void multi_server_init(const char *procname)
{
	const char *myname = "multi_server_init";
	static int  inited = 0;

	if (inited)
		return;

	inited = 1;

	if (procname == NULL || *procname == 0)
		acl_msg_fatal("%s(%d); procname null", myname, __LINE__);

	/*
	 * Don't die when a process goes away unexpectedly.
	 */
	signal(SIGPIPE, SIG_IGN);

	/*
	 * Don't die for frivolous reasons.
	 */
#ifdef SIGXFSZ
	signal(SIGXFSZ, SIG_IGN);
#endif

	/*
	 * May need this every now and then.
	 */
	acl_var_multi_pid = getpid();
	acl_var_multi_procname = acl_mystrdup(acl_safe_basename(procname));

	acl_var_multi_log_file = getenv("SERVICE_LOG");
	if (acl_var_multi_log_file == NULL) {
		acl_var_multi_log_file = acl_mystrdup("acl_master.log");
		acl_msg_warn("%s(%d)->%s: can't get MASTER_LOG's env value, use %s log",
				__FILE__, __LINE__, myname, acl_var_multi_log_file);
	}

	acl_get_app_conf_int_table(__conf_int_tab);
	acl_get_app_conf_str_table(__conf_str_tab);

	acl_master_vars_init(acl_var_multi_buf_size, acl_var_multi_rw_timeout);
}

static void multi_server_open_log(void)
{
	/* first, close the master's log */
	master_log_close();

	/* second, open the service's log */
	acl_msg_open(acl_var_multi_log_file, acl_var_multi_procname);
}

static void usage(int argc, char *argv[])
{
	int   i;
	char *service_name;

	if (argc <= 0)
		acl_msg_fatal("%s(%d): argc(%d) invalid", __FILE__, __LINE__, argc);

	service_name = acl_mystrdup(acl_safe_basename(argv[0]));

	for (i = 0; i < argc; i++) {
		acl_msg_info("argv[%d]: %s", i, argv[i]);
	}

	acl_msg_info("usage: %s -h[help]"
		" -c [use chroot]"
		" -d [debug]"
		" -l [run alone]"
		" -n service_name"
		" -s socket_count"
		" -i [use stdin]"
		" -t transport"
		" -u [use setgid initgroups setuid]"
		" -v [on acl_msg_verbose]"
		" -z [unlimit process count]"
		" -f conf_file",
		service_name);
}

/* acl_multi_server_main - the real main program */

void acl_multi_server_main(int argc, char **argv, ACL_MULTI_SERVER_FN service,...)
{
	char   myname[] = "acl_multi_server_main";
	ACL_VSTREAM *stream = 0;
	char   *root_dir = 0;
	char   *user_name = 0;
	int     debug_me = 0;
	char   *service_name = acl_mystrdup(acl_safe_basename(argv[0]));
	int     c;
	va_list ap;
	ACL_MASTER_SERVER_INIT_FN pre_init = 0;
	ACL_MASTER_SERVER_INIT_FN post_init = 0;
	ACL_MASTER_SERVER_LOOP_FN loop = 0;
	int     key;
	char   *transport = 0;
	int     alone = 0;
	int     zerolimit = 0;
	ACL_WATCHDOG *watchdog;
	char   *generation;
	int     fd, i, fdtype = 0;

	int     f_flag = 0;
	char   *conf_file_ptr = 0;

	if (acl_msg_verbose)
		acl_msg_info("%s(%d): daemon started, log = %s",
			acl_var_multi_procname, __LINE__, acl_var_multi_log_file);

	/*
	 * Pick up policy settings from master process. Shut up error messages to
	 * stderr, because no-one is going to see them.
	 */
	opterr = 0;
	while ((c = getopt(argc, argv, "hcdlm:n:s:it:uvzf:")) > 0) {
		switch (c) {
		case 'h':
			usage(argc, argv);
			exit (0);
		case 'f':
			acl_app_conf_load(optarg);
			f_flag = 1;
			conf_file_ptr = optarg;
			break;
		case 'c':
			root_dir = "setme";
			break;
		case 'd':
			debug_me = 1;
			break;
		case 'l':
			alone = 1;
			break;
		case 'n':
			service_name = optarg;
			break;
		case 's':
			if ((socket_count = atoi(optarg)) <= 0)
				acl_msg_fatal("invalid socket_count: %s", optarg);
			break;
		case 'i':
			stream = ACL_VSTREAM_IN;
			break;
		case 'u':
			user_name = "setme";
			break;
		case 't':
			transport = optarg;
			break;
		case 'v':
			acl_msg_verbose++;
			break;
		case 'z':
			zerolimit = 1;
			break;
		default:
#if 0
			usage(argc, argv);
			acl_msg_fatal("%s(%d)->%s: invalid option: %s",
				__FILE__, __LINE__, myname,
				optind > 0 ? argv[optind - 1] : "unknown");
#endif
			break;
		}
	}

	if (stream == NULL)
		multi_server_init(argv[0]);

	if (f_flag == 0)
		acl_msg_fatal("%s(%d)->%s: need \"-f pathname\"",
				__FILE__, __LINE__, myname);
	else if (acl_msg_verbose)
		acl_msg_info("%s(%d)->%s: configure file = %s", 
				__FILE__, __LINE__, myname, conf_file_ptr);

	/*
	 * Application-specific initialization.
	 */
	va_start(ap, service);
	while ((key = va_arg(ap, int)) != 0) {
		switch (key) {
		case ACL_MASTER_SERVER_INT_TABLE:
			acl_get_app_conf_int_table(va_arg(ap, ACL_CONFIG_INT_TABLE *));
			break;
		case ACL_MASTER_SERVER_INT64_TABLE:
			acl_get_app_conf_int64_table(va_arg(ap, ACL_CONFIG_INT64_TABLE *));
			break;
		case ACL_MASTER_SERVER_STR_TABLE:
			acl_get_app_conf_str_table(va_arg(ap, ACL_CONFIG_STR_TABLE *));
			break;
		case ACL_MASTER_SERVER_BOOL_TABLE:
			acl_get_app_conf_bool_table(va_arg(ap, ACL_CONFIG_BOOL_TABLE *));
			break;

		case ACL_MASTER_SERVER_PRE_INIT:
			pre_init = va_arg(ap, ACL_MASTER_SERVER_INIT_FN);
			break;
		case ACL_MASTER_SERVER_POST_INIT:
			post_init = va_arg(ap, ACL_MASTER_SERVER_INIT_FN);
			break;
		case ACL_MASTER_SERVER_LOOP:
			loop = va_arg(ap, ACL_MASTER_SERVER_LOOP_FN);
			break;
		case ACL_MASTER_SERVER_EXIT:
			multi_server_onexit = va_arg(ap, ACL_MASTER_SERVER_EXIT_FN);
			break;
		case ACL_MASTER_SERVER_PRE_ACCEPT:
			multi_server_pre_accept = va_arg(ap, ACL_MASTER_SERVER_ACCEPT_FN);
			break;
		case ACL_MASTER_SERVER_PRE_DISCONN:
			multi_server_pre_disconn = va_arg(ap, ACL_MASTER_SERVER_DISCONN_FN);
			break;
		case ACL_MASTER_SERVER_ON_ACCEPT:
			multi_server_on_accept = va_arg(ap, ACL_MASTER_SERVER_ON_ACCEPT_FN);
			break;

		/* add by zsx for rw timeout, 2005.9.25*/
		case ACL_MASTER_SERVER_RW_TIMER:
			multi_server_rw_timer = va_arg(ap, ACL_MASTER_SERVER_RW_TIMER_FN);
			break; /* bugfix, I forgot add 'break' here, sorry, 2005.9.26 */

		case ACL_MASTER_SERVER_IN_FLOW_DELAY:
			multi_server_in_flow_delay = 1;
			break;
		case ACL_MASTER_SERVER_SOLITARY:
			if (!alone)
				acl_msg_fatal("service %s requires a process limit of 1",
						service_name);
			break;
		case ACL_MASTER_SERVER_UNLIMITED:
			if (!zerolimit)
				acl_msg_fatal("service %s requires a process limit of 0",
						service_name);
			break;
		case ACL_MASTER_SERVER_PRIVILEGED:
			if (user_name)
				acl_msg_fatal("service %s requires privileged operation",
						service_name);
			break;
		default:
			acl_msg_panic("%s: unknown argument type: %d", myname, key);
		}
	}
	va_end(ap);

	if (root_dir)
		root_dir = acl_var_multi_queue_dir;
	if (user_name)
		user_name = acl_var_multi_owner;

	/*
	 * If not connected to stdin, stdin must not be a terminal.
	 */
	if (stream == 0 && isatty(STDIN_FILENO))
		acl_msg_fatal("%s(%d)->%s: do not run this command by hand",
				__FILE__, __LINE__, myname);

	/*
	 * Can options be required?
	 */
	if (stream == 0) {
		if (transport == 0)
			acl_msg_fatal("no transport type specified");
		if (strcasecmp(transport, ACL_MASTER_XPORT_NAME_INET) == 0) {
			multi_server_accept = multi_server_accept_inet;
			fdtype = ACL_VSTREAM_TYPE_LISTEN | ACL_VSTREAM_TYPE_LISTEN_INET;
		} else if (strcasecmp(transport, ACL_MASTER_XPORT_NAME_UNIX) == 0) {
			multi_server_accept = multi_server_accept_local;
			fdtype = ACL_VSTREAM_TYPE_LISTEN | ACL_VSTREAM_TYPE_LISTEN_UNIX;
#ifdef MASTER_XPORT_NAME_PASS
		} else if (strcasecmp(transport, ACL_MASTER_XPORT_NAME_PASS) == 0) {
			multi_server_accept = multi_server_accept_pass;
			fdtype = ACL_VSTREAM_TYPE_LISTEN;
#endif
		} else {
			acl_msg_fatal("unsupported transport type: %s", transport);
		}
	}

	/*
	 * Retrieve process generation from environment.
	 */
	if ((generation = getenv(ACL_MASTER_GEN_NAME)) != 0) {
		if (!acl_alldig(generation))
			acl_msg_fatal("bad generation: %s", generation);
		sscanf(generation, "%o", &multi_server_generation);
		if (acl_msg_verbose)
			acl_msg_info("process generation: %s (%o)",
					generation, multi_server_generation);
	}

	/*
	 * Traditionally, BSD select() can't handle multiple processes selecting
	 * on the same socket, and wakes up every process in select(). See TCP/IP
	 * Illustrated volume 2 page 532. We avoid select() collisions with an
	 * external lock file.
	 */

	/*
	 * Set up call-back info.
	 */
	multi_server_service = service;
	multi_server_name = service_name;
	multi_server_argv = argv + optind;

	__eventp = acl_event_new_select(acl_var_multi_delay_sec,
				acl_var_multi_delay_usec);

	/*
	 * Run pre-jail initialization.
	 */
	if (chdir(acl_var_multi_queue_dir) < 0)
		acl_msg_fatal("chdir(\"%s\"): %s", acl_var_multi_queue_dir, strerror(errno));
	if (pre_init)
		pre_init(multi_server_name, multi_server_argv);

	acl_chroot_uid(root_dir, user_name);
	/* �����ӽ������л������������ core �ļ� */
	if (acl_var_multi_enable_core)
		set_core_limit();
	multi_server_open_log();

	/*
	 * Run post-jail initialization.
	 */
	if (post_init)
		post_init(multi_server_name, multi_server_argv);

	/*
	 * Are we running as a one-shot server with the client connection on
	 * standard input? If so, make sure the output is written to stdout so as
	 * to satisfy common expectation.
	 */
	if (stream != 0) {
		while (1)
			service(stream, multi_server_name, multi_server_argv);
		/* not reached here */
		multi_server_exit();
	}

	/*
	 * Running as a semi-resident server. Service connection requests.
	 * Terminate when we have serviced a sufficient number of clients, when
	 * no-one has been talking to us for a configurable amount of time, or
	 * when the master process terminated abnormally.
	 */
	if (acl_var_multi_idle_limit > 0)
		acl_event_request_timer(__eventp,
			multi_server_timeout,
			NULL,
			(acl_int64) acl_var_multi_idle_limit * 1000000, 0);

	/* socket count is as same listen_fd_count in parent process */

	__listen_streams = (ACL_VSTREAM **)
		acl_mycalloc(socket_count + 1, sizeof(ACL_VSTREAM *));
	for (i = 0; i < socket_count + 1; i++)
		__listen_streams[i] = NULL;

	i = 0;
	for (fd = ACL_MASTER_LISTEN_FD; fd < ACL_MASTER_LISTEN_FD + socket_count; fd++) {
		stream = acl_vstream_fdopen(fd,
					O_RDWR,
					acl_var_multi_buf_size,
					acl_var_multi_rw_timeout,
					fdtype);
		if (stream == NULL)
			acl_msg_fatal("%s(%d)->%s: stream null, fd = %d",
					__FILE__, __LINE__, myname, fd);

		acl_event_enable_read(__eventp,
				stream,
				0,
				multi_server_accept,
				stream);
		acl_close_on_exec(ACL_VSTREAM_SOCK(stream), ACL_CLOSE_ON_EXEC);
		__listen_streams[i] = stream;
	}

	acl_event_enable_read(__eventp, ACL_MASTER_STAT_STREAM, 0, multi_server_abort, (void *) 0);
	acl_close_on_exec(ACL_MASTER_STATUS_FD, ACL_CLOSE_ON_EXEC);
	acl_close_on_exec(ACL_MASTER_FLOW_READ, ACL_CLOSE_ON_EXEC);
	acl_close_on_exec(ACL_MASTER_FLOW_WRITE, ACL_CLOSE_ON_EXEC);
	watchdog = acl_watchdog_create(acl_var_multi_daemon_timeout, (ACL_WATCHDOG_FN) 0, (char *) 0);

	/*
	 * The event loop, at last.
	 */
	while (acl_var_multi_use_limit == 0
	       || use_count < acl_var_multi_use_limit
	       || client_count > 0) {
		int  delay_sec;

		if (multi_server_lock != 0) {
			acl_watchdog_stop(watchdog);
			if (acl_myflock(ACL_VSTREAM_FILE(multi_server_lock),
					ACL_INTERNAL_LOCK,
					ACL_MYFLOCK_OP_EXCLUSIVE) < 0)
				acl_msg_fatal("select lock: %s", strerror(errno));
		}
		acl_watchdog_start(watchdog);
		delay_sec = loop ? loop(multi_server_name, multi_server_argv) : -1;
		acl_event_set_delay_sec(__eventp, delay_sec);
		acl_event_loop(__eventp);
		if (listen_disabled == 1) {
			listen_disabled = 2;
			disable_listen();
		}
	}
	multi_server_exit();
}

#endif /* ACL_UNIX */
