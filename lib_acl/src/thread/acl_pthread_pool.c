#include "StdAfx.h"
#ifndef ACL_PREPARE_COMPILE

#include "stdlib/acl_define.h"

#include <errno.h>
#ifdef	ACL_UNIX
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#elif	defined(ACL_MS_WINDOWS)
# ifdef ACL_MS_VC
#  pragma once
# endif
# ifdef ACL_BCB_COMPILER
#  pragma hdrstop
# endif
#endif

#include "stdlib/acl_sys_patch.h"
#include "stdlib/acl_msg.h"
#include "thread/acl_pthread.h"
#include "stdlib/acl_mymalloc.h"
#include "stdlib/acl_debug.h"
#include "stdlib/acl_slice.h"
#include "thread/acl_pthread_pool.h"

#endif

#define	ACL_PTHREAD_POOL_VALID		0x0decca62

typedef struct ACL_TASK {
	struct ACL_TASK *next;
	void (*worker_fn)(void *arg);       /* user function */
	void  *worker_arg;
} ACL_TASK;

#undef	USE_SLICE

struct acl_pthread_pool_t {
	acl_pthread_mutex_t	worker_mutex;   /* control access to queue */
	acl_pthread_cond_t	worker_cond;    /* wait_for_work */
	acl_pthread_mutex_t	poller_mutex;   /* just for wait poller exit */
	acl_pthread_cond_t	poller_cond;    /* just for wait poller exit */
	acl_pthread_attr_t	attr;           /* create detached */
	ACL_TASK *first, *last;                 /* work queue */
#ifdef	USE_SLICE
	ACL_SLICE *slice;
#endif
	int   poller_running;               /* is poller thread running ? */
	int   qlen;                         /* the work queue's length */
	int   valid;                        /* valid */
	int   worker_quit;                  /* worker should quit */
	int   poller_quit;                  /* poller should quit */
	int   parallelism;                  /* maximum threads */
	int   counter;                      /* current threads */
	int   idle;                         /* idle threads */
	int   idle_timeout;                 /* idle timeout second */
	int   overload_timewait;            /* when too busy, need sleep ? */
	time_t last_warn;                   /* last warn time               */
	int  (*poller_fn)(void *arg);       /* worker poll function */
	void *poller_arg;                   /* the arg of poller_fn */
	int  (*worker_init_fn)(void *arg);  /* the arg is worker_init_arg */
	void *worker_init_arg;
	void (*worker_free_fn)(void *arg);  /* the arg is worker_free_arg */
	void *worker_free_arg;
};

#undef	__SET_ERRNO
#ifdef	ACL_MS_WINDOWS
# define	__SET_ERRNO(_x_) (void) 0
#elif	defined(ACL_UNIX)
# define	__SET_ERRNO(_x_) (acl_set_error(_x_))
#else
# error "unknown OS type"
#endif

#ifdef	ACL_MS_WINDOWS
#define	sleep(_x_) do {  \
	Sleep(_x_ * 1000);  \
} while (0)
#endif

static void *__poller_thread(void *arg)
{
	char  myname[] = "__poller_thread";
	acl_pthread_pool_t   *thr_pool = (acl_pthread_pool_t*) arg;
	const int wait_sec = 1, max_loop_persec = 81920;
	int   loop_count;
	int   status;
	char  buf[256];
	time_t now_t, pre_loop_t;
#ifdef	ACL_UNIX
	pthread_t id = pthread_self();
#elif	defined(ACL_MS_WINDOWS)
	unsigned long id = acl_pthread_self();
#else
        # error "unknown OS"
#endif

	if (thr_pool->poller_fn == NULL)
		acl_msg_fatal("%s, %s(%d): poller_fn is null!",
			__FILE__, myname, __LINE__);

	acl_debug(ACL_DEBUG_THR_POOL, 2) ("%s(%d): poller(tid=%lu) started ...",
			myname, __LINE__, (unsigned long) id);
	loop_count = 0;
	pre_loop_t = time(NULL);

	status = acl_pthread_mutex_lock(&thr_pool->poller_mutex);
	if (status != 0) {
		thr_pool->poller_running = 0;
		__SET_ERRNO(status);
		acl_msg_error("%s, %s(%d): pthread_mutex_lock error(%s)",
				__FILE__, myname, __LINE__,
				acl_last_strerror(buf, sizeof(buf)));
		return (NULL);
	}

	thr_pool->poller_running = 1;

	for (;;) {
		if (thr_pool->poller_quit)
			break;
		
		now_t = time(NULL);
		loop_count++;
		if (loop_count >= max_loop_persec) {
			/* avoid loop too quickly in one second */
			if (now_t - pre_loop_t <= wait_sec) {
				acl_msg_warn("%s: loop too fast, sleep %d second",
						myname, wait_sec);
				sleep(wait_sec);
				now_t = time(NULL);  /* adjust the time of now */
			}
			pre_loop_t = now_t;  /*adjust the pre_loop_t time */
			loop_count = 0;
		}

		if (thr_pool->poller_fn(thr_pool->poller_arg) < 0)
			break;
	}

	acl_debug(ACL_DEBUG_THR_POOL, 2) ("%s(%d): poller(%lu) thread quit ...",
			myname, __LINE__, (unsigned long) id);

	thr_pool->poller_running = 0;
		
#ifdef	ACL_MS_WINDOWS 
	status = acl_pthread_cond_signal(&thr_pool->poller_cond);
#else
	status = pthread_cond_broadcast(&thr_pool->poller_cond);
#endif

	if ( status != 0 ) {
		__SET_ERRNO(status);
		acl_msg_error("%s(%d)->%s: pthread_cond_signal, serr = %s",
				__FILE__, __LINE__, myname,
				acl_last_strerror(buf, sizeof(buf)));
	}

	acl_debug(ACL_DEBUG_THR_POOL, 3) ("poller broadcast ok");

	status = acl_pthread_mutex_unlock(&thr_pool->poller_mutex);
	if (status != 0) {
		__SET_ERRNO(status);
		acl_msg_error("%s, %s(%d): pthread_mutex_unlock error(%s)",
				__FILE__, myname, __LINE__,
				acl_last_strerror(buf, sizeof(buf)));
		return (NULL);
	}

	acl_debug(ACL_DEBUG_THR_POOL, 3) ("poller unlock ok");

	return (NULL);
}

