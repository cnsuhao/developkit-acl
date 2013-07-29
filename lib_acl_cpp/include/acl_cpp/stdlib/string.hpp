#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <vector>
#include <list>
#include <utility>

struct ACL_VSTRING;

namespace acl {

/**
 * 该类为字符串处理类，支持大部分 std::string 中的功能，同时支持其不支持的一些
 * 功能；该类内部自动保证最后一个字符为 \0
 */
class ACL_CPP_API string
{
public:
	/**
	 * 构造函数
	 * @param n {size_t} 初始时分配的内存大小
	 * @param bin {bool} 是否以二进制方式构建缓冲区对象，该值为 true 时，
	 *  则当调用 += int|int64|short|char 或调用 << int|int64|short|char
	 *  时，则按二进制方式处理，否则按文本方式处理
	 */
	string(size_t n = 64, bool bin = false);

	/**
	 * 构造函数
	 * @param s {const string&} 源字符串对象，初始化后的类对象内部自动复制该字符串
	 */
	string(const string& s);

	/**
	 * 构造函数
	 * @param s {const char*} 内部自动用该字符串初始化类对象，s 必须是以 \0 结尾
	 */
	string(const char* s);

	/**
	 * 构造函数
	 * @param s {const char*} 源缓冲内容
	 * @param n {size_t} s 缓冲区数据长度
	 */
	string(const void* s, size_t n);
	~string(void);

	/**
	 * 设置字符串类对象为二进制处理模式
	 * @param bin {bool} 当该值为 true 时，则设置字符串类对象为二进制处理方式；
	 *  否则为文本方式；该值为 true 时，则当调用 += int|int64|short|char
	 *  或调用 << int|int64|short|char 时，则按二进制方式处理，否则按文本方式处理
	 */
	void set_bin(bool bin);

	/**
	 * 判断当前字符串类对象是否为二进制处理方式 
	 * @return {bool} 返回值为 true 时则表示为二进制方式
	 */
	bool get_bin() const;

	/**
	 * 根据字符数组下标获得指定位置的字符，输入参数必须为合法值，否则则内部产生断言
	 * @param n {size_t} 指定的位置（该值 >= 0 且 < 字符串长度)，如果越界，则产生断言
	 * @return {char} 返回指定位置的字符
	 */
	char operator[](size_t n);

	/**
	 * 根据字符数组下标获得指定位置的字符，输入参数必须为合法值，否则则内部产生断言
	 * @param n {int} 指定的位置（该值 >= 0 且 < 字符串长度)，如果越界，则产生断言
	 * @return {char} 返回指定位置的字符
	 */
	char operator[](int n);

	/**
	 * 对目标字符串类对象赋值
	 * @param s {const char*} 源字符串
	 * @return {string&} 返回当前字符串类对象的引用，便于对该类对象连续进行操作
	 */
	string& operator=(const char* s);

	/**
	 * 对目标字符串类对象赋值
	 * @param s {const string&} 源字符串对象
	 * @return {string&} 返回当前字符串类对象的引用，便于对该类对象连续进行操作
	 */
	string& operator=(const string&);

	/**
	 * 对目标字符串类对象赋值
	 * @param s {const string*} 源字符串对象
	 * @return {string&} 返回当前字符串类对象的引用，便于对该类对象连续进行操作
	 */
	string& operator=(const string*);

#ifdef WIN32
	/**
	 * 对目标字符串类对象赋值
	 * @param n {long long int} 源 64 位符号长整数，若字符串对象的当前状态为
	 *  二进制模式，则该函数便会以二进制方式赋值给字符串对象，否则以文本方式赋值给
	 *  字符串对象；关于二进制模式还是文本方式，其含义参见 set_bin(bool)
	 * @return {string&} 返回当前字符串类对象的引用，便于对该类对象连续进行操作
	 */
	string& operator=(__int64 n);

	/**
	 * 对目标字符串类对象赋值
	 * @param n {unsinged long long int} 源 64 位无符号长整数，若字符串对象
	 *  的当前状态为二进制模式，则该函数便会以二进制方式赋值给字符串对象，否则以文本方式
	 *  赋值给字符串对象；关于二进制模式还是文本方式，其含义参见 set_bin(bool)
	 * @return {string&} 返回当前字符串类对象的引用，便于对该类对象连续进行操作
	 */
	string& operator=(unsigned __int64);
#else
	string& operator=(long long int);
	string& operator=(unsigned long long int);
#endif

