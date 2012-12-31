#ifndef	__ACL_APP_MAIN_INCLUDE_H__
#define	__ACL_APP_MAIN_INCLUDE_H__

#include "lib_acl.h"

#ifdef	__cplusplus
extern "C" {
#endif

#ifdef ACL_UNIX

/* �ͻ��˶�д��ʱʱ��ֵ */
extern int acl_app_var_client_idle_limit;

/* �û��������к�������, ���ú�������ֵ != 0 ʱ, ��ܻ��Զ��رտͻ����� */
typedef int (*ACL_IOCTL_RUN_FN)(ACL_VSTREAM *stream, void *run_ctx);

/* ������IO������ģ������к������� */
typedef int (*ACL_AIO_RUN_FN)(ACL_ASTREAM *stream, void *run_ctx);
typedef int (*ACL_AIO_RUN2_FN)(ACL_SOCKET fd, void *run_ctx);

/* �û����ĳ�ʼ���������� */
typedef void (*ACL_APP_INIT_FN)(void*);
typedef void (*ACL_APP_EXIT_FN)(void*);
typedef void (*ACL_APP_PRE_JAIL)(void*);

/* ʹ���û��Լ�����־������ */
typedef void (*ACL_APP_OPEN_LOG)(void);
typedef void (*ACL_APP_CLOSE_LOG)(void);

typedef ACL_MASTER_SERVER_THREAD_INIT_FN ACL_APP_THREAD_INIT_FN  /* void (*)(void*) */;
typedef ACL_MASTER_SERVER_THREAD_EXIT_FN ACL_APP_THREAD_EXIT_FN;  /* void (*)(void*) */

#define	ACL_APP_CTL_END			0	/* �������ƽ�����־ */
#define	ACL_APP_CTL_INIT_FN		1	/* ��ʼ������ */
#define	ACL_APP_CTL_INIT_CTX		2	/* ��ʼ���������õĲ��� */
#define	ACL_APP_CTL_CFG_BOOL		3	/* �������͵����ò����� */
#define	ACL_APP_CTL_CFG_INT		4	/* �������͵����ò����� */
#define	ACL_APP_CTL_CFG_STR		5	/* �ַ������͵����ò����� */
#define	ACL_APP_CTL_EXIT_FN		6	/* �������˳�ʱ�Ļص����� */
#define	ACL_APP_CTL_EXIT_CTX		7	/* �����˳�ʱ�ص������Ĳ��� */
#define	ACL_APP_CTL_THREAD_INIT		8	/* ÿ���߳�����ʱ�Ļص����� */
#define	ACL_APP_CTL_THREAD_INIT_CTX	9	/* �߳�����ʱ�ص������Ĳ��� */
#define	ACL_APP_CTL_THREAD_EXIT		10	/* �߳��˳�ʱ�Ļص����� */
#define	ACL_APP_CTL_THREAD_EXIT_CTX	11	/* �߳��˳�ʱ�ص������Ĳ��� */
#define	ACL_APP_CTL_DENY_INFO		12	/* ���Ƿ��ͻ��˷���ʱ��������ʾ��Ϣ */
#define	ACL_APP_CTL_OPEN_LOG		13	/* ��־��ʼ��������ͨ���˲��������û��Լ�����־���� */
#define	ACL_APP_CTL_CLOSE_LOG		14	/* �ر��û��Լ�����־���� */
#define	ACL_APP_CTL_CFG_INT64		15	/* 64 λ�������͵����ò����� */
#define	ACL_APP_CTL_ON_ACCEPT		16	/* ���ûص������������½��յ���Զ������ */
#define	ACL_APP_CTL_PRE_JAIL		17	/* ���ý����л����ǰ�Ļص����� */
#define	ACL_APP_CTL_PRE_JAIL_CTX	18	/* ���ý����л����ǰ�ص������Ĳ��� */
#define	ACL_APP_CTL_ON_TIMEOUT		19	/* �� IO ��д��ʱʱ���ƵĻص����� */
#define	ACL_APP_CTL_ON_CLOSE		20	/* �� IO �ر�ʱ���ƵĻص����� */
/*----------------------------------------------------------------------------*/
/* in app_main.c */

/**
 * ���������, �û����ĳ�ʼ������ָ�뼰���к���ָ��ͨ�����Ʋ�������ע��, �������ڲ�
 * ���ڳ�ʼ��ʱ�Զ������û�����ʼ������(ACL_APP_INIT_FN ����), �����յ�������ʵĿͻ���
 * ����ʱ���Զ������û�(ACL_APP_RUN_FN ����).
 * �������к���.
 * @param argc "int main(int argc, char *argv[])" �е� argc
 * @param argv "int main(int argc, char *argv[])" �е� argv
 * @param run_fn �û�������������
 * @param run_ctx run_fn() ����ʱ�Ĳ���֮һ
 * @param name ���Ʋ����еĵ�һ����������, ��֧�ֵ��������϶���: ACL_APP_CTL_XXX
 *        ���÷�ʽ: ACL_APP_CTL_XXX, xxx; ���� ACL_APP_CTL_END Ϊ����Ŀ��Ʋ���, ��ʾ���Ʋ���
 *        ����.
 * @example:
 *   acl_xxx_app_main(argc, argv, {run_fn}, {run_ctx},
 *		ACL_APP_CTL_INIT_FN, {run_init_fn},
 *		ACL_APP_CTL_INIT_CTX, {run_init_ctx},
 *		ACL_APP_CTL_END);
 * ע: acl_xxx_app_main() �����в�����, argc, argv, run_fn, run_ctx(����ΪNULL), ACL_APP_CTL_END
 *     ���Ǳ����.
 */

void acl_ioctl_app_main(int argc, char *argv[], ACL_IOCTL_RUN_FN run_fn, void *run_ctx, int name, ...);
void acl_aio_app_main(int argc, char *argv[], ACL_AIO_RUN_FN run_fn, void *run_ctx, int name, ...);
void acl_aio_app2_main(int argc, char *argv[], ACL_AIO_RUN2_FN run2_fn, void *run_ctx, int name, ...);

/*----------------------------------------------------------------------------*/

#endif

#ifdef	__cplusplus
}
#endif

#endif