static void *__worker_thread(void* arg)
{
	const char *myname = "__worker_thread";
	acl_pthread_pool_t *thr_pool = (acl_pthread_pool_t*) arg;
	int  status;
	char  buf[256];
	ACL_TASK *we;
	struct	timespec  timeout;
	struct  timeval   tv;
	int     timedout;

#undef	RETURN
#define	RETURN(_x_) {  \
	if (thr_pool->worker_free_fn != NULL)  \
		thr_pool->worker_free_fn(thr_pool->worker_free_arg);  \
	return (_x_);  \
}
	
	if (thr_pool->worker_init_fn != NULL) {
		if (thr_pool->worker_init_fn(thr_pool->worker_init_arg) < 0) {
			acl_msg_error("%s(%d)->%s: tid = %lu, worker init call error",
					__FILE__, __LINE__, myname,
					(unsigned long) acl_pthread_self());
			return (NULL);
		}
	}

	status = acl_pthread_mutex_lock(&thr_pool->worker_mutex);
	if (status != 0) {
		__SET_ERRNO(status);
		acl_msg_error("%s(%d)->%s: tid = %lu, lock error(%s)",
				__FILE__, __LINE__, myname,
				(unsigned long) acl_pthread_self(),
				acl_last_strerror(buf, sizeof(buf)));
		return (NULL);
	}
	
	for (;;) {
		gettimeofday(&tv, NULL);
		timeout.tv_sec = tv.tv_sec + thr_pool->idle_timeout;
		timeout.tv_nsec = tv.tv_usec * 1000;
		timedout = 0;

		while (thr_pool->first == NULL && !thr_pool->worker_quit) {
			thr_pool->idle++;

			if (thr_pool->idle_timeout > 0)
				status = acl_pthread_cond_timedwait(&thr_pool->worker_cond,
						&thr_pool->worker_mutex, &timeout);
			else
				status = acl_pthread_cond_wait(&thr_pool->worker_cond,
						&thr_pool->worker_mutex);
			thr_pool->idle--;
			if (status == ACL_ETIMEDOUT) {
				timedout = 1;
				break;
			} else if (status != 0) {
				__SET_ERRNO(status);
				thr_pool->counter--;
				acl_pthread_mutex_unlock(&thr_pool->worker_mutex);
				acl_msg_error("%s(%d)->%s: tid = %lu,"
					" cond timewait error(%s)(%s)",
					__FILE__, __LINE__, myname,
					(unsigned long) acl_pthread_self(),
					acl_last_strerror(buf, sizeof(buf)),
					strerror(status));
				RETURN (NULL);
			}
		}  /* end while */
		we = thr_pool->first;
		if (we != NULL) {
			if (we->worker_fn == NULL)
				acl_msg_fatal("%s(%d)->%s: worker_fn null",
						__FILE__, __LINE__, myname);

			thr_pool->first = we->next;
			thr_pool->qlen--;
			if (thr_pool->last == we)
				thr_pool->last = NULL;
			/* the lock shuld be unlocked before enter working processs */
			status = acl_pthread_mutex_unlock(&thr_pool->worker_mutex);
			if (status != 0) {
				__SET_ERRNO(status);
				acl_msg_error("%s(%d)->%s: unlock error(%s), tid=%lu",
						__FILE__, __LINE__, myname,
						acl_last_strerror(buf, sizeof(buf)),
						(unsigned long) acl_pthread_self());
				RETURN (NULL);
			}
			we->worker_fn(we->worker_arg);
#ifndef	USE_SLICE
			acl_myfree(we);
#endif

			/* lock again */
			status = acl_pthread_mutex_lock(&thr_pool->worker_mutex);
			if (status != 0) {
				__SET_ERRNO(status);
				acl_msg_error("%s(%d)->%s: lock error(%s), sid=%lu",
						__FILE__, __LINE__, myname,
						acl_last_strerror(buf, sizeof(buf)),
						(unsigned long) acl_pthread_self());
				RETURN (NULL);
			}
#ifdef	USE_SLICE
			acl_slice_free2(thr_pool->slice, we);
#endif
		}
		if (thr_pool->first == NULL && thr_pool->worker_quit) {
			thr_pool->counter--;
			if (thr_pool->counter == 0)
				acl_pthread_cond_broadcast(&thr_pool->worker_cond);
			break;
		}

		if (thr_pool->first == NULL && timedout) {
			thr_pool->counter--;
			break;
		}
	}

	status = acl_pthread_mutex_unlock(&thr_pool->worker_mutex);
	if (status != 0) {
		__SET_ERRNO(status);
		acl_msg_error("%s, %s(%d): unlock error(%s)",
				__FILE__, myname, __LINE__,
				acl_last_strerror(buf, sizeof(buf)));
	}

	acl_debug(ACL_DEBUG_THR_POOL, 2) ("%s(%d): thread(%lu) exit now",
		myname, __LINE__, (unsigned long) acl_pthread_self());

	RETURN (NULL);
}