	/**
	 * 对目标字符串类对象赋值
	 * @param n {char} 源有符号字符；若字符串对象的当前状态为二进制模式，则该函数
	 *  便会以二进制方式赋值给字符串对象，否则以文本方式赋值给字符串对象；关于二进制模
	 *  式还是文本方式，其含义参见 set_bin(bool)
	 * @return {string&} 返回当前字符串类对象的引用，便于对该类对象连续进行操作
	 */
	string& operator=(char n);

	/**
	 * 对目标字符串类对象赋值
	 * @param n {char} 源无符号字符；若字符串对象的当前状态为二进制模式，则该函数
	 *  便会以二进制方式赋值给字符串对象，否则以文本方式赋值给字符串对象；关于二进制模
	 *  式还是文本方式，其含义参见 set_bin(bool)
	 * @return {string&} 返回当前字符串类对象的引用，便于对该类对象连续进行操作
	 */
	string& operator=(unsigned char n);

	/**
	 * 对目标字符串类对象赋值
	 * @param n {char} 源有符号长整型；若字符串对象的当前状态为二进制模式，则该函数
	 *  便会以二进制方式赋值给字符串对象，否则以文本方式赋值给字符串对象；关于二进制模
	 *  式还是文本方式，其含义参见 set_bin(bool)
	 * @return {string&} 返回当前字符串类对象的引用，便于对该类对象连续进行操作
	 */
	string& operator=(long n);

	/**
	 * 对目标字符串类对象赋值
	 * @param n {char} 源无符号长整型；若字符串对象的当前状态为二进制模式，则该函数
	 *  便会以二进制方式赋值给字符串对象，否则以文本方式赋值给字符串对象；关于二进制模
	 *  式还是文本方式，其含义参见 set_bin(bool)
	 * @return {string&} 返回当前字符串类对象的引用，便于对该类对象连续进行操作
	 */
	string& operator=(unsigned long n);

	/**
	 * 对目标字符串类对象赋值
	 * @param n {char} 源有符号整型；若字符串对象的当前状态为二进制模式，则该函数
	 *  便会以二进制方式赋值给字符串对象，否则以文本方式赋值给字符串对象；关于二进制模
	 *  式还是文本方式，其含义参见 set_bin(bool)
	 * @return {string&} 返回当前字符串类对象的引用，便于对该类对象连续进行操作
	 */
	string& operator=(int n);

	/**
	 * 对目标字符串类对象赋值
	 * @param n {char} 源无符号整型；若字符串对象的当前状态为二进制模式，则该函数
	 *  便会以二进制方式赋值给字符串对象，否则以文本方式赋值给字符串对象；关于二进制模
	 *  式还是文本方式，其含义参见 set_bin(bool)
	 * @return {string&} 返回当前字符串类对象的引用，便于对该类对象连续进行操作
	 */
	string& operator=(unsigned int n);

	/**
	 * 对目标字符串类对象赋值
	 * @param n {char} 源有符号短整型；若字符串对象的当前状态为二进制模式，则该函数
	 *  便会以二进制方式赋值给字符串对象，否则以文本方式赋值给字符串对象；关于二进制模
	 *  式还是文本方式，其含义参见 set_bin(bool)
	 * @return {string&} 返回当前字符串类对象的引用，便于对该类对象连续进行操作
	 */
	string& operator=(short n);

	/**
	 * 对目标字符串类对象赋值
	 * @param n {char} 源无符号短整型；若字符串对象的当前状态为二进制模式，则该函数
	 *  便会以二进制方式赋值给字符串对象，否则以文本方式赋值给字符串对象；关于二进制模
	 *  式还是文本方式，其含义参见 set_bin(bool)
	 * @return {string&} 返回当前字符串类对象的引用，便于对该类对象连续进行操作
	 */
	string& operator=(unsigned short n);

	/**
	 * 向目标字符串对象尾部添加字符串
	 * @param s {const char*} 源字符串指针
	 * @return {string&} 目标字符串对象的引用
	 */
	string& operator+=(const char* s);

