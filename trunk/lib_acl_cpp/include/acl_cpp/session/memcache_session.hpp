#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/session/session.hpp"

namespace acl {

class memcache;

/**
 * session �࣬����ʹ�� memcached �洢 session ����
 */
class ACL_CPP_API memcache_session : public session
{
public:
	/**
	 * ���캯��
	 * @param cache_addr {const char*} memcached �����ַ����ʽ��
	 *  IP:PORT������Ϊ��
	 * @param prefix {const char*} �� memcached �洢�ļ�ֵ��ǰ׺
	 * @param conn_timeout {int} ���� memcached �ĳ�ʱʱ��(��)
	 * @param rw_timeout {int} �� memcached ͨѶ�� IO ��ʱʱ��(��)
	 * @param ttl {time_t} ��������(��)
	 * @param sid {const char*} session ��Ӧ�� sid����Ϊ��ʱ���ڲ�
	 *  ���Զ�����һ��������˵����ο����� session ��˵��
	 * @param encode_key {bool} �Ƿ�Դ洢�� memcached �ļ�ֵ���б���
	 */
	memcache_session(const char* cache_addr, int conn_timeout = 180,
		int rw_timeout = 300, const char* prefix = NULL,
		time_t ttl = 0, const char* sid = NULL, bool encode_key = true);

	/**
	 * ������� memcached �����Ӷ���Ϊ�����Ĺ��캯��
	 * @param cache {memcache*} ����� memcached ���Ӷ���
	 * @param auto_free {bool} ���ò���Ϊ true ʱ����Ҫ���
	 *  memcached_session ���������������ͷŴ���� cache ����
	 *  �������ֹ�� memcached_session �������������ͷ� cache ����
	 * @param ttl {time_t} ��������(��)
	 * @param sid {const char*} session ��Ӧ�� sid����Ϊ��ʱ���ڲ�
	 *  ���Զ�����һ��������˵����ο����� session ��˵��
	 */
	memcache_session(memcache* cache, bool auto_free = false,
		time_t ttl = 0, const char* sid = NULL);

	~memcache_session(void);

private:
	// ���ി�麯������ memcached �л������
	bool get_attrs(const char* sid, std::map<string, VBUF*>& attrs);

	// ���ി�麯������ memcached ����ӻ��޸�����
	bool set_attrs(const char* sid, std::map<string, VBUF*>& attrs,
		time_t ttl);

	// ���ി�麯������ memcached ��ɾ������
	bool del_key(const char* sid);

	//�������� session �� memcached �ϵĻ���ʱ��
	bool set_timeout(const char* sid, time_t ttl);

private:
	memcache* cache_;
	bool auto_free_;
};

} // namespace acl
