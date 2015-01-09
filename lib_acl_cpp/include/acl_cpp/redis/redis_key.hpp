#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <vector>
#include "acl_cpp/stdlib/string.hpp"

namespace acl {

typedef enum
{
	REDIS_KEY_UNKNOWN,
	REDIS_KEY_NONE,
	REDIS_KEY_STRING,
	REDIS_KEY_HASH,
	REDIS_KEY_LIST,
	REDIS_KEY_SET,
	REDIS_KEY_ZSET
} redis_key_t;

class redis_key
{
public:
	redis_key(redis_client& conn);
	~redis_key();

	/**
	 * ɾ��һ�� KEY
	 * @return {int} ������ɾ���� KEY �ĸ��������£�
	 *  0: δɾ���κ� KEY
	 *  -1: ����
	 *  >0: ����ɾ���� KEY �ĸ�������ֵ�п������������ KEY �ĸ���
	 */
	int del_keys(const char* first_key, ...) ACL_CPP_PRINTF(2, 3);
	int del_keys(const std::vector<string>& keys);
	int del_keys(const std::vector<char*>& keys);
	int del_keys(const std::vector<const char*>& keys);
	int del_keys(const std::vector<int>& keys);
	int del_keys(const char* keys[], size_t argc);
	int del_keys(const int keys[], size_t argc);
	int del_keys(const char* keys[], const size_t lens[], size_t argc);

	/**
	 * ���� KEY ���������ڣ���λ���룩
	 * @param key {const char*} ��ֵ
	 * @param n {int} �������ڣ��룩
	 * @return {int} ����ֵ�������£�
	 *  > 0: �ɹ���������������
	 *  0���� key �����ڻ�ü�δ������������
	 *  < 0: ����
	 */
	int set_ttl(const char* key, int n);

	/**
	 * ��� KEY ��ʣ���������ڣ���λ���룩
	 * @param key {const char*} ��ֵ
	 * @return {int} ���ض�Ӧ��ֵ����������
	 *  > 0: �� key ʣ����������ڣ��룩
	 *  0���� key �����ڻ�ü�δ������������
	 *  < 0: ����
	 */
	int get_ttl(const char* key);

	/**
	 * �ж� KEY �Ƿ����
	 * @param key {const char*} KEY ֵ
	 * @return {bool} ���� true ��ʾ���ڣ������ʾ����򲻴���
	 */
	bool key_exists(const char* key);

	/**
	 * ��� KEY �Ĵ洢����
	 * @para key {const char*} KEY ֵ
	 * @return {redis_key_t} ���� KEY �Ĵ洢����
	 */
	redis_key_t get_key_type(const char* key);

private:
	redis_client& conn_;
};

} // namespace acl