	/**
	 * 向目标字符串对象尾部添加字符串
	 * @param s {const string&} 源字符串对象引用
	 * @return {string&} 目标字符串对象的引用
	 */
	string& operator+=(const string& s);

	/**
	 * 向目标字符串对象尾部添加字符串
	 * @param s {const string*} 源字符串对象指针
	 * @return {string&} 目标字符串对象的引用
	 */
	string& operator+=(const string*);
#ifdef WIN32
	/**
	 * 向目标字符串对象尾部添加有符号长整型数字，当为目标字符串对象为
	 * 二进制方式时，则按二进制数字方式添加；否则按文本方式添加
	 * @param n {long long int} 源 64 位有符号整数
	 * @return {string&} 目标字符串对象的引用
	 */
	string& operator+=(__int64 n);

	/**
	 * 向目标字符串对象尾部添加无符号长整型数字，当为目标字符串对象为
	 * 二进制方式时，则按二进制数字方式添加；否则按文本方式添加
	 * @param n {long long int} 源 64 位无符号整数
	 * @return {string&} 目标字符串对象的引用
	 */
	string& operator+=(unsigned __int64 n);
#else
	string& operator+=(long long int n);
	string& operator+=(unsigned long long int n);
#endif

	/**
	 * 向目标字符串对象尾部添加有符号长整型数字，当为目标字符串对象为
	 * 二进制方式时，则按二进制数字方式添加；否则按文本方式添加
	 * @param n {long} 源有符号长整数
	 * @return {string&} 目标字符串对象的引用
	 */
	string& operator+=(long n);

	/**
	 * 向目标字符串对象尾部添加无符号长整型数字，当为目标字符串对象为
	 * 二进制方式时，则按二进制数字方式添加；否则按文本方式添加
	 * @param n {long} 源无符号长整数
	 * @return {string&} 目标字符串对象的引用
	 */
	string& operator+=(unsigned long n);

	/**
	 * 向目标字符串对象尾部添加有符号整型数字，当为目标字符串对象为
	 * 二进制方式时，则按二进制数字方式添加；否则按文本方式添加
	 * @param n {long} 源有符号整数
	 * @return {string&} 目标字符串对象的引用
	 */
	string& operator+=(int n);

	/**
	 * 向目标字符串对象尾部添加无符号整型数字，当为目标字符串对象为
	 * 二进制方式时，则按二进制数字方式添加；否则按文本方式添加
	 * @param n {long} 源无符号整数
	 * @return {string&} 目标字符串对象的引用
	 */
	string& operator+=(unsigned int n);

	/**
	 * 向目标字符串对象尾部添加有符号短整型数字，当为目标字符串对象为
	 * 二进制方式时，则按二进制数字方式添加；否则按文本方式添加
	 * @param n {long} 源有符号短整数
	 * @return {string&} 目标字符串对象的引用
	 */
	string& operator+=(short n);

	/**
	 * 向目标字符串对象尾部添加无符号短整型数字，当为目标字符串对象为
	 * 二进制方式时，则按二进制数字方式添加；否则按文本方式添加
	 * @param n {long} 源无符号短整数
	 * @return {string&} 目标字符串对象的引用
	 */
	string& operator+=(unsigned short);

	/**
	 * 向目标字符串对象尾部添加有符号字符，当为目标字符串对象为
	 * 二进制方式时，则按二进制数字方式添加；否则按文本方式添加
	 * @param n {long} 源有符号字符
	 * @return {string&} 目标字符串对象的引用
	 */
	string& operator+=(char n);

	/**
	 * 向目标字符串对象尾部添加无符号字符，当为目标字符串对象为
	 * 二进制方式时，则按二进制数字方式添加；否则按文本方式添加
	 * @param n {long} 源无符号字符
	 * @return {string&} 目标字符串对象的引用
	 */
	string& operator+=(unsigned char n);

	/**
	 * 向目标字符串对象尾部添加字符串
	 * @param s {const string&} 源字符串对象引用
	 * @return {string&} 目标字符串对象的引用
	 */
	string& operator<<(const string& s);