static void __init_workq(acl_pthread_pool_t *thr_pool)
{
	thr_pool->worker_quit = 0;
	thr_pool->poller_quit = 0;
	thr_pool->poller_running = 0;
	thr_pool->first = NULL;
	thr_pool->last = NULL;
	thr_pool->qlen = 0;
	thr_pool->overload_timewait = 0;
	thr_pool->counter = 0;
	thr_pool->idle = 0;
}

/* create work queue */

acl_pthread_pool_t *acl_thread_pool_create(int threads_limit, int idle_timeout)
{
	acl_pthread_pool_t *thr_pool;  /* �̳߳ؾ�� */
	acl_pthread_pool_attr_t attr;  /* �̳߳س�ʼ��ʱ������ */

	/* ��ʼ���̳߳ض������� */
	acl_pthread_pool_attr_init(&attr);
	acl_pthread_pool_attr_set_threads_limit(&attr, threads_limit);
	acl_pthread_pool_attr_set_idle_timeout(&attr, idle_timeout);

	/* ������פ���߳̾�� */
	thr_pool = acl_pthread_pool_create(&attr);
	return (thr_pool);
}

acl_pthread_pool_t *acl_pthread_pool_create(const acl_pthread_pool_attr_t *attr)
{
	char *myname = "acl_pthread_pool_create";
	int   status;
	acl_pthread_pool_t *thr_pool;
	char  buf[256];

	thr_pool = acl_mycalloc(1, sizeof(*thr_pool));
	status = acl_pthread_attr_init(&thr_pool->attr);
	if (status != 0) {
		__SET_ERRNO(status);
		acl_msg_error("%s(%d)->%s: pthread_attr_init, serr = %s",
				__FILE__, __LINE__, myname,
				acl_last_strerror(buf, sizeof(buf)));
		return (NULL);
	}

	if (attr && attr->stack_size > 0)
		acl_pthread_attr_setstacksize(&thr_pool->attr, attr->stack_size);

#ifdef	ACL_UNIX
	status = pthread_attr_setdetachstate(&thr_pool->attr, PTHREAD_CREATE_DETACHED);
	if (status != 0) {
		acl_set_error(status);
		pthread_attr_destroy(&thr_pool->attr);
		acl_myfree(thr_pool);
		acl_msg_error("%s(%d)->%s: pthread_attr_setdetachstate, serr = %s",
				__FILE__, __LINE__, myname,
				acl_last_strerror(buf, sizeof(buf)));
		return (NULL);
	}
# if     !defined(__FreeBSD__)
	status = pthread_attr_setscope(&thr_pool->attr, PTHREAD_SCOPE_SYSTEM);
	if (status != 0) {
		pthread_attr_destroy(&thr_pool->attr);
		acl_myfree(thr_pool);
		acl_set_error(status);
		acl_msg_error("%s(%d)->%s: pthread_attr_setscope, serr = %s",
				__FILE__, __LINE__, myname,
				acl_last_strerror(buf, sizeof(buf)));
		return (NULL);
	}
# endif
#elif defined(ACL_MS_WINDOWS)
	(void) acl_pthread_attr_setdetachstate(&thr_pool->attr, 1);
#endif
	status = acl_pthread_mutex_init(&thr_pool->worker_mutex, NULL);
	if (status != 0) {
		__SET_ERRNO(status);
		acl_pthread_attr_destroy(&thr_pool->attr);
		acl_myfree(thr_pool);
		acl_msg_error("%s(%d)->%s: pthread_mutex_init, serr = %s",
				__FILE__, __LINE__, myname,
				acl_last_strerror(buf, sizeof(buf)));
		return (NULL);
	}
	status = acl_pthread_cond_init(&thr_pool->worker_cond, NULL);
	if (status != 0) {
		acl_pthread_attr_destroy(&thr_pool->attr);
		acl_pthread_mutex_destroy(&thr_pool->worker_mutex);
		acl_myfree(thr_pool);
		__SET_ERRNO(status);
		acl_msg_error("%s(%d)->%s: pthread_cond_init, serr = %s",
				__FILE__, __LINE__, myname,
				acl_last_strerror(buf, sizeof(buf)));
		return (NULL);
	}

	status = acl_pthread_mutex_init(&thr_pool->poller_mutex, NULL);
	if (status != 0) {
		__SET_ERRNO(status);
		acl_pthread_attr_destroy(&thr_pool->attr);
		acl_pthread_mutex_destroy(&thr_pool->worker_mutex);
		acl_pthread_cond_destroy(&thr_pool->worker_cond);
		acl_myfree(thr_pool);
		acl_msg_error("%s(%d)->%s: pthread_mutex_init, serr = %s",
				__FILE__, __LINE__, myname,
				acl_last_strerror(buf, sizeof(buf)));
		return (NULL);
	}
	status = acl_pthread_cond_init(&thr_pool->poller_cond, NULL);
	if (status != 0) {
		acl_pthread_attr_destroy(&thr_pool->attr);
		acl_pthread_mutex_destroy(&thr_pool->worker_mutex);
		acl_pthread_cond_destroy(&thr_pool->worker_cond);
		acl_pthread_mutex_destroy(&thr_pool->poller_mutex);
		acl_myfree(thr_pool);
		__SET_ERRNO(status);
		acl_msg_error("%s(%d)->%s: pthread_cond_init, serr = %s",
				__FILE__, __LINE__, myname,
				acl_last_strerror(buf, sizeof(buf)));
		return (NULL);
	}

	__init_workq(thr_pool);

#ifdef	USE_SLICE
	thr_pool->slice = acl_slice_create("thread_pool", 10240 * sizeof(ACL_TASK),
			sizeof(ACL_TASK), ACL_SLICE_FLAG_GC2);
#endif
	thr_pool->parallelism = (attr && attr->threads_limit > 0) ?
		attr->threads_limit : ACL_PTHREAD_POOL_DEF_THREADS;
	thr_pool->idle_timeout = (attr && attr->idle_timeout > 0) ?
		attr->idle_timeout : ACL_PTHREAD_POOL_DEF_IDLE;
	thr_pool->poller_fn = NULL;
	thr_pool->poller_arg = NULL;
	
	thr_pool->worker_init_fn = NULL;
	thr_pool->worker_init_arg = NULL;
	thr_pool->worker_free_fn = NULL;
	thr_pool->worker_free_arg = NULL;

	thr_pool->valid = ACL_PTHREAD_POOL_VALID;

	return (thr_pool);
}

