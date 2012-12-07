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
		size_t size;
		char  buf[1];
	};

	/**
	 * session �࣬����ʹ�� memcached �洢 session ����
	 */
	class ACL_CPP_API session
	{
	public:
		/**
		 * �����캯���Ĳ��� sid �ǿ�ʱ����� session ����ʹ�ø�
		 * sid�������ڲ����Զ�����һ�� sid���û�Ӧ��ͨ�� get_sid()
		 * �������Զ����ɵ� sid �Ա���ÿ�β�ѯ�� sid ��Ӧ������
		 * @param ttl {time_t} ָ�� session ����������(��)
		 * @param sid {const char*} �ǿ�ʱ���� session �� sid ʹ
		 *  ��ֵ�������ڲ������һ������� session sid���������
		 *  sid ����ͨ������ get_sid() ��ã���Ȼ��ʹ�ù����У��û�
		 *  Ҳ����ͨ�� set_sid() �޸ı������ session sid��
		 *  ���⣬����� sid Ϊ�գ�������û������ĳ�� sid ��Ӧ��
		 *  ���ݣ����û������ȵ��� set_sid()
		 */
		session(time_t ttl = 0, const char* sid = NULL);
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
		 * ���ñ� session �����Ψһ ID ��ʶ
		 * @param sid {const char*} �ǿ�
		 */
		void set_sid(const char* sid);

		/**
		 * ������ session ��� set/set_ttl ʱ��������һ������ delay Ϊ true��
		 * �����ͨ�����ñ������������������и���
		 * @return {bool} ���ݸ����Ƿ�ɹ�
		 */
		bool flush();

		/**
		 * �� session ������µ��ַ������ԣ�ͬʱ���ø�
		 * session �Ĺ���ʱ����(��)
		 * @param name {const char*} session �����ǿ�
		 * @param value {const char*} session ֵ���ǿ�
		 * @param delay {bool} ��Ϊ true ʱ�����ӳٷ��͸���ָ���˵�
		 *  ��������������û������� session::flush ���ٽ������ݸ��£���
		 *  ��������ߴ���Ч�ʣ���Ϊ false ʱ�������̸�������
		 * @return {bool} ���� false ��ʾ����
		 */
		bool set(const char* name, const char* value, bool delay = false);

		/**
		 * �� session ������µ����Զ���ͬʱ���ø�
		 * session �Ĺ���ʱ����(��)
		 * @param name {const char*} session ���������ǿ�
		 * @param value {const char*} session ����ֵ���ǿ�
		 * @param len {size_t} value ֵ����
		 * @param delay {bool} ��Ϊ true ʱ�����ӳٷ��͸���ָ���˵�
		 *  ��������������û������� session::flush ���ٽ������ݸ��£���
		 *  ��������ߴ���Ч�ʣ���Ϊ false ʱ�������̸�������
		 * @return {bool} ���� false ��ʾ����
		 */
		bool set(const char* name, const void* value, size_t len, bool delay = false);
		
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
		 * @param delay {bool} ��Ϊ true ʱ�����ӳٷ��͸���ָ���˵�
		 *  ��������������û������� session::flush ���ٽ������ݸ��£���
		 *  ��������ߴ���Ч�ʣ���Ϊ false ʱ�������̸�������
		 * @return {bool} �����Ƿ�ɹ�
		 */
		bool set_ttl(time_t ttl, bool delay = false);

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
		bool dirty_;
		std::map<string, VBUF*> attrs_;
		std::map<string, VBUF*> attrs_cache_;

		// �� session �������л�
		void serialize(string& buf);

		void serialize(const char* name, const void* value,
			size_t len, string& buf);

		// �� session ���ݷ����л�
		void deserialize(string& buf);

		void attrs_clear();
		void attrs_cache_clear();

		// �����ڴ����
		static VBUF* vbuf_new(const void* str, size_t len);

		// ���ڴ����ֵ������ڴ����ռ䲻���������·���
		// �ڴ棬�����߱����÷���ֵ��Ϊ�µ��ڴ���󣬸ö���
		// ������ԭ�е��ڴ����Ҳ�п������µ��ڴ����
		static VBUF* vbuf_set(VBUF* buf, const void* str, size_t len);

		// �ͷ��ڴ����
		static void vbuf_free(VBUF* buf);
	};

} // namespace acl