	/**
	 * 向目标字符串对象尾部添加字符串
	 * @param s {const string*} 源字符串对象指针
	 * @return {string&} 目标字符串对象的引用
	 */
	string& operator<<(const string* s);

	/**
	 * 向目标字符串对象尾部添加字符串
	 * @param s {const char*} 源字符串指针
	 * @return {string&} 目标字符串对象的引用
	 */
	string& operator<<(const char* s);
#ifdef WIN32
	/**
	 * 向目标字符串对象尾部添加有符号长整型数字，当为目标字符串对象为
	 * 二进制方式时，则按二进制数字方式添加；否则按文本方式添加
	 * @param n {long long int} 源 64 位有符号整数
	 * @return {string&} 目标字符串对象的引用
	 */
	string& operator<<(__int64 n);

	/**
	 * 向目标字符串对象尾部添加无符号长整型数字，当为目标字符串对象为
	 * 二进制方式时，则按二进制数字方式添加；否则按文本方式添加
	 * @param n {long long int} 源 64 位无符号整数
	 * @return {string&} 目标字符串对象的引用
	 */
	string& operator<<(unsigned __int64 n);
#else
	string& operator<<(long long int);
	string& operator<<(unsigned long long int);
#endif

	/**
	 * 向目标字符串对象尾部添加有符号长整型数字，当为目标字符串对象为
	 * 二进制方式时，则按二进制数字方式添加；否则按文本方式添加
	 * @param n {long} 源有符号长整数
	 * @return {string&} 目标字符串对象的引用
	 */
	string& operator<<(long n);

	/**
	 * 向目标字符串对象尾部添加无符号长整型数字，当为目标字符串对象为
	 * 二进制方式时，则按二进制数字方式添加；否则按文本方式添加
	 * @param n {long} 源无符号长整数
	 * @return {string&} 目标字符串对象的引用
	 */
	string& operator<<(unsigned long n);

	/**
	 * 向目标字符串对象尾部添加有符号整型数字，当为目标字符串对象为
	 * 二进制方式时，则按二进制数字方式添加；否则按文本方式添加
	 * @param n {long} 源有符号整数
	 * @return {string&} 目标字符串对象的引用
	 */
	string& operator<<(int n);

	/**
	 * 向目标字符串对象尾部添加无符号整型数字，当为目标字符串对象为
	 * 二进制方式时，则按二进制数字方式添加；否则按文本方式添加
	 * @param n {long} 源无符号整数
	 * @return {string&} 目标字符串对象的引用
	 */
	string& operator<<(unsigned int n);

	/**
	 * 向目标字符串对象尾部添加有符号短整型数字，当为目标字符串对象为
	 * 二进制方式时，则按二进制数字方式添加；否则按文本方式添加
	 * @param n {long} 源有符号短整数
	 * @return {string&} 目标字符串对象的引用
	 */
	string& operator<<(short n);

	/**
	 * 向目标字符串对象尾部添加无符号短整型数字，当为目标字符串对象为
	 * 二进制方式时，则按二进制数字方式添加；否则按文本方式添加
	 * @param n {long} 源无符号短整数
	 * @return {string&} 目标字符串对象的引用
	 */
	string& operator<<(unsigned short n);

	/**
	 * 向目标字符串对象尾部添加有符号字符，当为目标字符串对象为
	 * 二进制方式时，则按二进制数字方式添加；否则按文本方式添加
	 * @param n {long} 源有符号字符
	 * @return {string&} 目标字符串对象的引用
	 */
	string& operator<<(char n);

	/**
	 * 向目标字符串对象尾部添加无符号字符，当为目标字符串对象为
	 * 二进制方式时，则按二进制数字方式添加；否则按文本方式添加
	 * @param n {long} 源无符号字符
	 * @return {string&} 目标字符串对象的引用
	 */
	string& operator<<(unsigned char n);

	/**
	 * 将字符串对象中的内容赋予目标字符串对象
	 * @param s {string*} 目标字符串对象
	 * @return {string&} 源字符串对象的引用
	 */
	string& operator>>(string* s);
#ifdef WIN32
	/**
	 * 将字符串对象中的内容赋予目标 64 位有符号整数
	 * @param s {string*} 目标64位有符号整数
	 * @return {string&} 源字符串对象的引用
	 */
	string& operator>>(__int64& n);

