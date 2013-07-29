#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <vector>
#include <list>
#include <utility>

struct ACL_VSTRING;

namespace acl {

/**
 * ����Ϊ�ַ��������֧࣬�ִ󲿷� std::string �еĹ��ܣ�ͬʱ֧���䲻֧�ֵ�һЩ
 * ���ܣ������ڲ��Զ���֤���һ���ַ�Ϊ \0
 */
class ACL_CPP_API string
{
public:
	/**
	 * ���캯��
	 * @param n {size_t} ��ʼʱ������ڴ��С
	 * @param bin {bool} �Ƿ��Զ����Ʒ�ʽ�������������󣬸�ֵΪ true ʱ��
	 *  �򵱵��� += int|int64|short|char ����� << int|int64|short|char
	 *  ʱ���򰴶����Ʒ�ʽ���������ı���ʽ����
	 */
	string(size_t n = 64, bool bin = false);

	/**
	 * ���캯��
	 * @param s {const string&} Դ�ַ������󣬳�ʼ�����������ڲ��Զ����Ƹ��ַ���
	 */
	string(const string& s);

	/**
	 * ���캯��
	 * @param s {const char*} �ڲ��Զ��ø��ַ�����ʼ�������s �������� \0 ��β
	 */
	string(const char* s);

	/**
	 * ���캯��
	 * @param s {const char*} Դ��������
	 * @param n {size_t} s ���������ݳ���
	 */
	string(const void* s, size_t n);
	~string(void);

	/**
	 * �����ַ��������Ϊ�����ƴ���ģʽ
	 * @param bin {bool} ����ֵΪ true ʱ���������ַ��������Ϊ�����ƴ���ʽ��
	 *  ����Ϊ�ı���ʽ����ֵΪ true ʱ���򵱵��� += int|int64|short|char
	 *  ����� << int|int64|short|char ʱ���򰴶����Ʒ�ʽ���������ı���ʽ����
	 */
	void set_bin(bool bin);

	/**
	 * �жϵ�ǰ�ַ���������Ƿ�Ϊ�����ƴ���ʽ 
	 * @return {bool} ����ֵΪ true ʱ���ʾΪ�����Ʒ�ʽ
	 */
	bool get_bin() const;

	/**
	 * �����ַ������±���ָ��λ�õ��ַ��������������Ϊ�Ϸ�ֵ���������ڲ���������
	 * @param n {size_t} ָ����λ�ã���ֵ >= 0 �� < �ַ�������)�����Խ�磬���������
	 * @return {char} ����ָ��λ�õ��ַ�
	 */
	char operator[](size_t n);

	/**
	 * �����ַ������±���ָ��λ�õ��ַ��������������Ϊ�Ϸ�ֵ���������ڲ���������
	 * @param n {int} ָ����λ�ã���ֵ >= 0 �� < �ַ�������)�����Խ�磬���������
	 * @return {char} ����ָ��λ�õ��ַ�
	 */
	char operator[](int n);

	/**
	 * ��Ŀ���ַ��������ֵ
	 * @param s {const char*} Դ�ַ���
	 * @return {string&} ���ص�ǰ�ַ������������ã����ڶԸ�������������в���
	 */
	string& operator=(const char* s);

	/**
	 * ��Ŀ���ַ��������ֵ
	 * @param s {const string&} Դ�ַ�������
	 * @return {string&} ���ص�ǰ�ַ������������ã����ڶԸ�������������в���
	 */
	string& operator=(const string&);

	/**
	 * ��Ŀ���ַ��������ֵ
	 * @param s {const string*} Դ�ַ�������
	 * @return {string&} ���ص�ǰ�ַ������������ã����ڶԸ�������������в���
	 */
	string& operator=(const string*);

#ifdef WIN32
	/**
	 * ��Ŀ���ַ��������ֵ
	 * @param n {long long int} Դ 64 λ���ų����������ַ�������ĵ�ǰ״̬Ϊ
	 *  ������ģʽ����ú�������Զ����Ʒ�ʽ��ֵ���ַ������󣬷������ı���ʽ��ֵ��
	 *  �ַ������󣻹��ڶ�����ģʽ�����ı���ʽ���京��μ� set_bin(bool)
	 * @return {string&} ���ص�ǰ�ַ������������ã����ڶԸ�������������в���
	 */
	string& operator=(__int64 n);

	/**
	 * ��Ŀ���ַ��������ֵ
	 * @param n {unsinged long long int} Դ 64 λ�޷��ų����������ַ�������
	 *  �ĵ�ǰ״̬Ϊ������ģʽ����ú�������Զ����Ʒ�ʽ��ֵ���ַ������󣬷������ı���ʽ
	 *  ��ֵ���ַ������󣻹��ڶ�����ģʽ�����ı���ʽ���京��μ� set_bin(bool)
	 * @return {string&} ���ص�ǰ�ַ������������ã����ڶԸ�������������в���
	 */
	string& operator=(unsigned __int64);
#else
	string& operator=(long long int);
	string& operator=(unsigned long long int);
#endif