int acl_pthread_pool_set_timewait(acl_pthread_pool_t *thr_pool, int timewait)
{
	char *myname = "acl_pthread_pool_set_timewait";

	if (thr_pool == NULL || thr_pool->valid != ACL_PTHREAD_POOL_VALID || timewait < 0) {
		acl_msg_error("%s(%d)->%s: invalid input",
				__FILE__, __LINE__, myname);
		return (-1);
	}

	thr_pool->overload_timewait = timewait;
	return (0);
}

int acl_pthread_pool_atinit(acl_pthread_pool_t *thr_pool,
		int (*init_fn)(void *), void *init_arg)
{
	char *myname = "acl_pthread_pool_atinit";

	if (thr_pool == NULL || thr_pool->valid != ACL_PTHREAD_POOL_VALID) {
		acl_msg_error("%s(%d)->%s: input invalid",
				__FILE__, __LINE__, myname);
		return (ACL_EINVAL);
	}

	thr_pool->worker_init_fn = init_fn;
	thr_pool->worker_init_arg = init_arg;

	return (0);
}

int acl_pthread_pool_atfree(acl_pthread_pool_t *thr_pool,
			void (*free_fn)(void *), void *free_arg)
{
	char *myname = "acl_pthread_pool_atfree";

	if (thr_pool == NULL || thr_pool->valid != ACL_PTHREAD_POOL_VALID) {
		acl_msg_error("%s(%d)->%s: input invalid",
				__FILE__, __LINE__, myname);
		return (ACL_EINVAL);
	}

	thr_pool->worker_free_fn = free_fn;
	thr_pool->worker_free_arg = free_arg;

	return (0);
}

static int __wait_poller_exit(acl_pthread_pool_t *thr_pool)
{
	char *myname = "__wait_poller_exit";
	int   status, nwait = 0;
	char  buf[256];
	struct  timeval   tv;
	struct	timespec timeout;

	acl_debug(ACL_DEBUG_THR_POOL, 3) ("%s: begin to lock", myname);

	thr_pool->poller_quit = 1;

	status = acl_pthread_mutex_lock(&thr_pool->poller_mutex);
	if (status != 0) {
		__SET_ERRNO(status);
		acl_msg_error("%s, %s(%d): pthread_mutex_lock, serr = %s",
				__FILE__, myname, __LINE__,
				acl_last_strerror(buf, sizeof(buf)));
		return (status);
	}

	acl_debug(ACL_DEBUG_THR_POOL, 3) ("%s: begin to wait cond", myname);

	while (thr_pool->poller_running != 0) {
		gettimeofday(&tv, NULL);
		timeout.tv_sec = tv.tv_sec + 1;
		timeout.tv_nsec = tv.tv_usec * 1000;

		nwait++;

		status = acl_pthread_cond_timedwait(&thr_pool->poller_cond,
					&thr_pool->poller_mutex, &timeout);
		if (status == ACL_ETIMEDOUT) {
			acl_debug(ACL_DEBUG_THR_POOL, 3) ("%s: nwait=%d", myname, nwait);
		} else if (status != 0) {
			__SET_ERRNO(status);
			acl_pthread_mutex_unlock(&thr_pool->poller_mutex);
			acl_msg_error("%s, %s(%d): pthread_cond_wait,"
					" serr = %s", __FILE__, myname, __LINE__,
					acl_last_strerror(buf, sizeof(buf)));
			return (status);
		}
	}

	acl_debug(ACL_DEBUG_THR_POOL, 3) ("%s: begin to unlock", myname);

	status = acl_pthread_mutex_unlock(&thr_pool->poller_mutex);
	if (status != 0) {
		__SET_ERRNO(status);
		acl_msg_error("%s, %s(%d): pthread_mutex_unlock error(%s)",
				__FILE__, myname, __LINE__,
				acl_last_strerror(buf, sizeof(buf)));
	}

	return (status);
}

