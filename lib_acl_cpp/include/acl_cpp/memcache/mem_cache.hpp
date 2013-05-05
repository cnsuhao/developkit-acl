#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <time.h>
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/mime/rfc2047.hpp"

namespace acl {

class socket_stream;

/**
 * memcached �ͻ���ͨ��Э��⣬֧�ֳ��������Զ�����
 */
class ACL_CPP_API mem_cache
{
public:

	/**
	* ���캯��
	* @param key_pre {const char*} ÿһ��Ӧ�ö�Ӧ�����Լ��ļ�ֵǰ׺��
	*  �Ա�����ʹ��memcacheʱ��������Ӧ�����𿪣�Ҳ���������Ӧ�õ�
	*  �������ظ����ü�ǰ׺����������е�Ӧ�ó�����˵��Ψһ�ģ����
	*  �� receiver_server ������˵�������� receiver_server�����Է���
	*  �ؿ�������Ӧ�õĺ�����
	* @param addr {const char*} memcached ������������ַ����ʽΪ��
	*  ip:port����: 127.0.0.1:11211
	* @param retry {bool} ������ԭ������ΪIO����ʱ�Ƿ���Ҫ���Բ���
	* @param conn_timeout {int} ���ӳ�ʱʱ��(��)
	* @param rw_timeout {int} IO ��д��ʱʱ��(��)
	* @param encode_key {bool} �Ƿ�Բ�ѯ��ֵ���б���
	*/
	mem_cache(const char* key_pre = NULL, const char* addr = "127.0.0.1:11211",
		bool retry = true, int conn_timeout = 180, int rw_timeout = 300,
		bool encode_key = true);
	~mem_cache();

	/**
	* �� memcached ���޸Ļ�����µ����ݻ������
	* @param key {const char*} ��ֵ
	* @param klen {size_t} key ��ֵ����
	* @param dat {const void*} ����
	* @param dlen {size_t} data ���ݳ���
	* @param timeout {time_t} ���泬ʱʱ��(��)
	* @param flags {unsigned short} �����ı�־λ
	* @return {bool} �Ƿ�ɹ�
	*/
	bool set(const char* key, size_t klen,
		const void* dat, size_t dlen,
		time_t timeout = 0, unsigned short flags = 0);

	/**
	* �� memcached ���޸Ļ�����µ����ݻ������
	* @param key {const char*} �ַ�����ֵ
	* @param dat {const void*} ����
	* @param dlen {size_t} data ���ݳ���
	* @param timeout {time_t} ���泬ʱʱ��(��)
	* @param flags {unsigned short} �����ı�־λ
	* @return {bool} �Ƿ�ɹ�
	*/
	bool set(const char* key, const void* dat, size_t dlen,
		time_t timeout = 0, unsigned short flags = 0);

	/**
	* ���� memcached ���Ѿ����ڵļ��Ĺ������ڣ���ΪĿǰ libmemcached û��
	* �ṩ�˽ӿڣ����Ըú���ʵ�ֵķ�ʽ���ȵ��� get ȡ����Ӧ����ֵ��Ȼ����
	* ���� set �������øü���ֵ������ʱ��
	* @param key {const char*} ��ֵ
	* @param klen {size_t} key ��ֵ����
	* @param timeout {time_t} ����ʱ��(��)
	* @return {bool} �Ƿ�ɹ�
	*/
	bool set(const char* key, size_t klen, time_t timeout = 0);

	/**
	* ���� memcached ���Ѿ����ڵļ��Ĺ������ڣ���ΪĿǰ libmemcached û��
	* �ṩ�˽ӿڣ����Ըú���ʵ�ֵķ�ʽ���ȵ��� get ȡ����Ӧ����ֵ��Ȼ����
	* ���� set �������øü���ֵ������ʱ��
	* @param key {const char*} �ַ�����ֵ
	* @param timeout {time_t} ����ʱ��(��)
	* @return {bool} �Ƿ�ɹ�
	*/
	bool set(const char* key, time_t timeout = 0);

	/**
	* �� memcached �л�ö�Ӧ��ֵ�Ļ�������
	* @param key {const char*} �ַ�����ֵ
	* @param klen {size_t} ��ֵ����
	* @param buf {string&} �洢����Ļ��������ڲ����Ȼ���ոû�����
	* @param flags {unsigned short*} �洢�����ı�־λ
	* @return {bool} ���� true ��ʾ��ȷ��ý��ֵ�������ʾ��ֵ��Ӧ��
	*  ���ݲ����ڻ����
	*/
	bool get(const char* key, size_t klen, string& buf,
		unsigned short* flags = NULL);

	/**
	* �� memcached �л�ö�Ӧ��ֵ�Ļ�������
	* @param key {const char*} �ַ�����ֵ
	* @param buf {string&} �洢����Ļ��������ڲ����Ȼ���ոû�����
	* @param flags {unsigned short*} �洢�����ı�־λ
	* @return {bool} ���� true ��ʾ��ȷ��ý��ֵ�������ʾ��ֵ��Ӧ��
	*  ���ݲ����ڻ����
	*/
	bool get(const char* key, string& buf, unsigned short* flags = NULL);

	/**
	* �� memcached ��ɾ������
	* @param key {const char*} ��ֵ
	* @param klen {size_t} ��ֵ����
	* @return {bool} ɾ���Ƿ�ɹ�
	*/
	bool del(const char* key, size_t klen);

	/**
	* �� memcached ��ɾ������
	* @param key {const char*} �ַ�����ֵ
	* @return {bool} ɾ���Ƿ�ɹ�
	*/
	bool del(const char* key);

	/**
	* ����ϴβ��� memcached ����������Ϣ
	* @return {const char*} ����������Ϣ������Ϊ��
	*/
	const char* last_serror() const;

	/**
	* ����ϴβ��� memcached �Ĵ����
	* @return {int} �����
	*/
	int  last_error() const;

	/**
	* ���� memcached ������, ��Ϊ set/get/del ���������Զ�����
	* memcached �����ӣ����Բ�����ʾ�ص��ô˺��������� memcached
	* ������
	* @return {bool} ���Ƿ�ɹ�
	*/
	bool open();

	/**
	* �ر��� memcached �����ӣ�һ��ú�������Ҫ���ã���Ϊ�������
	* ����ʱ���Զ����ô˺���
	*/
	void close();

	/**
	* �г� memcached ���ӵ�һЩ���ԣ�������
	*/
	void property_list();

private:
	bool set(const string& key, const void* dat, size_t dlen,
		time_t timeout, unsigned short flags);
	bool get(const string& key, string& buf, unsigned short* flags);
	const string& get_key(const char* key, size_t klen);

	string* m_key_pre;
	rfc2047 m_coder;
	int   m_conn_timeout;
	int   m_rw_timeout;
	bool  m_encode_key;

	bool  m_opened;
	bool  m_retry;
	char* m_addr;
	char* m_ip;
	int   m_port;
	int   m_enum;
	string m_ebuf;
	string m_kbuf;

	socket_stream* m_conn;
	string m_line;
	bool error_happen(const char* line);
};

} // namespace acl
