#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/stdlib/string.hpp"
#include <map>

namespace acl
{

/**
 * SQL ��ѯ��乹���࣬������Զ��� sql �е�һЩ�����ַ�����ת�壬ʹ�÷�ʽ������
 * java hibernate �� SQL ��乹����ʽ
 */
class query
{
public:
	query();
	~query();

	/**
	 * ���� sql ���
	 * @param sql_fmt {const char*} sql ��䣬��ʽ�磺
	 *  select * from xxx where name = :name and len >= :len
	 *  ���е� :name, :len ���� set_parameter �е�ֵ�����滻
	 * @return {query&}
	 */
	query& create_sql(const char* sql_fmt, ...);

	/**
	 * �����ַ������͵ı���ֵ
	 * @param name {const char*} ������
	 * @param value {const char*} ����ֵ
	 * @return {query&}
	 */
	query& set_parameter(const char* name, const char *value);

	/**
	 * �����ַ����͵ı���ֵ
	 * @param name {const char*} ������
	 * @param value {char} ����ֵ
	 * @return {query&}
	 */
	query& set_parameter(const char* name, char value);

	/**
	 * ���� 16 λ�������͵ı���ֵ
	 * @param name {const char*} ������
	 * @param value {short} ����ֵ
	 * @return {query&}
	 */
	query& set_parameter(const char* name, short value);

	/**
	 * ���� 32 λ�������͵ı���ֵ
	 * @param name {const char*} ������
	 * @param value {int} ����ֵ
	 * @return {query&}
	 */
	query& set_parameter(const char* name, int value);

	/**
	 * ���� 64 λ�������͵ı���ֵ
	 * @param name {const char*} ������
	 * @param value {long long int} ����ֵ
	 * @return {query&}
	 */
#ifdef WIN32
	query& set_parameter(const char* name, __int64 value);
#else
	query& set_parameter(const char* name, long long int value);
#endif

	/**
	 * �Բ�ѯ sql ������ת��󷵻ظ�������
	 * @return {const string&}
	 */
	const string& to_string();

private:
	typedef enum
	{
		DB_PARAM_CHAR,
		DB_PARAM_SHORT,
		DB_PARAM_INT32,
		DB_PARAM_INT64,
		DB_PARAM_STR
	} db_param_type;

	struct query_param
	{
		char type;
		int  dlen;
		union
		{
			char  c;
			short s;
			int   n;
			long long int l;
			char  S[1];
		} v;
	};

	std::map<string, query_param*> params_;
	string* sql_buf_;
	string sql_;
	string buf_;

	void del_param(const string& key);
	bool append_key(string& buf, char* key);

	string& escape(const char* in, size_t len);
};

} // namespace acl