	/**
	 * 将字符串对象中的内容赋予目标 64 位无符号整数
	 * @param s {string*} 目标64位无符号整数
	 * @return {string&} 源字符串对象的引用
	 */
	string& operator>>(unsigned __int64& n);
#else
	string& operator>>(long long int&);
	string& operator>>(unsigned long long int&);
#endif

	/**
	 * 将字符串对象中的内容赋予目标 32 位有符号整数
	 * @param s {string*} 目标32位有符号整数
	 * @return {string&} 源字符串对象的引用
	 */
	string& operator>>(int& n);

	/**
	 * 将字符串对象中的内容赋予目标 32 位无符号整数
	 * @param s {string*} 目标32位无符号整数
	 * @return {string&} 源字符串对象的引用
	 */
	string& operator>>(unsigned int& n);
	string& operator>>(short&);
	string& operator>>(unsigned short&);
	string& operator>>(char&);
	string& operator>>(unsigned char&);
	bool operator==(const string&) const;
	bool operator==(const string*) const;
	bool operator==(const char*) const;
	bool operator!=(const string&) const;
	bool operator!=(const string*) const;
	bool operator!=(const char*) const;
	bool operator<(const string&) const;
	bool operator>(const string&) const;
	operator const char*() const;
	operator const void*() const;

	string& push_back(char ch);
	int compare(const string&) const;
	int compare(const string*) const;
	int compare(const char*, bool case_sensitive=true) const;
	int compare(const void* ptr, size_t len) const;
	int ncompare(const char*, size_t len, bool case_sensitive=true) const;
	int rncompare(const char*, size_t len, bool case_sensitive=true) const;

	int find(char) const;
	const char* find(const char* needle, bool case_sensitive=true) const;
	const char* rfind(const char* needle, bool case_sensitive=true) const;
	const string left(size_t npos);
	const string right(size_t npos);

	string& scan_buf(void* buf, size_t size);
	char* buf_end(void);
	void* buf() const;
	char* c_str() const;
	size_t length() const;
	size_t size() const;
	size_t capacity() const;
	bool empty() const;
	ACL_VSTRING* vstring(void) const;
	string& set_offset(size_t n);
	string& space(size_t n);

	const std::list<string>& split(const char*);
	const std::vector<string>& split2(const char*);
	const std::pair<string, string>& split_nameval(void);

	string& copy(const char* ptr);
	string& copy(const void* ptr, size_t len);
	string& memmove(const char* src);
	string& memmove(const char* src, size_t len);
	string& append(const string& s);
	string& append(const string* s);
	string& append(const char* ptr);
	string& append(const void* ptr, size_t len);
	string& prepend(const char* s);
	string& prepend(const void* ptr, size_t len);
	string& insert(size_t start, const void* ptr, size_t len);
	string& format(const char* fmt, ...);
	string& vformat(const char* fmt, va_list ap);
	string& format_append(const char* fmt, ...);
	string& vformat_append(const char* fmt, va_list ap);
	string& replace(char from, char to);
	string& truncate(size_t n);
	string& strip(const char* needle, bool each = false);
	string& clear();

	string& lower(void);
	string& upper(void);
	string& base64_encode(void);
	string& base64_encode(const void* ptr, size_t len);
	string& base64_decode(void);
	string& base64_decode(const char* s);
	string& base64_decode(const void* ptr, size_t len);
	string& url_encode(const char* s);
	string& url_decode(const char* s);
	string& hex_encode(const void* s, size_t len);
	string& hex_decode(const char* s, size_t len);

	static const string& parse_int(int);
	static const string& parse_int(unsigned int);
#ifdef WIN32
	static const string& parse_int64(__int64);
	static const string& parse_int64(unsigned __int64);
#else
	static const string& parse_int64(long long int);
	static const string& parse_int64(unsigned long long int);
#endif

private:
	bool m_bin;
	void init(size_t len);
	ACL_VSTRING* m_pVbf;
	const char* m_ptr;
	std::list<string>* m_psList;
	std::vector<string>* m_psList2;
	std::pair<string, string>* m_psPair;
};

} // namespce acl
