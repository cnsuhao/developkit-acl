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
	string& operator=(const string& s);

	/**
	 * ��Ŀ���ַ��������ֵ
	 * @param s {const string*} Դ�ַ�������
	 * @return {string&} ���ص�ǰ�ַ������������ã����ڶԸ�������������в���
	 */
	string& operator=(const string* s);

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
	string& operator+=(const string* s);
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
	string& operator+=(unsigned short n);

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
	string& operator<<(long long int n);
	string& operator<<(unsigned long long int n);
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
	 * @param n {string*} Ŀ�� 64 λ�з�������
	 * @return {string&} Դ�ַ������������
	 */
	string& operator>>(__int64& n);

	/**
	 * ���ַ��������е����ݸ���Ŀ�� 64 λ�޷�������
	 * @param n {string*} Ŀ�� 64 λ�޷�������
	 * @return {string&} Դ�ַ������������
	 */
	string& operator>>(unsigned __int64& n);
#else
	string& operator>>(long long int&);
	string& operator>>(unsigned long long int&);
#endif

	/**
	 * ���ַ��������е����ݸ���Ŀ�� 32 λ�з�������
	 * @param n {string*} Ŀ�� 32 λ�з�������
	 * @return {string&} Դ�ַ������������
	 */
	string& operator>>(int& n);

	/**
	 * ���ַ��������е����ݸ���Ŀ�� 32 λ�޷�������
	 * @param n {string*} Ŀ�� 32 λ�޷�������
	 * @return {string&} Դ�ַ������������
	 */
	string& operator>>(unsigned int& n);

	/**
	 * ���ַ��������е����ݸ���Ŀ�� 16 λ�з�������
	 * @param n {string*} Ŀ�� 16 λ�з�������
	 * @return {string&} Դ�ַ������������
	 */
	string& operator>>(short& n);

	/**
	 * ���ַ��������е����ݸ���Ŀ�� 16 λ�޷�������
	 * @param n {string*} Ŀ�� 16 λ�޷�������
	 * @return {string&} Դ�ַ������������
	 */
	string& operator>>(unsigned short& n);

	/**
	 * ���ַ��������е����ݸ���Ŀ�� 8 λ�з����ַ�
	 * @param n {string*} Ŀ�� 16 λ�з����ַ�
	 * @return {string&} Դ�ַ������������
	 */
	string& operator>>(char& n);

	/**
	 * ���ַ��������е����ݸ���Ŀ�� 8 λ�޷����ַ�
	 * @param n {string*} Ŀ�� 16 λ�޷����ַ�
	 * @return {string&} Դ�ַ������������
	 */
	string& operator>>(unsigned char& n);

	/**
	 * �жϵ�ǰ�ַ���������������������ַ������������Ƿ���ȣ��ڲ����ִ�Сд��
	 * @param s {const string&} ������ַ�����������
	 * @return {bool} ���� true ��ʾ�ַ���������ͬ
	 */
	bool operator==(const string& s) const;

	/**
	 * �жϵ�ǰ�ַ���������������������ַ������������Ƿ���ȣ��ڲ����ִ�Сд��
	 * @param s {const string&} ������ַ�������ָ��
	 * @return {bool} ���� true ��ʾ�ַ���������ͬ
	 */
	bool operator==(const string* s) const;

	/**
	 * �жϵ�ǰ�ַ���������������������ַ��������Ƿ���ȣ��ڲ����ִ�Сд��
	 * @param s {const string&} ������ַ�������ָ��
	 * @return {bool} ���� true ��ʾ�ַ���������ͬ
	 */
	bool operator==(const char* s) const;

	/**
	 * �жϵ�ǰ�ַ���������������������ַ������������Ƿ񲻵ȣ��ڲ����ִ�Сд��
	 * @param s {const string&} ������ַ�����������
	 * @return {bool} ���� true ��ʾ�ַ������ݲ�ͬ
	 */
	bool operator!=(const string& s) const;

	/**
	 * �жϵ�ǰ�ַ���������������������ַ������������Ƿ񲻵ȣ��ڲ����ִ�Сд��
	 * @param s {const string&} ������ַ�������ָ��
	 * @return {bool} ���� true ��ʾ�ַ������ݲ�ͬ
	 */
	bool operator!=(const string* s) const;

	/**
	 * �жϵ�ǰ�ַ���������������������ַ��������Ƿ񲻵ȣ��ڲ����ִ�Сд��
	 * @param s {const string&} ������ַ�������ָ��
	 * @return {bool} ���� true ��ʾ�ַ������ݲ�ͬ
	 */
	bool operator!=(const char* s) const;

	/**
	 * �жϵ�ǰ�ַ�������������Ƿ�С���������ַ����������ݣ��ڲ����ִ�Сд��
	 * @param s {const string&} ������ַ�����������
	 * @return {bool} ���� true ��ʾ��ǰ�ַ������������С��������ַ���
	 *  ��������
	 */
	bool operator<(const string& s) const;

	/**
	 * �жϵ�ǰ�ַ�������������Ƿ�����������ַ����������ݣ��ڲ����ִ�Сд��
	 * @param s {const string&} ������ַ�����������
	 * @return {bool} ���� true ��ʾ��ǰ�ַ�����������ݴ���������ַ���
	 *  ��������
	 */
	bool operator>(const string& s) const;

	/**
	 * ����ǰ�ַ�������ֱ��תΪ�ַ���ָ�루�����ڲ�������ֱ�ӵ�����
	 * @return {const char*} ����ֵ��ԶΪ�ǿ�ָ�룬�п���Ϊ�մ�
	 */
	operator const char*() const;

	/**
	 * ����ǰ�ַ�������ֱ��תΪͨ��ָ�루�����ڲ�������ֱ�ӵ�����
	 * @return {const char*} ����ֵ��ԶΪ�ǿ�ָ��
	 */
	operator const void*() const;

	/**
	 * ��һ���з����ַ���ӽ���ǰ�ַ��������β��
	 * @param ch {char} �з����ַ�
	 * @return {string&} ��ǰ�ַ������������
	 */
	string& push_back(char ch);

	/**
	 * �Ƚ������ַ�������������Ƿ���ͬ�����ִ�Сд��
	 * @param s {const string&} ������ַ������������
	 * @return {int} 0����ʾ������ͬ�� > 0����ǰ�ַ������ݴ�����������ݣ�
	 *  < 0 ����ǰ�ַ�������С�����������
	 */
	int compare(const string& s) const;

	/**
	 * �Ƚ������ַ�������������Ƿ���ͬ�����ִ�Сд��
	 * @param s {const string&} ������ַ��������ָ��
	 * @return {int} 0����ʾ������ͬ�� > 0����ǰ�ַ������ݴ�����������ݣ�
	 *  < 0 ����ǰ�ַ�������С�����������
	 */
	int compare(const string* s) const;

	/**
	 * �Ƚ������ַ����������Ƿ���ͬ
	 * @param s {const string&} ������ַ������������
	 * @param case_sensitive {bool} Ϊ true ��ʾ���ִ�Сд
	 * @return {int} 0����ʾ������ͬ�� > 0����ǰ�ַ������ݴ�����������ݣ�
	 *  < 0 ����ǰ�ַ�������С�����������
	 */
	int compare(const char* s, bool case_sensitive = true) const;

	/**
	 * �Ƚϵ�ǰ����Ļ����������Ƿ��������Ļ�������������ͬ
	 * @param ptr {const void*} ����Ļ�������ַ
	 * @param len {size_t} ptr �Ļ����������ݳ���
	 * @return {int} 0����ʾ������ͬ�� > 0����ǰ���󻺳������ݴ�����������ݣ�
	 *  < 0 ����ǰ���󻺳�����С�����������
	 */
	int compare(const void* ptr, size_t len) const;

	/**
	 * �Ƚϵ�ǰ����Ļ����������Ƿ��������Ļ�������������ͬ���޶��Ƚ����ݳ���
	 * @param s {const void*} ����Ļ�������ַ
	 * @param len {size_t} ptr �Ļ����������ݳ���
	 * @param case_sensitive {bool} Ϊ true ��ʾ���ִ�Сд
	 * @return {int} 0����ʾ������ͬ�� > 0����ǰ���󻺳������ݴ�����������ݣ�
	 *  < 0 ����ǰ���󻺳�����С�����������
	 */
	int ncompare(const char* s, size_t len, bool case_sensitive = true) const;

	/**
	 * ��β����ǰ�Ƚϵ�ǰ����Ļ����������Ƿ��������Ļ�������������ͬ���޶��Ƚ����ݳ���
	 * @param s {const void*} ����Ļ�������ַ
	 * @param len {size_t} ptr �Ļ����������ݳ���
	 * @param case_sensitive {bool} Ϊ true ��ʾ���ִ�Сд
	 * @return {int} 0����ʾ������ͬ�� > 0����ǰ���󻺳������ݴ�����������ݣ�
	 *  < 0 ����ǰ���󻺳�����С�����������
	 */
	int rncompare(const char* s, size_t len, bool case_sensitive = true) const;

	/**
	 * ����ָ���ַ��ڵ�ǰ���󻺳�����λ�ã��±�� 0 ��ʼ��
	 * @param n {char} Ҫ���ҵ��з����ַ�
	 * @return {int} �ַ��ڻ������е�λ�ã�������ֵ < 0 ���ʾ������
	 */
	int find(char n) const;

	/**
	 * ����ָ���ַ����ڵ�ǰ���󻺳�������ʼλ�ã��±�� 0 ��ʼ��
	 * @param needle {const char*} Ҫ���ҵ��з����ַ���
	 * @param case_sensitive {bool} Ϊ true ��ʾ���ִ�Сд
	 * @return {const char*} �ַ����ڻ������е���ʼλ�ã�������ֵΪ��ָ�����ʾ������
	 */
	const char* find(const char* needle, bool case_sensitive=true) const;

	/**
	 * ��β����ǰ����ָ���ַ����ڵ�ǰ���󻺳�������ʼλ�ã��±�� 0 ��ʼ��
	 * @param needle {const char*} Ҫ���ҵ��з����ַ���
	 * @param case_sensitive {bool} Ϊ true ��ʾ���ִ�Сд
	 * @return {const char*} �ַ����ڻ������е���ʼλ�ã�������ֵΪ��ָ�����ʾ������
	 */
	const char* rfind(const char* needle, bool case_sensitive=true) const;

	/**
	 * ���شӵ�ǰ�ַ��������л�����ָ��λ�����������
	 * @param npos {size_t} �±�λ�ã�����ֵ���ڵ��ڵ�ǰ�ַ��������ݳ���ʱ��
	 *  �򷵻������ַ������󣻷���ֵ��������ֵָ��λ�õ��ַ�����
	 * @return {const string} ����ֵΪһ�����Ķ��󣬲���Ҫ�����ͷţ��ú�����Ч��
	 *  ���ܲ���̫��
	 */
	const string left(size_t npos);

	/**
	 * ���شӵ�ǰ�ַ��������л�����ָ��λ�����ҵ�����
	 * @param npos {size_t} �±�λ�ã�����ֵ���ڵ��ڵ�ǰ�ַ��������ݳ���ʱ��
	 *  �򷵻ص��ַ�����������Ϊ�գ�����ֵ��������ֵָ��λ�õ��ַ�����
	 * @return {const string} ����ֵΪһ�����Ķ��󣬲���Ҫ�����ͷţ��ú�����Ч��
	 *  ���ܲ���̫��
	 */
	const string right(size_t npos);

	/**
	 * ����ǰ����Ļ������ݿ���һ����������Ŀ�껺����
	 * @param buf {void*} Ŀ�껺������ַ
	 * @param size {size_t} buf ����������
	 * @return {string&} ��ǰ���������
	 */
	string& scan_buf(void* buf, size_t size);

	/**
	 * ���ص�ǰ���󻺳����е�һ���������ݵ�β����ַ
	 * @return {char*} ����ֵΪ NULL ��������ڲ�������
	 */
	char* buf_end(void);

	/**
	 * ���ص�ǰ���󻺳�������ʼ��ַ
	 * @return {void*} ���ص�ַ��Զ�ǿ�
	 */
	void* buf() const;

	/**
	 * ���ַ�����ʽ���ص�ǰ���󻺳�������ʼ��ַ
	 * @return {char*} ���ص�ַ��Զ�ǿ�
	 */
	char* c_str() const;

	/**
	 * ���ص�ǰ�����ַ����ĳ��ȣ�����\0��
	 * @return {size_t} ����ֵ >= 0
	 */
	size_t length() const;

	/**
	 * ���ص�ǰ�����ַ����ĳ��ȣ�����\0���������� length ��ͬ
	 * @return {size_t} ����ֵ >= 0
	 */
	size_t size() const;

	/**
	 * ���ص�ǰ����Ļ������Ŀռ䳤�ȣ���ֵ >= �����������ݳ���
	 * @return {size_t} ����ֵ > 0
	 */
	size_t capacity() const;

	/**
	 * �жϵ�ǰ����Ļ����������ݳ����Ƿ�Ϊ 0
	 * @return {bool} ���� true ��ʾ����Ϊ��
	 */
	bool empty() const;

	/**
	 * ���ص�ǰ�����ڲ����õ� acl C ���е� ACL_VSTRING �����ַ
	 * @return {ACL_VSTRING*} ����ֵ��Զ�ǿ�
	 */
	ACL_VSTRING* vstring(void) const;

	/**
	 * ����ǰ����Ļ��������±�λ������ָ��λ��
	 * @param n {size_t} Ŀ���±�λ�ã�����ֵ >= capacity ʱ���ڲ���
	 *  ���·������Щ���ڴ�
	 * @return {string&} ��ǰ���������
	 */
	string& set_offset(size_t n);

	/**
	 * ���øú�������Ԥ�ȱ�֤����Ҫ�Ļ�������С
	 * @param n {size_t} ϣ���Ļ������ռ��Сֵ
	 * @return {string&} ��ǰ���������
	 */
	string& space(size_t n);

	/**
	 * ����ǰ����洢���ַ������зָ�
	 * @param sep {const char*} ���зָ�ʱ�ķָ���
	 * @return {const std::list<string>&} ���� list ��ʽ�ķָ���
	 */
	const std::list<string>& split(const char* sep);

	/**
	 * ����ǰ����洢���ַ������зָ�
	 * @param sep {const char*} ���зָ�ʱ�ķָ���
	 * @return {const std::vector<string>&} ���� vector ��ʽ�ķָ���
	 */
	const std::vector<string>& split2(const char* sep);

	/**
	 * �� '=' Ϊ�ָ�������ǰ����洢���ַ����ָ�� name/value �ԣ��ָ�ʱ���Զ�
	 * ȥ��Դ�ַ�������ʼ������β���Լ��ָ��� '=' ���ߵĿո� TAB
	 * @return {const std::pair<string, string>&} �����ǰ����洢���ַ���
	 *  �����Ϸָ��������������ϸ�� name=value��ʽ�����򷵻صĽ�����ַ�������Ϊ�մ�
	 */
	const std::pair<string, string>& split_nameval(void);

	/**
	 * ���ַ�����������ǰ����Ļ�������
	 * @param ptr {const char*} Դ�ַ�����ַ������ '\0' ����
	 * @return {string&} ��ǰ���������
	 */
	string& copy(const char* ptr);

	/**
	 * ��Դ���ݵĶ������ݿ�������ǰ����Ļ�������
	 * @param ptr {const void*} Դ���ݵ�ַ
	 * @param len {size_t} ptr Դ���ݳ���
	 * @return {string&} ��ǰ���������
	 */
	string& copy(const void* ptr, size_t len);

	/**
	 * ��Դ�ַ����������ƶ�����ǰ����Ļ������У��ڲ����Զ��ж�Դ����
	 * ��ַ�Ƿ���ڵ�ǰ����Ļ�������
	 * @param src {const char*} Դ���ݵ�ַ
	 * @return {string&} ��ǰ���������
	 */
	string& memmove(const char* src);

	/**
	 * ��Դ�ַ����������ƶ�����ǰ����Ļ������У��ڲ����Զ��ж�Դ����
	 * ��ַ�Ƿ���ڵ�ǰ����Ļ�������
	 * @param src {const char*} Դ���ݵ�ַ
	 * @param len {size_t} �ƶ����ݵĳ���
	 * @return {string&} ��ǰ���������
	 */
	string& memmove(const char* src, size_t len);

	/**
	 * ��ָ���ַ�������ڵ�ǰ�������ݻ��������ݵ�β��
	 * @param s {const string&} Դ���ݶ�������
	 * @return {string&} ��ǰ���������
	 */
	string& append(const string& s);

	/**
	 * ��ָ���ַ�������ڵ�ǰ�������ݻ��������ݵ�β��
	 * @param s {const string&} Դ���ݶ���ָ��
	 * @return {string&} ��ǰ���������
	 */
	string& append(const string* s);

	/**
	 * ��ָ���ַ�������ڵ�ǰ�������ݻ��������ݵ�β��
	 * @param s {const string&} Դ���ݶ���ָ��
	 * @return {string&} ��ǰ���������
	 */
	string& append(const char* s);

	/**
	 * ��ָ���������е���������ڵ�ǰ�������ݻ��������ݵ�β��
	 * @param ptr {const void*} Դ���ݶ���ָ��
	 * @param len {size_t} ptr ���ݳ���
	 * @return {string&} ��ǰ���������
	 */
	string& append(const void* ptr, size_t len);

	/**
	 * ��ָ���ַ�����������ڵ�ǰ�������ݻ��������ݵ��ײ�
	 * @param s {const char*} Դ���ݵ�ַ
	 * @return {string&} ��ǰ���������
	 */
	string& prepend(const char* s);

	/**
	 * ��ָ���ڴ���������ڵ�ǰ�������ݻ��������ݵ��ײ�
	 * @param ptr {const void*} Դ���ݵ�ַ
	 * @param len {size_t} ptr ���ݳ���
	 * @return {string&} ��ǰ���������
	 */
	string& prepend(const void* ptr, size_t len);

	/**
	 * ���ڴ����ݲ���ָ���±�λ�ÿ�ʼ�ĵ�ǰ���󻺳�����
	 * @param start {size_t} ��ǰ���󻺳����Ŀ�ʼ�����±�ֵ
	 * @param ptr {const void*} �ڴ����ݵĵ�ַ
	 * @param len {size_t} �ڴ����ݵĳ���
	 * @return {string&} ��ǰ���������
	 */
	string& insert(size_t start, const void* ptr, size_t len);

	/**
	 * ����ʽ��ʽ��������ݣ������� sprintf �ӿڷ�ʽ��
	 * @param fmt {const char*} ��ʽ�ַ���
	 * @param ... �������
	 * @return {string&} ��ǰ���������
	 */
