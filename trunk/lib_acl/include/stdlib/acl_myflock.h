#ifndef __ACL_MYFLOCK_H_INCLUDED__
#define __ACL_MYFLOCK_H_INCLUDED__

#ifdef  __cplusplus
extern "C" {
#endif

#include "acl_define.h"

/**
 * �Դ򿪵��ļ�������м���
 * @param fd {ACL_FILE_HANDLE} �ļ����
 * @param lock_style {int} ϵͳ�ṩ��API�������ͣ�����UNIX��Ч��
 *  ACL_MYFLOCK_STYLE_FLOCK or ACL_MYFLOCK_STYLE_FCNTL
 * @param operation {int} ����������ʽ, ACL_MYFLOCK_OP_XXX
 * @return {int} 0: �����ɹ�; -1: ����ʧ��
 */
ACL_API int acl_myflock(ACL_FILE_HANDLE fd, int lock_style, int operation);

/*
 * Lock styles.
 */
#define ACL_MYFLOCK_STYLE_FLOCK     1  /**< ���� flock ��������(unix) */
#define ACL_MYFLOCK_STYLE_FCNTL     2  /**< ���� fcntl ��������(unix) */

/*
 * Lock request types.
 */
#define ACL_MYFLOCK_OP_NONE         0  /**< ���� */
#define ACL_MYFLOCK_OP_SHARED       1  /**< ������ */
#define ACL_MYFLOCK_OP_EXCLUSIVE    2  /**< ���������� */
#define ACL_MYFLOCK_OP_NOWAIT       4  /**< �޵ȴ�����, ������
                                          ACL_MYFLOCK_OP_SHARED,
                                          �� ACL_MYFLOCK_OP_EXCLUSIVE ��� */

/**
 * ������ʽ��λ����
 */
#define ACL_MYFLOCK_OP_BITS \
	(ACL_MYFLOCK_OP_SHARED | ACL_MYFLOCK_OP_EXCLUSIVE | ACL_MYFLOCK_OP_NOWAIT)

#ifdef  __cplusplus
}
#endif

#endif

