#pragma once
#include "acl_cpp_define.hpp"
#include <map>
#include "string.hpp"

namespace acl
{

	// �����洢����ֵ�Ļ����������壬����Ҫ��Ϊ�˼�������ֵ
	// ����Ϊ�����Ƶ����ζ����ӵĽṹ����
	struct VBUF
	{
		size_t len;
		char  buf[1];
	};

	/**
	 * session �࣬����ʹ�� memcached �洢 session ����
	 */
	class ACL_CPP_API session
	{
	public:
		/**
		 * �ڵ��ñ����캯�����ڲ����Զ�����һ��Ψһ ID,
		 * �û�Ӧ������ ID (ͨ�� get_sid() ���)��Ȼ���Ժ�ÿ��
		 * ����ʱӦ���ȵ��� set_sid(const char*) ������һ���뱾
		 * session ������ ID
		 */
		session(time_t ttl = 0);
		virtual ~session(void);
		
		/**
		 * �����ڲ�״̬�������һЩ��ʱ����
		 */
		void reset(void);

		/**
		 * ��ñ� session �����Ψһ ID ��ʶ
		 * @return {const char*} �ǿ�
		 */
		virtual const char* get_sid(void) const;

		/**
		 * ���ñ� session �����Ψһ ID ��ʶ�����������º���ʱ��
		 * Ӧ�ñ����ȵ��� set_sid(const char*) ���ñ� session ����
		 * ��Ӧ�����ݶ����ʶ id
		 * @param sid {const char*} �ǿ�
		 */
		void set_sid(const char* sid);

		/**
		 * �� session ������µ��ַ������ԣ�ͬʱ���ø�
		 * session �Ĺ���ʱ����(��)
		 * @param name {const char*} session �����ǿ�
		 * @param value {const char*} session ֵ���ǿ�
		 * @return {bool} ���� false ��ʾ����
		 */
		bool set(const char* name, const char* value);

		/**
		 * �� session ������µ����Զ���ͬʱ���ø�
		 * session �Ĺ���ʱ����(��)
		 * @param name {const char*} session ���������ǿ�
		 * @param value {const char*} session ����ֵ���ǿ�
		 * @param len {size_t} value ֵ����
		 * @return {bool} ���� false ��ʾ����
		 */
		bool set(const char* name, const void* value, size_t len);
		
		/**
		 * �� session ��ȡ���ַ�����������ֵ
		 * @param name {const char*} session ���������ǿ�
		 * @return {const char*} session ����ֵ�����ؿ�ʱ��ʾ����򲻴���
		 */
		const char* get(const char* name);

		/**
		 * �� session ��ȡ�ö������������͵�����ֵ
		 * @param name {const char*} session ���������ǿ�
		 * @return {const VBUF*} session ����ֵ�����ؿ�ʱ��ʾ����򲻴���
		 */
		const VBUF* get_vbuf(const char* name);

		/**
		 * �� session ��ɾ��ָ������ֵ�������еı�����ɾ��
		 * ʱ�Ὣ��������� memcached ��ɾ��
		 * @param name {const char*} session ���������ǿ�
		 * @return {bool} true ��ʾ�ɹ�(�����������)��false ��ʾɾ��ʧ��
		 */
		bool del(const char* name);

		/**
		 * �������� session �ڻ���������ϵĻ���ʱ��
		 * @param ttl {time_t} ��������(��)
		 * @return {bool} �����Ƿ�ɹ�
		 */
		bool set_ttl(time_t ttl);

		/**
		 * ��ñ� session �����м�¼�� session �������ڣ���ֵ�п���
		 * �������洢�ڻ����������ʱ�䲻һ�£���Ϊ�п���������ʵ��
		 * ���������� session �ڻ���������ϵ���������
		 * @return {time_t}
		 */
		time_t get_ttl(void) const;

		/**
		 * ʹ session �ӷ���˵Ļ�����ɾ����ʹ session ʧЧ
		 * @return {bool} �Ƿ�ʹ session ʧЧ
		 */
		bool remove(void);

	protected:
		// ��ö�Ӧ sid ������
		virtual bool get_data(string& buf) = 0;

		// ���ö�Ӧ sid ������
		virtual bool set_data(const char* buf, size_t len, time_t ttl) = 0;

		// ɾ����Ӧ sid ������
		virtual bool del_data(void) = 0;

		// ���ö�Ӧ sid ���ݵĹ���ʱ��
		virtual bool set_timeout(time_t ttl) = 0;

	private:
		time_t ttl_;
		char sid_[256];
		std::map<string, VBUF*> attrs_;

		// �� session �������л�
		void serialize(string& buf);

		void serialize(const char* name, const void* value,
			size_t len, string& buf);

		// �� session ���ݷ����л�
		bool deserialize(string& buf);

		// �����ڴ����
		static VBUF* vbuf_new(const void* str, size_t len);

		// �ͷ��ڴ����
		static void vbuf_free(VBUF* buf);
	};

} // namespace acl
