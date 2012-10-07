
#ifndef __ACL_WORK_QUEUE_H_INCLUDED__
#define __ACL_WORK_QUEUE_H_INCLUDED__

#include "stdlib/acl_define.h"
#include <time.h>

#ifdef	ACL_UNIX
#include <pthread.h>
#endif
#include "acl_task.h"

#ifdef	__cplusplus
extern "C" {
#endif

/**
 * �ýṹ�ɹ����̲߳�����ά�����û������ڵ��� acl_workq_atinit() ʱ��ע��Ļص�
 * ���� worker_init_fn() ������ʱ��Ϊһ��ָ��������ݸ��û����û����Խ������߳�
 * ������ʼ������(�����ص��� worker_init_fn ())�н��Լ��ĳ�ʼ����������ֵ����
 * �ṹָ���е� init_data ��Ա�����������������߳��ڽ���������(������worker_fn())
 * ʱ�Ὣ�û��� init_data �ĳ�Ա��ֵ������ ACL_WORKER_ATTR �ṹָ������û���������.
 */
typedef struct ACL_WORKER_ATTR {
#ifdef	ACL_UNIX
	pthread_t id;		/* ��ǰ�����̵߳��̺߳� */
#elif	defined(ACL_MS_WINDOWS)
	unsigned long id;	/* ��ǰ�����̵߳��̺߳� */
#else
# error "unknown OS"
#endif
	unsigned int count;	/* ��ǰ�����̵߳���������� */
	time_t begin_t;		/* ��ǰ�����߳̿�ʼ����ʱ��ʱ�� */
	time_t end_t;		/* ��ǰ�����߳��˳�ʱ��ʱ�� */
	time_t idle_t;		/* ��ǰ�����̵߳Ŀ��еȴ�ʱ�� */
	void *init_data;	/* �û����̳߳�ʼ��ʱ���ݹ������������ */
	void *free_data;	/* �߳��˳�ʱ�û����ݹ����ı��� */
	void *run_data;	
} ACL_WORKER_ATTR;

/* #define	ACL_THREAD_IDLE_TIMEOUT		10 */

typedef struct ACL_WORK_ELEMENT ACL_WORK_ELEMENT;
typedef struct ACL_WORK_QUEUE   ACL_WORK_QUEUE;

/**
 * ��ʼ��һ���̳߳ض���
 * @param threads ���̳߳ض��������߳���
 * @param idle_timeout �����̵߳Ŀ���ʱ�䣬��λΪ��
 * @param poller_fn ѭ�����������еĻص�����
 * @param poller_arg poller_fn ����Ҫ�Ĳ���
 * @return ���� ACL_WORK_QUEUE ���͵�ָ��, �����Ϊ�����ʾ�ɹ�������ʧ��
 */
ACL_API ACL_WORK_QUEUE *acl_workq_create(int threads,
					int idle_timeout,
					int (*poller_fn)(void *),
					void *poller_arg);

/**
 * �����жѻ������������ڿ����߳�����2��ʱ. ͨ���˺���������������
 * �߳�����ʱ��, ��������ô˺�����������, ������̲߳����������״̬.
 * @param wq �̳߳ض��󣬲���Ϊ��
 * @param timewait_sec ���ߡ���ʱ��ֵ, ���齫��ֵ����Ϊ 1--5 ����
 * @return �ɹ����� 0, ʧ�ܷ��� -1
 */
ACL_API int acl_workq_set_timewait(ACL_WORK_QUEUE *wq, int timewait_sec);

/**
 * ���ע�ắ�������̴߳���������ִ�д˳�ʼ������
 * @param wq �̳߳ض��󣬲���Ϊ��
 * @param worker_init_fn �����̳߳�ʼ������, ����ú������� < 0, ����߳��Զ��˳���
 * @param worker_init_arg worker_init_fn ����Ҫ�Ĳ���
 * @return == 0, OK; != 0, Err.
 * ע: acl_workq_atinit() Ӧ�ڵ��� acl_workq_create() ����������
 *     worker_free_fn �еĵڶ�������ΪACL_WORKER_ATTR �ṹָ�룬���̳߳ص�ĳ��
 *     �����߳����ɣ��û����Խ��Լ��������������͸�ֵΪ�ýṹָ��� init_data ��Ա,
 *     �磺���ӳض���ȡ�
 */
ACL_API int acl_workq_atinit(ACL_WORK_QUEUE *wq,
				int (*worker_init_fn)(void *, ACL_WORKER_ATTR *),
				void *worker_init_arg);

/**
 * ���ע�ắ�������߳��˳�����ִ�д˳�����
 * @param wq �̳߳ض��󣬲���Ϊ��
 * @param worker_free_fn �����߳��˳�ǰ����ִ�еĺ���
 * @param worker_free_arg worker_free_fn ����Ҫ�Ĳ���
 * @return == 0, OK; != 0, Err.
 * ע: acl_workq_atfree() Ӧ�ڵ��� acl_workq_create() ����������
 *     worker_free_fn �еĵڶ�������ΪACL_WORKER_ATTR �ṹָ�룬���̳߳ص�ĳ��
 *     �����߳����ɣ��û����Խ��Լ��������������͸�ֵΪ�ýṹָ��� init_data ��Ա,
 *     �磺���ӳض���ȡ�
 */
ACL_API int acl_workq_atfree(ACL_WORK_QUEUE *wq,
				void (*worker_free_fn)(void *, ACL_WORKER_ATTR *),
				void *worker_free_arg);

/**
 * ����һ���̳߳ض���, �ɹ����ٺ�ö���������.
 * @param wq �̳߳ض��󣬲���Ϊ��
 * @return 0 �ɹ�; != 0 ʧ��
 */
ACL_API int acl_workq_destroy(ACL_WORK_QUEUE *wq);

/**
 * ��ͣһ���̳߳ض��������, ֹͣ�󻹿���������.
 * @param wq �̳߳ض��󣬲���Ϊ��
 * @return 0 �ɹ�; != 0 ʧ��
 */
ACL_API int acl_workq_stop(ACL_WORK_QUEUE *wq);

/**
 * ���һ������
 * @param wq �̳߳ض��󣬲���Ϊ��
 * @param worker_fn ���п��ù����߳�ʱ�����õĻص�������
 * @param worker_arg �ص����� worker_fn() ����Ҫ�Ļص�����
 * @param event_type �¼����ͣ�һ�㲻�ع��Ĵ�ֵ
 * @return 0 �ɹ�; != 0 ʧ��
 * ע��worker_fn �еĵڶ�������ΪACL_WORKER_ATTR�ṹָ�룬���̳߳ص�ĳ��
 *     �����߳�ά�����ýṹָ���еĳ�Ա���� init_data Ϊ�û��ĸ�ֵ���ͱ�����
 *     �磺���ݿ����Ӷ���ȡ�
 */
ACL_API int acl_workq_add(ACL_WORK_QUEUE *wq,
			void (*worker_fn)(int, void *),
			int  event_type,
			void *worker_arg);

/**
 * ��ʼ����������ʽ���������, ʵ�����ǿ�ʼ���м���
 * @param wq �̳߳ض���ACL_WORK_QUEUE ���͵Ľṹָ��, ����Ϊ��
 */
ACL_API void acl_workq_batadd_begin(void *wq);

/**
 * ���һ��������, ǰ�����Ѿ��ɹ�����, ������ cl_workq_batadd_begin �ɹ�
 * @param wq �̳߳ض��󣬲���Ϊ��
 * @param worker_fn ���п��ù����߳�ʱ�����õĻص�������
 * @param worker_arg �ص����� worker_fn() ����Ҫ�Ļص�����
 * @param event_type �¼����ͣ�һ�㲻�ع��Ĵ�ֵ
 * ע��worker_fn �еĵڶ�������ΪACL_WORKER_ATTR�ṹָ�룬���̳߳ص�ĳ��
 *     �����߳�ά�����ýṹָ���еĳ�Ա���� init_data Ϊ�û��ĸ�ֵ���ͱ�����
 *     �磺���ݿ����Ӷ���ȡ�
 */
ACL_API void acl_workq_batadd_one(ACL_WORK_QUEUE *wq,
		void (*worker_fn)(int, void *),
		int  event_type,
		void *worker_arg);
/**
 * ��������ӽ���
 * @param wq �̳߳ض���ACL_WORK_QUEUE ���͵Ľṹָ��, ����Ϊ��
 */
ACL_API void acl_workq_batadd_end(void *wq);

/**
 * ����һ���̳߳��������
 * @param wq �̳߳ض��󣬲���Ϊ��
 * @return 0 �ɹ�; != 0 ʧ��, ���ԶԷ���ֵ���� strerror(ret) ȡ�ô���ԭ������
 */
ACL_API int acl_workq_start(ACL_WORK_QUEUE *wq);

/**
 * ��������ʽ��������ķַ�, ���ڲ���ʵ�ǵ����� acl_workq_batadd_one()
 * @param dispatch_arg �������ʱ�Ĳ���
 * @param worker_fn ����ص�����ָ��
 * @param event_type �¼����ͣ�һ�㲻�ع��Ĵ�ֵ
 * @param worker_arg worker_fn ����֮һ
 * @return 0: OK; -1: err
 * ע��worker_fn �еĵڶ�������ΪACL_WORKER_ATTR�ṹָ�룬���̳߳ص�ĳ��
 *     �����߳�ά�����ýṹָ���еĳ�Ա���� init_data Ϊ�û��ĸ�ֵ���ͱ�����
 *     �磺���ݿ����Ӷ���ȡ�
 */
ACL_API int acl_workq_batadd_dispatch(void *dispatch_arg,
		void (*worker_fn)(int, void *),
		int  event_type,
		void *worker_arg);

/**
 * �Ե�����ӵķ�ʽ��������ķַ�
 * @param dispatch_arg �������ʱ�Ĳ���
 * @param worker_fn ����ص�����ָ��
 * @param event_type �¼����ͣ�һ�㲻�ع��Ĵ�ֵ
 * @param worker_arg worker_fn ����֮һ
 * @return 0: OK; -1: err
 * ע��worker_fn �еĵڶ�������ΪACL_WORKER_ATTR�ṹָ�룬���̳߳ص�ĳ��
 *     �����߳�ά�����ýṹָ���еĳ�Ա���� init_data Ϊ�û��ĸ�ֵ���ͱ�����
 *     �磺���ݿ����Ӷ���ȡ�
 */
ACL_API int acl_workq_dispatch(void *dispatch_arg,
		void (*worker_fn)(int, void *),
		int  event_type,
		void *worker_arg);

/**
 * �˺���Ŀǰδ��
 * @param wq �̳߳ض��󣬲���Ϊ��
 * @deprecated
 */
ACL_API int acl_workq_quit_wait(ACL_WORK_QUEUE *wq);

/**
 * ��ǰ�̳߳��е��߳���
 * @param wq �̳߳ض��󣬲���Ϊ��
 * @return �����̳߳��е����߳���
 */
ACL_API int acl_workq_nworker(ACL_WORK_QUEUE *wq);

/**
 * �����̳߳����̵߳Ķ�ջ��С
 * @param stacksize {size_t} �̴߳���ʱ�Ķ�ջ��С����λΪ�ֽ�
 */
ACL_API void acl_workq_set_stacksize(size_t stacksize);

#ifdef	__cplusplus
}
#endif

#endif	/* !__ACL_WORK_QUEUE_H_INCLUDED__ */