static int __wait_worker_exit(acl_pthread_pool_t *thr_pool)
{
	char *myname = "__wait_worker_exit";
	int   status, nwait = 0;
	char  buf[256];
	struct	timespec  timeout;

	status = acl_pthread_mutex_lock(&thr_pool->worker_mutex);
	if (status != 0) {
		__SET_ERRNO(status);
		acl_msg_error("%s(%d)->%s: pthread_mutex_lock, serr = %s",
				__FILE__, __LINE__, myname,
				acl_last_strerror(buf, sizeof(buf)));
		return (status);
	}

	thr_pool->worker_quit = 1;

	if (thr_pool->counter < 0) {
		acl_msg_fatal("%s(%d)->%s: counter = %d",
				__FILE__, __LINE__, myname,
				thr_pool->counter);
	} else if (thr_pool->counter == 0) {
		acl_debug(ACL_DEBUG_THR_POOL, 2) ("%s: debug: counter = 0", myname);
		status = acl_pthread_mutex_unlock(&thr_pool->worker_mutex);
		return (0);
	}

	/* 1. set quit flag
	 * 2. broadcast to wakeup any sleeping
	 * 4. wait till all quit
	 */
	/* then: thr_pool->counter > 0 */
	
	if (thr_pool->idle > 0) {
		acl_debug(ACL_DEBUG_THR_POOL, 2) ("%s: idle=%d, signal idle thread",
			myname, thr_pool->idle);
		status = acl_pthread_cond_broadcast(&thr_pool->worker_cond);
		if (status != 0) {
			__SET_ERRNO(status);
			acl_pthread_mutex_unlock(&thr_pool->worker_mutex);
			acl_msg_error("%s(%d)->%s: pthread_cond_broadcast,"
					" serr = %s", __FILE__, __LINE__, myname,
					acl_last_strerror(buf, sizeof(buf)));
			return (status);
		}
	}

	while (thr_pool->counter > 0) {
		timeout.tv_sec  = 0;
		timeout.tv_nsec = 1000;

		nwait++;

		acl_debug(ACL_DEBUG_THR_POOL, 2)
			("debug(2): counter = %d, nwait=%d, idle=%d",
			thr_pool->counter, nwait, thr_pool->idle);

		/* status = pthread_cond_timedwait(&thr_pool->worker_cond, &thr_pool->worker_mutex, &timeout); */
		status = acl_pthread_cond_wait(&thr_pool->worker_cond, &thr_pool->worker_mutex);
		if (status == ACL_ETIMEDOUT) {
			acl_debug(ACL_DEBUG_THR_POOL, 2) ("%s: timeout nwait=%d", myname, nwait);
		} else if (status != 0) {
			__SET_ERRNO(status);
			acl_pthread_mutex_unlock(&thr_pool->worker_mutex);
			acl_msg_error("%s(%d)->%s: pthread_cond_timedwait,"
					" serr = %s", __FILE__, __LINE__, myname,
					acl_last_strerror(buf, sizeof(buf)));
			return (status);
		}
	}

	status = acl_pthread_mutex_unlock(&thr_pool->worker_mutex);
	if (status != 0) {
		__SET_ERRNO(status);
		acl_msg_error("%s(%d)->%s: pthread_mutex_unlock, serr = %s",
				__FILE__, __LINE__, myname, acl_last_strerror(buf, sizeof(buf)));
	}

	return (status);
}

