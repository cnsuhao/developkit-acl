#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/stdlib/string.hpp"
#include <map>

namespace acl
{

/**
 * SQL 查询语句构建类，该类会自动对 sql 中的一些特殊字符进行转义，使用方式类似于
 * java hibernate 的 SQL 语句构建方式
 */
class query
{
public:
	query();
	~query();

	/**
	 * 创建 sql 语句
	 * @param sql_fmt {const char*} sql 语句，格式如：
	 *  select * from xxx where name = :name and len >= :len
	 *  其中的 :name, :len 将由 set_parameter 中的值进行替换
	 * @return {query&}
	 */
	query& create_sql(const char* sql_fmt, ...);

	/**
	 * 设置字符串类型的变量值
	 * @param name {const char*} 变量名
	 * @param value {const char*} 变量值
	 * @return {query&}
	 */
	query& set_parameter(const char* name, const char *value);

	/**
	 * 设置字符类型的变量值
	 * @param name {const char*} 变量名
	 * @param value {char} 变量值
	 * @return {query&}
	 */
	query& set_parameter(const char* name, char value);

	/**
	 * 设置 16 位短整类型的变量值
	 * @param name {const char*} 变量名
	 * @param value {short} 变量值
	 * @return {query&}
	 */
	query& set_parameter(const char* name, short value);

	/**
	 * 设置 32 位短整类型的变量值
	 * @param name {const char*} 变量名
	 * @param value {int} 变量值
	 * @return {query&}
	 */
	query& set_parameter(const char* name, int value);

	/**
	 * 设置 64 位短整类型的变量值
	 * @param name {const char*} 变量名
	 * @param value {long long int} 变量值
	 * @return {query&}
	 */
#ifdef WIN32
	query& set_parameter(const char* name, __int64 value);
#else
	query& set_parameter(const char* name, long long int value);
#endif

	/**
	 * 对查询 sql 语句进行转义后返回给调用者
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
