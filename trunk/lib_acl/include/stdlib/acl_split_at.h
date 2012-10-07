#ifndef __ACL_SPLIT_AT_H_INCLUDED__
#define __ACL_SPLIT_AT_H_INCLUDED__

#ifdef  __cplusplus
extern "C" {
#endif

#include "acl_define.h"

/**
 * ���ַ�����߿�ʼ�����������ָ������ڵ��ұ߽ض�
 * @param string {char*} Դ�ַ���
 * @param delimiter {int} �ָ���
 * @return {char*} �ָ������ҵ��ַ�������ΪNULLʱ����δ�ҵ�ָ���ָ���
 */
ACL_API char *acl_split_at(char *string, int delimiter);

/**
 * ���ַ����ұ߿�ʼ�����������ָ������ڵ��ұ߽ض�
 * @param string {char*} Դ�ַ���
 * @param delimiter {int} �ָ���
 * @return {char*} �ָ������ҵ��ַ�������ΪNULLʱ����δ�ҵ�ָ���ָ���
 */
ACL_API char *acl_split_at_right(char *string, int delimiter);

#ifdef  __cplusplus
}
#endif

#endif