int acl_pthread_pool_destroy(acl_pthread_pool_t *thr_pool)
{
	char *myname = "acl_pthread_pool_destroy";
	int   status, s1, s2, s3, s4, s5;
	char  buf[256];

	if (thr_pool == NULL || thr_pool->valid != ACL_PTHREAD_POOL_VALID) {
		acl_msg_error("%s(%d)->%s: input invalid",
				__FILE__, __LINE__, myname);
		return (ACL_EINVAL);
	}

	thr_pool->valid = 0;			/* prevent any other operations	*/

	status = __wait_poller_exit(thr_pool);
	if (status != 0) {
		__SET_ERRNO(status);
		acl_msg_error("%s, %s(%d): wait_poller_exit error(%s), ret=%d",
				__FILE__, myname, __LINE__,
				acl_last_strerror(buf, sizeof(buf)), status);
		return (status);
	}

	acl_debug(ACL_DEBUG_THR_POOL, 2)
		("%s(%d): poller thread exits ok, worker counter = %d",
		 myname, __LINE__, thr_pool->counter);

	status = __wait_worker_exit(thr_pool);
	if (status != 0) {
		__SET_ERRNO(status);
		acl_msg_error("%s, %s(%d): wait_worker_exit error(%s), ret=%d",
				__FILE__, myname, __LINE__,
				acl_last_strerror(buf, sizeof(buf)), status);
		return (status);
	}

	acl_debug(ACL_DEBUG_THR_POOL, 2)
		("%s(%d): worker threads exit ok, conter=%d",
		 myname, __LINE__, thr_pool->counter);

	sleep(1);
	s1 = acl_pthread_mutex_destroy(&thr_pool->poller_mutex);
	s2 = acl_pthread_cond_destroy(&thr_pool->poller_cond);

	s3 = acl_pthread_mutex_destroy(&thr_pool->worker_mutex);
	s4 = acl_pthread_cond_destroy(&thr_pool->worker_cond);
	s5 = acl_pthread_attr_destroy(&thr_pool->attr);

	acl_myfree(thr_pool);

	status = s1 ? s1 : (s2 ? s2 : (s3 ? s3 : (s4 ? s4 : s5)));

	return (status);
}

int acl_pthread_pool_stop(acl_pthread_pool_t *thr_pool)
{
	char *myname = "acl_pthread_pool_stop";
	int   status;
	char  buf[256];

	if (thr_pool == NULL || thr_pool->valid != ACL_PTHREAD_POOL_VALID) {
		acl_msg_error("%s(%d)->%s: input invalid",
				__FILE__, __LINE__, myname);
		return (ACL_EINVAL);
	}

	thr_pool->valid = 0;			/* prevent any other operations	*/

	status = __wait_poller_exit(thr_pool);
	if (status != 0) {
		__SET_ERRNO(status);
		acl_msg_error("%s, %s(%d): wait_poller_exit error(%s), ret=%d",
				__FILE__, myname, __LINE__,
				acl_last_strerror(buf, sizeof(buf)), status);
		return (status);
	}

	acl_debug(ACL_DEBUG_THR_POOL, 2)
		("%s(%d): poller thread exits ok, worker counter = %d",
		 myname, __LINE__, thr_pool->counter);

	status = __wait_worker_exit(thr_pool);
	if (status != 0) {
		__SET_ERRNO(status);
		acl_msg_error("%s, %s(%d): wait_worker_exit error(%s), ret=%d",
				__FILE__, myname, __LINE__,
				acl_last_strerror(buf, sizeof(buf)), status);
		return (status);
	}

	thr_pool->valid = ACL_PTHREAD_POOL_VALID;  /* restore the valid status */

	acl_debug(ACL_DEBUG_THR_POOL, 2)
		("%s(%d): worker threads exit ok, conter=%d",
		 myname, __LINE__, thr_pool->counter);

	return (0);
}

static void __workq_addone(acl_pthread_pool_t *thr_pool, ACL_TASK *item)
{
	char *myname = "__workq_addone";
	acl_pthread_t id;
	int   status;
	char  buf[256];

	if (thr_pool->first == NULL)
		thr_pool->first = item;
	else
		thr_pool->last->next = item;
	thr_pool->last = item;

	thr_pool->qlen++;

	if (thr_pool->idle > 0) {
#ifdef	ACL_MS_WINDOWS 
		status = acl_pthread_cond_signal(&thr_pool->worker_cond);
#else
		status = pthread_cond_broadcast(&thr_pool->worker_cond);
#endif
		if ( status != 0 ) {
			__SET_ERRNO(status);
			acl_msg_error("%s(%d)->%s: pthread_cond_signal, serr = %s",
				__FILE__, __LINE__, myname,
				acl_last_strerror(buf, sizeof(buf)));
			return;
		}
	} else if (thr_pool->counter < thr_pool->parallelism) {
		status = acl_pthread_create(&id, &thr_pool->attr,
					__worker_thread, (void*) thr_pool);
		if (status != 0) {
			__SET_ERRNO(status);
			acl_msg_fatal("%s(%d)->%s: pthread_create worker, serr = %s",
				__FILE__, __LINE__, myname,
				acl_last_strerror(buf, sizeof(buf)));
		}
		thr_pool->counter++;
	} else if (thr_pool->qlen > 10 * thr_pool->parallelism) {
		time_t now = time(NULL);

		if (now - thr_pool->last_warn >= 2) {
			thr_pool->last_warn = now;
			acl_msg_warn("%s(%d), %s: reached the max_thread = %d"
				", push into the queue now, qlen=%d, idle=%d",
				__FILE__, __LINE__, myname,
				thr_pool->parallelism, thr_pool->qlen, thr_pool->idle);
		}
		if (thr_pool->overload_timewait > 0) {
			acl_msg_warn("%s(%d), %s: sleep %d seconds",
				__FILE__, __LINE__, myname,
				thr_pool->overload_timewait);
			sleep(thr_pool->overload_timewait);
		}
	}

	return;
}

