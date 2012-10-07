/*
 * Name: acl_sane_socket.h
 * Author: zsx
 * Date: 2003-12-18
 * Version: 1.0
 */

#ifndef	__ACL_SANE_SOCKET_INCLUDE_H__
#define	__ACL_SANE_SOCKET_INCLUDE_H__

#ifdef	__cplusplus
extern "C" {
#endif

#include "stdlib/acl_define.h"

/**
 * ȡ���׽������ӶԷ��������ַ, ��ַ��ʽΪ: IP:PORT
 * @param sockfd {ACL_SOCKET} �����׽���
 * @param buf {char*} �洢��ַ�Ļ�����������Ϊ��
 * @param bsize {size_t} buf �ռ��С
 * @return {int} 0: ok; -1: error
 */
ACL_API int acl_getpeername(ACL_SOCKET sockfd, char *buf, size_t bsize);

/**
 * ȡ���׽������ӱ��ص������ַ, ��ַ��ʽΪ: IP:PORT
 * @param sockfd {ACL_SOCKET} �����׽���
 * @param buf {char*} �洢��ַ�Ļ�����������Ϊ��
 * @param bsize {size_t} buf �ռ��С
 * @return {int} 0: ok; -1: error
 */
ACL_API int acl_getsockname(ACL_SOCKET sockfd, char *buf, size_t bsize);

#ifdef	__cplusplus
}
#endif

#endif