#ifdef	WIN32
	string& format(const char* fmt, ...);
#else
	string& format(const char* fmt, ...) __attribute__((format(printf, 2, 3)));
#endif

	/**
	 * ����ʽ��ʽ��������ݣ������� vsprintf �ӿڷ�ʽ��
	 * @param fmt {const char*} ��ʽ�ַ���
	 * @param ap {va_list} �������
	 * @return {string&} ��ǰ���������
	 */
	string& vformat(const char* fmt, va_list ap);

	/**
	 * ����ʽ��ʽ�ڵ�ǰ�����β���������
	 * @param fmt {const char*} ��ʽ�ַ���
	 * @param ... �������
	 * @return {string&} ��ǰ���������
	 */
	string& format_append(const char* fmt, ...);

	/**
	 * ����ʽ��ʽ�ڵ�ǰ�����β���������
	 * @param fmt {const char*} ��ʽ�ַ���
	 * @param ap {va_list} �������
	 * @return {string&} ��ǰ���������
	 */
	string& vformat_append(const char* fmt, va_list ap);

	/**
	 * ����ǰ�����е����ݵ��ַ������滻
	 * @param from {char} Դ�ַ�
	 * @param to {char} Ŀ���ַ�
	 * @return {string&} ��ǰ���������
	 */
	string& replace(char from, char to);

	/**
	 * ����ǰ��������ݽض̣��������ڲ��ƶ��±�ָ���ַ
	 * @param n {size_t} ���ݽض̺�����ݳ��ȣ������ֵ >= ��ǰ������
	 *  ���ݳ��ȣ����ڲ������κα仯
	 * @return {string&} ��ǰ���������
	 */
	string& truncate(size_t n);

	/**
	 * �ڵ�ǰ����Ļ�����������ȥ��ָ�����ַ������ݣ��ڴ�������лᷢ�������ƶ����
	 * @param needle {const char*} ָ����Ҫȥ�����ַ�������
	 * @param each {bool} �Ƿ�ȥ�����е�ָ���ַ�������
	 * @return {string&} ��ǰ���������
	 */
	string& strip(const char* needle, bool each = false);

	/**
	 * ��յ�ǰ��������ݻ�����
	 * @return {string&} ��ǰ���������
	 */
	string& clear();

	/**
	 * ����ǰ��������ݻ������е����ݾ�תΪСд
	 * @return {string&} ��ǰ���������
	 */
	string& lower(void);

	/**
	 * ����ǰ��������ݻ������е����ݾ�תΪ��д
	 * @return {string&} ��ǰ���������
	 */
	string& upper(void);

	/**
	 * ����ǰ��������ݻ������е����ݽ��� base64 ת��
	 * @return {string&} ��ǰ���������
	 */
	string& base64_encode(void);

	/**
	 * �������Դ���ݽ��� base64 ת�벢���뵱ǰ����Ļ�������
	 * @param ptr {const void*} Դ���ݵĵ�ַ
	 * @param len {size_t} Դ���ݳ���
	 * @return {string&} ��ǰ���������
	 */
	string& base64_encode(const void* ptr, size_t len);

	/**
	 * �����ǰ����Ļ������е������Ǿ� base64 ����ģ���˺�������Щ���ݽ��н���
	 * @return {string&} ��ǰ���������
	 */
	string& base64_decode(void);

	/**
	 * ������� base64 ��������ݽ��н��벢���뵱ǰ����Ļ�������
	 * @param s {const char*} �� base64 �����Դ����
	 * @return {string&} ��ǰ���������
	 */
	string& base64_decode(const char* s);

	/**
	 * ������� base64 ��������ݽ��н��벢���뵱ǰ����Ļ�������
	 * @param ptr {const void*} �� base64 �����Դ����
	 * @param len {size_t} ptr ���ݳ���
	 * @return {string&} ��ǰ���������
	 */
	string& base64_decode(const void* ptr, size_t len);

	/**
	 * �������Դ���ݽ��� url ���벢���뵱ǰ����Ļ�������
	 * @param s {const char*} Դ����
	 * @return {string&} ��ǰ���������
	 */
	string& url_encode(const char* s);

	/**
	 * ��������� url �����Դ���ݽ��벢���뵱ǰ����Ļ�������
	 * @param s {const char*} �� url �����Դ����
	 * @return {string&} ��ǰ���������
	 */
	string& url_decode(const char* s);

	/**
	 * ��Դ���ݽ��� H2B ���벢���뵱ǰ����Ļ�������
	 * @param s {const void*} Դ���ݵ�ַ
	 * @param len {size_t} Դ���ݳ���
	 * @return {string&} ��ǰ���������
	 */
	string& hex_encode(const void* s, size_t len);

	/**
	 * ��Դ���ݽ��� H2B ���벢���뵱ǰ����Ļ�������
	 * @param s {const char*} Դ���ݵ�ַ
	 * @param len {size_t} Դ���ݳ���
	 * @return {string&} ��ǰ���������
	 */
	string& hex_decode(const char* s, size_t len);

	/**
	 * �� 32 λ�з�������תΪ�ַ�����
	 * @param n {int} 32 λ�з�������
	 * @return {const string&} ת��������������
	 */
	static const string& parse_int(int n);

	/**
	 * �� 32 λ�޷�������תΪ�ַ�����
	 * @param n {int} 32 λ�޷�������
	 * @return {const string&} ת��������������
	 */
	static const string& parse_int(unsigned int n);
#ifdef WIN32
	static const string& parse_int64(__int64 n);


	static const string& parse_int64(unsigned __int64 n);
#else
	/**
	 * �� 64 λ�з�������תΪ�ַ�����
	 * @param n {long long int} 64 λ�з�������
	 * @return {const string&} ת��������������
	 */
	static const string& parse_int64(long long int n);

	/**
	 * �� 64 λ�޷�������תΪ�ַ�����
	 * @param n {unsigned long long int} 64 λ�޷�������
	 * @return {const string&} ת��������������
	 */
	static const string& parse_int64(unsigned long long int n);
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
