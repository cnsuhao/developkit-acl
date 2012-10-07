#ifndef __ACL_VSTREAM_NET_INCLUDE_H__
#define __ACL_VSTREAM_NET_INCLUDE_H__
#ifdef __cplusplus
extern "C" {
#endif

#include "stdlib/acl_define.h"
#include "stdlib/acl_vstream.h"

/**
 * ����ĳ����ַ������UNIX�������Լ������׽��֣�
 * @param addr {const char*} ������ַ,
 *  �磺127.0.0.1:80; �����׽���(UNIXƽ̨) �磺/tmp/test.sock
 * @param qlen {int} �������еĳ���
 * @param block_mode {int} �������������Ƿ���������, ACL_BLOCKING: ����ģʽ,
 *  ACL_NON_BLOCKING: ������ģʽ
 * @param io_bufsize {int} ���յ��µĿͻ����׽��ֵ�IO��������С
 * @param io_timeout {int} ���յ��µĿͻ����׽��ֵ�IO��д��ʱʱ�䣬��λΪ��
 * @return {ACL_VSTREAM*} ������ָ��
 */
ACL_API ACL_VSTREAM *acl_vstream_listen_ex(const char *addr,
					int qlen,
					int block_mode,
					int io_bufsize,
					int io_timeout);

/**
 * ����ĳ����ַ������UNIX�������Լ������׽��֣�
 * @param addr {const char*} ������ַ
 *  �磺127.0.0.1:80, �����׽���, �磺/tmp/test.sock
 * @param qlen {int} �������еĳ���
 * @return {ACL_VSTREAM*} ������ָ��
 */
ACL_API ACL_VSTREAM *acl_vstream_listen(const char *addr, int qlen);

/**
 * �Ӽ������н���һ���ͻ���������
 * @param listen_stream {ACL_VSTREAM*} ������
 * @param client_stream {ACL_VSTREAM*} ���ظ����õ� ACL_VSTREAM �ṹ��
 *  ���Ϊ�����ڲ�����һ���µ� ACL_VSTREAM ���������øýṹ�ռ�
 * @param ipbuf {char*} �����Ϊ���������洢�ͻ��˵�IP��ַ
 * @param bsize {int} ��� ipbuf ��Ϊ�գ����ʾ ipbuf �Ŀռ��С
 * @return {ACL_VSTREAM*} �����Ϊ�����ʾ�½��յĿͻ�����
 */
ACL_API ACL_VSTREAM *acl_vstream_accept_ex(ACL_VSTREAM *listen_stream,
					ACL_VSTREAM *client_stream,
					char *ipbuf,
					int bsize);

/**
 * �Ӽ������н���һ���ͻ���������
 * @param listen_stream {ACL_VSTREAM*} ������
 * @param ipbuf {char*} �����Ϊ���������洢�ͻ��˵�IP��ַ
 * @param bsize {int} ��� ipbuf ��Ϊ�գ����ʾ ipbuf �Ŀռ��С
 * @return {ACL_VSTREAM*} �����Ϊ�����ʾ�½��յĿͻ�����
 */
ACL_API ACL_VSTREAM *acl_vstream_accept(ACL_VSTREAM *listen_stream,
					char *ipbuf,
					int bsize);

/**
 * Զ�����ӷ�����
 * @param addr {const char*} ��������ַ, �������һ�����׽ӿڷ�����(��UNIXƽ̨),
 *  ���׽ӵ�ַ��/tmp/test.sock; �������һ��TCP�����������ַ��ʽΪ:
 *  [${local_ip}@]${remote_addr}, ��: 60.28.250.199@www.sina.com:80, ��˼�ǰ󶨱���
 *  ������ַΪ: 60.28.250.199, Զ������ www.sina.com �� 80 �˿�, �����OS�Զ��󶨱���
 *  IP ��ַ�������дΪ��www.sina.com:80
 * @param block_mode {int} �������ӻ��Ƿ��������ӣ�ACL_BLOCKING, ACL_NON_BLOCKING
 * @param connect_timeout {int} ���ӳ�ʱʱ��(��)
 * @param rw_timeout {int} �������ɹ���Ķ�д��ʱʱ�䣬��λΪ��
 * @param rw_bufsize {int} �������ɹ���Ļ�������С
 * @param he_errorp {int*} �����Ϊ�գ���洢����ʧ�ܺ�Ĵ����
 * @return {ACL_VSTREAM*} �����Ϊ�գ����ʾ���ӳɹ����������
 */
ACL_API ACL_VSTREAM *acl_vstream_connect_ex(const char *addr,
					int block_mode,
					int connect_timeout,
					int rw_timeout,
					int rw_bufsize,
					int *he_errorp);

/**
 * Զ�����ӷ�����
 * @param addr {const char*} ��������ַ����ʽ�磺127.0.0.1��
 *  �� ���׽ӵ�ַ��/tmp/test.sock
 * @param block_mode {int} �������ӻ��Ƿ��������ӣ�ACL_BLOCKING, ACL_NON_BLOCKING
 * @param connect_timeout {int} ���ӳ�ʱʱ��(��)
 * @param rw_timeout {int} �������ɹ���Ķ�д��ʱʱ�䣬��λΪ��
 * @param rw_bufsize {int} �������ɹ���Ļ�������С
 * @return {ACL_VSTREAM*} �����Ϊ�գ����ʾ���ӳɹ����������
 */
ACL_API ACL_VSTREAM *acl_vstream_connect(const char *addr,
					int block_mode,
					int connect_timeout,
					int rw_timeout,
					int rw_bufsize);

#ifdef __cplusplus
}
#endif
#endif

