#pragma once
#include "acl_cpp/acl_cpp_define.hpp"

namespace acl {

#ifdef WIN32

#include <stdarg.h>

/**
 * ��׼ C snprintf API ��װ��ͬʱ���� WIN32 �߰汾 VC �µİ�ȫģʽ�����Ա�֤���������
 * �е����һ���ֽ�Ϊ '\0'
 * @param buf {char*} �洢����Ļ�����
 * @param size {size_t} buf ����������
 * @param fmt {const char*} ��θ�ʽ�ַ���
 * @return {int} ������������(�� size ֵ) �ܹ����������������ܱ�֤β���� '\0' ����ʱ��
 *  �򷵻ش洢���������е�ʵ�����ݳ��ȣ����򷵻� -1��ͬʱ�����������һ���ֽڱ��� '\0'��
 *  ��Ҫע��÷���ֵ�ĺ����� UNIX �µĲ�ͬ
 */
ACL_CPP_API int ACL_CPP_PRINTF(3, 4) snprintf(char *buf, size_t size,
	const char *fmt, ...);

/**
 * ��׼ C snprintf API ��װ��ͬʱ���� WIN32 �߰汾 VC �µİ�ȫģʽ�����Ա�֤���������
 * �е����һ���ֽ�Ϊ '\0'
 * @param buf {char*} �洢����Ļ�����
 * @param size {size_t} buf ����������
 * @param fmt {const char*} ��θ�ʽ�ַ���
 * @param ap {va_list} ��α���
 * @return {int} ������������(�� size ֵ) �ܹ����������������ܱ�֤β���� '\0' ����ʱ��
 *  �򷵻ش洢���������е�ʵ�����ݳ��ȣ����򷵻� -1��ͬʱ�����������һ���ֽڱ��� '\0'��
 *  ��Ҫע��÷���ֵ�ĺ����� UNIX �µĲ�ͬ
 */
ACL_CPP_API int vsnprintf(char *buf, size_t size,
	const char *fmt, va_list ap);

#else  // UNIX/LINUX

/**
 * ��׼ C snprintf API ��װ�����Ա�֤����������е����һ���ֽ�Ϊ '\0'
 * @param buf {char*} �洢����Ļ�����
 * @param size {size_t} buf ����������
 * @param fmt {const char*} ��θ�ʽ�ַ���
 * @return {int} �����������ȹ���ʱ����ʵ�ʿ��������ݳ��ȣ����򷵻�ʵ����Ҫ�Ļ��������ȣ�
 *  �������������Ȳ���ʱ����ֵ >= size����Ҫע��÷���ֵ�ĺ����� WIN32 �µĲ�ͬ
 */
ACL_CPP_API int ACL_CPP_PRINTF(3, 4) snprintf(char *buf, size_t size,
	const char *fmt, ...);

/**
 * ��׼ C snprintf API ��װ�����Ա�֤����������е����һ���ֽ�Ϊ '\0'
 * @param buf {char*} �洢����Ļ�����
 * @param size {size_t} buf ����������
 * @param fmt {const char*} ��θ�ʽ�ַ���
 * @param ap {va_list} ��α���
 * @return {int} �����������ȹ���ʱ����ʵ�ʿ��������ݳ��ȣ����򷵻�ʵ����Ҫ�Ļ��������ȣ�
 *  �������������Ȳ���ʱ����ֵ >= size����Ҫע��÷���ֵ�ĺ����� WIN32 �µĲ�ͬ
 */
ACL_CPP_API int vsnprintf(char *buf, size_t size,
	const char *fmt, va_list ap);

#endif

}  // namespace acl