int acl_pthread_pool_add(acl_pthread_pool_t *thr_pool,
		void (*run_fn)(void *), void *run_arg)
{
	char *myname = "acl_pthread_pool_add";
	ACL_TASK *item;
	int   status;
	char  buf[256];

	if (thr_pool->valid != ACL_PTHREAD_POOL_VALID || run_fn == NULL)
		return (ACL_EINVAL);

#ifndef	USE_SLICE
	item = (ACL_TASK*) acl_mymalloc(sizeof(ACL_TASK));
	if (item == NULL)
		return (ACL_ENOMEM);

	item->worker_fn  = run_fn;
	item->worker_arg = run_arg;
	item->next       = NULL;
#endif

	status = acl_pthread_mutex_lock(&thr_pool->worker_mutex);
	if (status != 0) {
		__SET_ERRNO(status);
#ifndef	USE_SLICE
		acl_myfree(item);
#endif
		acl_msg_fatal("%s(%d)->%s: pthread_mutex_lock, serr = %s",
				__FILE__, __LINE__, myname,
				acl_last_strerror(buf, sizeof(buf)));
	}

#ifdef	USE_SLICE
	item = (ACL_TASK*) acl_slice_alloc(thr_pool->slice);
	if (item == NULL) {
		acl_pthread_mutex_unlock(&thr_pool->worker_mutex);
		return (ACL_ENOMEM);
	}
#endif

	item->worker_fn  = run_fn;
	item->worker_arg = run_arg;
	item->next       = NULL;

	__workq_addone(thr_pool, item);

	status = acl_pthread_mutex_unlock(&thr_pool->worker_mutex);
	if (status != 0) {
		__SET_ERRNO(status);
		acl_msg_fatal("%s(%d)->%s: pthread_mutex_unlock error=%s",
				__FILE__, __LINE__, myname,
				acl_last_strerror(buf, sizeof(buf)));
	}

	return (0);
}

void acl_pthread_pool_add_begin(acl_pthread_pool_t *thr_pool)
{
	char *myname = "acl_pthread_pool_add_begin";
	int   status;
	char  buf[256];

	if (thr_pool == NULL)
		acl_msg_fatal("%s(%d)->%s: invalid input",
			__FILE__, __LINE__, myname);

	if (thr_pool->valid != ACL_PTHREAD_POOL_VALID)
		acl_msg_fatal("%s(%d)->%s: invalid thr_pool->valid",
				__FILE__, __LINE__, myname);

	status = acl_pthread_mutex_lock(&thr_pool->worker_mutex);
	if (status != 0) {
		__SET_ERRNO(status);
		acl_msg_fatal("%s(%d)->%s: pthread_mutex_lock, serr = %s",
				__FILE__, __LINE__, myname,
				acl_last_strerror(buf, sizeof(buf)));
	}
}

void acl_pthread_pool_add_one(acl_pthread_pool_t *thr_pool,
		void (*run_fn)(void *), void *run_arg)
{
	char *myname = "acl_pthread_pool_add_one";
	ACL_TASK *item;
	char  buf[256];

	if (thr_pool == NULL)
		acl_msg_fatal("%s(%d)->%s: invalid input",
				__FILE__, __LINE__, myname);

	if (thr_pool->valid != ACL_PTHREAD_POOL_VALID || run_fn == NULL)
		acl_msg_fatal("%s(%d)->%s: invalid thr_pool->valid or run_fn",
				__FILE__, __LINE__, myname);

	item = (ACL_TASK*) acl_mymalloc(sizeof(ACL_TASK));
	if (item == NULL) {
		acl_msg_fatal("%s(%d)->%s: can't malloc, error=%s",
				__FILE__, __LINE__, myname,
				acl_last_strerror(buf, sizeof(buf)));
	}

	item->worker_fn  = run_fn;
	item->worker_arg = run_arg;
	item->next       = NULL;

	__workq_addone(thr_pool, item);
}

void acl_pthread_pool_add_end(acl_pthread_pool_t *thr_pool)
{
	char *myname = "acl_pthread_pool_add_end";
	int   status;
	char  buf[256];

	if (thr_pool == NULL)
		acl_msg_fatal("%s(%d)->%s: invalid input",
				__FILE__, __LINE__, myname);
	if (thr_pool->valid != ACL_PTHREAD_POOL_VALID)
		acl_msg_fatal("%s(%d)->%s: invalid thr_pool->valid",
				__FILE__, __LINE__, myname);

	status = acl_pthread_mutex_unlock(&thr_pool->worker_mutex);
	if (status != 0) {
		__SET_ERRNO(status);
		acl_msg_fatal("%s(%d)->%s: pthread_mutex_unlock, serr = %s",
				__FILE__, __LINE__, myname,
				acl_last_strerror(buf, sizeof(buf)));
	}
}

void acl_pthread_pool_set_poller(acl_pthread_pool_t *thr_pool,
		int (*poller_fn)(void *), void *poller_arg)
{
	const char *myname = "acl_pthread_pool_set_poller";
	if (thr_pool == NULL || thr_pool->valid != ACL_PTHREAD_POOL_VALID || poller_fn == NULL) {
		acl_msg_error("%s(%d)->%s: input invalid",
			__FILE__, __LINE__, myname);
		return;
	}

	thr_pool->poller_fn = poller_fn;
	thr_pool->poller_arg = poller_arg;
}

