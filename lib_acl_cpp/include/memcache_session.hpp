#pragma once
#include "acl_cpp_define.hpp"
#include "session.hpp"

namespace acl
{

	class mem_cache;

	/**
	 * session �࣬����ʹ�� memcached �洢 session ����
	 */
	class ACL_CPP_API memcache_session : public session
	{
	public:
		/**
		 * ��������
		 * @param cache_addr {const char*} memcached �����ַ����ʽ��
		 *  IP:PORT������Ϊ��
		 * @param prefix {const char*} �� memcached �洢�ļ�ֵ��ǰ׺
		 * @param ttl {time_t} ��������(��)
		 * @param sid {const char*} session ��Ӧ�� sid����Ϊ��ʱ���ڲ�
		 *  ���Զ�����һ��������˵����ο����� session ��˵��
		 */
		memcache_session(const char* cache_addr, const char* prefix = NULL,
			time_t ttl = 0, const char* sid = NULL);
		~memcache_session(void);

	private:
		// ���ി�麯������ memcached �л������
		virtual bool get_data(string& buf);

		// ���ി�麯������ memcached ����ӻ��޸�����
		virtual bool set_data(const char* buf, size_t len, time_t ttl);

		// ���ി�麯������ memcached ��ɾ������
		virtual bool del_data(void);

		//�������� session �ڻ���������ϵĻ���ʱ��
		virtual bool set_timeout(time_t ttl);
	private:
		mem_cache* cache_;
	};

} // namespace acl