	/**
	 * ��Ŀ���ַ��������ֵ
	 * @param n {char} Դ�з����ַ������ַ�������ĵ�ǰ״̬Ϊ������ģʽ����ú���
	 *  ����Զ����Ʒ�ʽ��ֵ���ַ������󣬷������ı���ʽ��ֵ���ַ������󣻹��ڶ�����ģ
	 *  ʽ�����ı���ʽ���京��μ� set_bin(bool)
	 * @return {string&} ���ص�ǰ�ַ������������ã����ڶԸ�������������в���
	 */
	string& operator=(char n);

	/**
	 * ��Ŀ���ַ��������ֵ
	 * @param n {char} Դ�޷����ַ������ַ�������ĵ�ǰ״̬Ϊ������ģʽ����ú���
	 *  ����Զ����Ʒ�ʽ��ֵ���ַ������󣬷������ı���ʽ��ֵ���ַ������󣻹��ڶ�����ģ
	 *  ʽ�����ı���ʽ���京��μ� set_bin(bool)
	 * @return {string&} ���ص�ǰ�ַ������������ã����ڶԸ�������������в���
	 */
	string& operator=(unsigned char n);

	/**
	 * ��Ŀ���ַ��������ֵ
	 * @param n {char} Դ�з��ų����ͣ����ַ�������ĵ�ǰ״̬Ϊ������ģʽ����ú���
	 *  ����Զ����Ʒ�ʽ��ֵ���ַ������󣬷������ı���ʽ��ֵ���ַ������󣻹��ڶ�����ģ
	 *  ʽ�����ı���ʽ���京��μ� set_bin(bool)
	 * @return {string&} ���ص�ǰ�ַ������������ã����ڶԸ�������������в���
	 */
	string& operator=(long n);

	/**
	 * ��Ŀ���ַ��������ֵ
	 * @param n {char} Դ�޷��ų����ͣ����ַ�������ĵ�ǰ״̬Ϊ������ģʽ����ú���
	 *  ����Զ����Ʒ�ʽ��ֵ���ַ������󣬷������ı���ʽ��ֵ���ַ������󣻹��ڶ�����ģ
	 *  ʽ�����ı���ʽ���京��μ� set_bin(bool)
	 * @return {string&} ���ص�ǰ�ַ������������ã����ڶԸ�������������в���
	 */
	string& operator=(unsigned long n);

	/**
	 * ��Ŀ���ַ��������ֵ
	 * @param n {char} Դ�з������ͣ����ַ�������ĵ�ǰ״̬Ϊ������ģʽ����ú���
	 *  ����Զ����Ʒ�ʽ��ֵ���ַ������󣬷������ı���ʽ��ֵ���ַ������󣻹��ڶ�����ģ
	 *  ʽ�����ı���ʽ���京��μ� set_bin(bool)
	 * @return {string&} ���ص�ǰ�ַ������������ã����ڶԸ�������������в���
	 */
	string& operator=(int n);

	/**
	 * ��Ŀ���ַ��������ֵ
	 * @param n {char} Դ�޷������ͣ����ַ�������ĵ�ǰ״̬Ϊ������ģʽ����ú���
	 *  ����Զ����Ʒ�ʽ��ֵ���ַ������󣬷������ı���ʽ��ֵ���ַ������󣻹��ڶ�����ģ
	 *  ʽ�����ı���ʽ���京��μ� set_bin(bool)
	 * @return {string&} ���ص�ǰ�ַ������������ã����ڶԸ�������������в���
	 */
	string& operator=(unsigned int n);

	/**
	 * ��Ŀ���ַ��������ֵ
	 * @param n {char} Դ�з��Ŷ����ͣ����ַ�������ĵ�ǰ״̬Ϊ������ģʽ����ú���
	 *  ����Զ����Ʒ�ʽ��ֵ���ַ������󣬷������ı���ʽ��ֵ���ַ������󣻹��ڶ�����ģ
	 *  ʽ�����ı���ʽ���京��μ� set_bin(bool)
	 * @return {string&} ���ص�ǰ�ַ������������ã����ڶԸ�������������в���
	 */
	string& operator=(short n);