int acl_pthread_pool_start_poller(acl_pthread_pool_t *thr_pool)
{
	char *myname = "acl_pthread_pool_start_poller";
	acl_pthread_t id;
	int   status;
	char  buf[256];

	if (thr_pool == NULL || thr_pool->valid != ACL_PTHREAD_POOL_VALID) {
		acl_msg_error("%s(%d)->%s: input invalid",
				__FILE__, __LINE__, myname);
		return (-1);
	}

	if (thr_pool->poller_fn == NULL) {
		acl_msg_warn("%s, %s(%d): poller_fn is null, don't need call %s",
			__FILE__, myname, __LINE__, myname);
		return (-1);
	}

	status = acl_pthread_mutex_lock(&thr_pool->poller_mutex);
	if (status != 0) {
		__SET_ERRNO(status);
		acl_msg_error("%s, %s(%d): lock poller_mutex error(%s)",
				__FILE__, myname, __LINE__,
				acl_last_strerror(buf, sizeof(buf)));
		return (-1);
	}

	if (thr_pool->poller_running) {
		acl_msg_error("%s, %s(%d): server is running",
				__FILE__, myname, __LINE__);
		return (-1);
	}

	status = acl_pthread_mutex_unlock(&thr_pool->poller_mutex);
	if (status != 0) {
		__SET_ERRNO(status);
		acl_msg_error("%s, %s(%d): unlock poller_mutex error(%s)",
				__FILE__, myname, __LINE__,
				acl_last_strerror(buf, sizeof(buf)));
		return (-1);
	}

	__init_workq(thr_pool);

	status = acl_pthread_create(&id, &thr_pool->attr,
				__poller_thread, (void*) thr_pool);
	if (status != 0) {
		__SET_ERRNO(status);
		acl_msg_error("%s(%d)->%s: pthread_create poller, serr = %s",
				__FILE__, __LINE__, myname,
				acl_last_strerror(buf, sizeof(buf)));
		return (status);
	}

	return (0);
}

int acl_pthread_pool_add_dispatch(void *dispatch_arg, void (*run_fn)(void *), void *run_arg)
{
	char  myname[] = "acl_workq_batadd_dispatch";
	acl_pthread_pool_t *thr_pool;

	if (dispatch_arg == NULL || run_fn == NULL)
		acl_msg_fatal("%s(%d)->%s: invalid input",
				__FILE__, __LINE__, myname);

	thr_pool = (acl_pthread_pool_t *) dispatch_arg;
	acl_pthread_pool_add_one(thr_pool, run_fn, run_arg);

	return (0);
}

int acl_pthread_pool_dispatch(void *dispatch_arg, void (*run_fn)(void *), void *run_arg)
{
	char *myname = "acl_pthread_pool_dispatch";
	acl_pthread_pool_t *thr_pool;

	if (dispatch_arg == NULL || run_fn == NULL)
		acl_msg_fatal("%s(%d)->%s: invalid input",
				__FILE__, __LINE__, myname);

	thr_pool = (acl_pthread_pool_t *) dispatch_arg;

	return (acl_pthread_pool_add(thr_pool, run_fn, run_arg));
}

int acl_pthread_pool_size(acl_pthread_pool_t *thr_pool)
{
	const char *myname = "acl_pthread_pool_size";
	int   status, n;

	status = acl_pthread_mutex_lock(&thr_pool->worker_mutex);
	if (status) {
		acl_msg_error("%s(%d)->%s: pthread_mutex_lock error(%s)",
			__FILE__, __LINE__, myname, strerror(status));
		return (-1);
	}

	n = thr_pool->counter;
	status = acl_pthread_mutex_unlock(&thr_pool->worker_mutex);
	if (status) {
		acl_msg_error("%s(%d)->%s: pthread_mutex_unlock error(%s)",
			__FILE__, __LINE__, myname, strerror(status));
		return (-1);
	}

	return (n);
}

void acl_pthread_pool_set_stacksize(acl_pthread_pool_t *thr_pool, size_t size)
{
	if (thr_pool && size > 0)
		acl_pthread_attr_setstacksize(&thr_pool->attr, size);
}

void acl_pthread_pool_attr_init(acl_pthread_pool_attr_t *attr)
{
	if (attr)
		memset(attr, 0, sizeof(acl_pthread_pool_attr_t));
}

void acl_pthread_pool_attr_set_stacksize(acl_pthread_pool_attr_t *attr, size_t size)
{
	if (attr && size > 0)
		attr->stack_size = size;
}

void acl_pthread_pool_attr_set_threads_limit(acl_pthread_pool_attr_t *attr, int threads_limit)
{
	if (attr && threads_limit > 0)
		attr->threads_limit = threads_limit;
}

void acl_pthread_pool_attr_set_idle_timeout(acl_pthread_pool_attr_t *attr, int idle_timeout)
{
	if (attr && idle_timeout > 0)
		attr->idle_timeout = idle_timeout;
}
