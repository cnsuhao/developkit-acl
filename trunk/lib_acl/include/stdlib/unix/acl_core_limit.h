#ifndef	__ACL_CORE_LIMIT_INCLUDE__
#define	__ACL_CORE_LIMIT_INCLUDE__

#ifdef  __cplusplus
extern "C" {
#endif

#include "../acl_define.h"
#ifdef ACL_UNIX

/**
 * ���ô˺������ó������ʱ������ core �ļ������ֵ
 * @param max {size_t} ����ֵ == 0 ʱ����ʹ��ϵͳ�涨�����ֵ������ʹ�ø�ֵ
 */
void acl_set_core_limit(size_t max);

#endif  /* ACL_UNIX */

#ifdef  __cplusplus
}
#endif

#endif