	/**
	 * ��Ŀ���ַ��������ֵ
	 * @param n {char} Դ�޷��Ŷ����ͣ����ַ�������ĵ�ǰ״̬Ϊ������ģʽ����ú���
	 *  ����Զ����Ʒ�ʽ��ֵ���ַ������󣬷������ı���ʽ��ֵ���ַ������󣻹��ڶ�����ģ
	 *  ʽ�����ı���ʽ���京��μ� set_bin(bool)
	 * @return {string&} ���ص�ǰ�ַ������������ã����ڶԸ�������������в���
	 */
	string& operator=(unsigned short n);

	/**
	 * ��Ŀ���ַ�������β������ַ���
	 * @param s {const char*} Դ�ַ���ָ��
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator+=(const char* s);

	/**
	 * ��Ŀ���ַ�������β������ַ���
	 * @param s {const string&} Դ�ַ�����������
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator+=(const string& s);

	/**
	 * ��Ŀ���ַ�������β������ַ���
	 * @param s {const string*} Դ�ַ�������ָ��
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator+=(const string*);
#ifdef WIN32
	/**
	 * ��Ŀ���ַ�������β������з��ų��������֣���ΪĿ���ַ�������Ϊ
	 * �����Ʒ�ʽʱ���򰴶��������ַ�ʽ��ӣ������ı���ʽ���
	 * @param n {long long int} Դ 64 λ�з�������
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator+=(__int64 n);

	/**
	 * ��Ŀ���ַ�������β������޷��ų��������֣���ΪĿ���ַ�������Ϊ
	 * �����Ʒ�ʽʱ���򰴶��������ַ�ʽ��ӣ������ı���ʽ���
	 * @param n {long long int} Դ 64 λ�޷�������
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator+=(unsigned __int64 n);
#else
	string& operator+=(long long int n);
	string& operator+=(unsigned long long int n);
#endif

	/**
	 * ��Ŀ���ַ�������β������з��ų��������֣���ΪĿ���ַ�������Ϊ
	 * �����Ʒ�ʽʱ���򰴶��������ַ�ʽ��ӣ������ı���ʽ���
	 * @param n {long} Դ�з��ų�����
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator+=(long n);

	/**
	 * ��Ŀ���ַ�������β������޷��ų��������֣���ΪĿ���ַ�������Ϊ
	 * �����Ʒ�ʽʱ���򰴶��������ַ�ʽ��ӣ������ı���ʽ���
	 * @param n {long} Դ�޷��ų�����
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator+=(unsigned long n);

	/**
	 * ��Ŀ���ַ�������β������з����������֣���ΪĿ���ַ�������Ϊ
	 * �����Ʒ�ʽʱ���򰴶��������ַ�ʽ��ӣ������ı���ʽ���
	 * @param n {long} Դ�з�������
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator+=(int n);

	/**
	 * ��Ŀ���ַ�������β������޷����������֣���ΪĿ���ַ�������Ϊ
	 * �����Ʒ�ʽʱ���򰴶��������ַ�ʽ��ӣ������ı���ʽ���
	 * @param n {long} Դ�޷�������
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator+=(unsigned int n);

	/**
	 * ��Ŀ���ַ�������β������з��Ŷ��������֣���ΪĿ���ַ�������Ϊ
	 * �����Ʒ�ʽʱ���򰴶��������ַ�ʽ��ӣ������ı���ʽ���
	 * @param n {long} Դ�з��Ŷ�����
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator+=(short n);

	/**
	 * ��Ŀ���ַ�������β������޷��Ŷ��������֣���ΪĿ���ַ�������Ϊ
	 * �����Ʒ�ʽʱ���򰴶��������ַ�ʽ��ӣ������ı���ʽ���
	 * @param n {long} Դ�޷��Ŷ�����
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator+=(unsigned short);

	/**
	 * ��Ŀ���ַ�������β������з����ַ�����ΪĿ���ַ�������Ϊ
	 * �����Ʒ�ʽʱ���򰴶��������ַ�ʽ��ӣ������ı���ʽ���
	 * @param n {long} Դ�з����ַ�
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator+=(char n);

	/**
	 * ��Ŀ���ַ�������β������޷����ַ�����ΪĿ���ַ�������Ϊ
	 * �����Ʒ�ʽʱ���򰴶��������ַ�ʽ��ӣ������ı���ʽ���
	 * @param n {long} Դ�޷����ַ�
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator+=(unsigned char n);

	/**
	 * ��Ŀ���ַ�������β������ַ���
	 * @param s {const string&} Դ�ַ�����������
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator<<(const string& s);

	/**
	 * ��Ŀ���ַ�������β������ַ���
	 * @param s {const string*} Դ�ַ�������ָ��
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator<<(const string* s);

	/**
	 * ��Ŀ���ַ�������β������ַ���
	 * @param s {const char*} Դ�ַ���ָ��
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator<<(const char* s);
#ifdef WIN32
	/**
	 * ��Ŀ���ַ�������β������з��ų��������֣���ΪĿ���ַ�������Ϊ
	 * �����Ʒ�ʽʱ���򰴶��������ַ�ʽ��ӣ������ı���ʽ���
	 * @param n {long long int} Դ 64 λ�з�������
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator<<(__int64 n);

	/**
	 * ��Ŀ���ַ�������β������޷��ų��������֣���ΪĿ���ַ�������Ϊ
	 * �����Ʒ�ʽʱ���򰴶��������ַ�ʽ��ӣ������ı���ʽ���
	 * @param n {long long int} Դ 64 λ�޷�������
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator<<(unsigned __int64 n);
#else
	string& operator<<(long long int);
	string& operator<<(unsigned long long int);
#endif

	/**
	 * ��Ŀ���ַ�������β������з��ų��������֣���ΪĿ���ַ�������Ϊ
	 * �����Ʒ�ʽʱ���򰴶��������ַ�ʽ��ӣ������ı���ʽ���
	 * @param n {long} Դ�з��ų�����
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator<<(long n);

	/**
	 * ��Ŀ���ַ�������β������޷��ų��������֣���ΪĿ���ַ�������Ϊ
	 * �����Ʒ�ʽʱ���򰴶��������ַ�ʽ��ӣ������ı���ʽ���
	 * @param n {long} Դ�޷��ų�����
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator<<(unsigned long n);

	/**
	 * ��Ŀ���ַ�������β������з����������֣���ΪĿ���ַ�������Ϊ
	 * �����Ʒ�ʽʱ���򰴶��������ַ�ʽ��ӣ������ı���ʽ���
	 * @param n {long} Դ�з�������
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator<<(int n);

	/**
	 * ��Ŀ���ַ�������β������޷����������֣���ΪĿ���ַ�������Ϊ
	 * �����Ʒ�ʽʱ���򰴶��������ַ�ʽ��ӣ������ı���ʽ���
	 * @param n {long} Դ�޷�������
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator<<(unsigned int n);

	/**
	 * ��Ŀ���ַ�������β������з��Ŷ��������֣���ΪĿ���ַ�������Ϊ
	 * �����Ʒ�ʽʱ���򰴶��������ַ�ʽ��ӣ������ı���ʽ���
	 * @param n {long} Դ�з��Ŷ�����
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator<<(short n);

	/**
	 * ��Ŀ���ַ�������β������޷��Ŷ��������֣���ΪĿ���ַ�������Ϊ
	 * �����Ʒ�ʽʱ���򰴶��������ַ�ʽ��ӣ������ı���ʽ���
	 * @param n {long} Դ�޷��Ŷ�����
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator<<(unsigned short n);

	/**
	 * ��Ŀ���ַ�������β������з����ַ�����ΪĿ���ַ�������Ϊ
	 * �����Ʒ�ʽʱ���򰴶��������ַ�ʽ��ӣ������ı���ʽ���
	 * @param n {long} Դ�з����ַ�
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator<<(char n);

	/**
	 * ��Ŀ���ַ�������β������޷����ַ�����ΪĿ���ַ�������Ϊ
	 * �����Ʒ�ʽʱ���򰴶��������ַ�ʽ��ӣ������ı���ʽ���
	 * @param n {long} Դ�޷����ַ�
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator<<(unsigned char n);

	/**
	 * ���ַ��������е����ݸ���Ŀ���ַ�������
	 * @param s {string*} Ŀ���ַ�������
	 * @return {string&} Դ�ַ������������
	 */
	string& operator>>(string* s);
#ifdef WIN32
	/**
	 * ���ַ��������е����ݸ���Ŀ�� 64 λ�з�������
	 * @param s {string*} Ŀ��64λ�з�������
	 * @return {string&} Դ�ַ������������
	 */
	string& operator>>(__int64& n);

	/**
	 * ���ַ��������е����ݸ���Ŀ�� 64 λ�޷�������
	 * @param s {string*} Ŀ��64λ�޷�������
	 * @return {string&} Դ�ַ������������
	 */
	string& operator>>(unsigned __int64& n);
#else
	string& operator>>(long long int&);
	string& operator>>(unsigned long long int&);
#endif

	/**
	 * ���ַ��������е����ݸ���Ŀ�� 32 λ�з�������
	 * @param s {string*} Ŀ��32λ�з�������
	 * @return {string&} Դ�ַ������������
	 */
	string& operator>>(int& n);

	/**
	 * ���ַ��������е����ݸ���Ŀ�� 32 λ�޷�������
	 * @param s {string*} Ŀ��32λ�޷�������
	 * @return {string&} Դ�ַ������������
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
