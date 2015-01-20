#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <vector>
#include <utility>
#include "acl_cpp/redis/redis_command.hpp"

namespace acl
{

class redis_client;
class redis_result;

class ACL_CPP_API redis_zset : public redis_command
{
public:
	redis_zset(redis_client* conn = NULL);
	~redis_zset();

	/////////////////////////////////////////////////////////////////////

	/**
	 * ��Ӷ�Ӧ key ������
	 * @param key {const char*} ���򼯼�ֵ
	 * @param members "��ֵ-��Ա" ������
	 * @return {int} �³ɹ���ӵ� "��ֵ-��Ա" �Ե�����
	 *  0����ʾһ��Ҳδ��ӣ�������Ϊ�ó�Ա�Ѿ�������������
	 * -1����ʾ����� key ��������򼯶���
	 * >0������ӵĳ�Ա����
	 */
	int zadd(const char* key,
		std::vector<std::pair<double, string> >&members);
	int zadd(const char* key,
		std::vector<std::pair<double, const char*> >&members);
	int zadd(const char* key, std::vector<double>& scores,
		std::vector<string>& members);
	int zadd(const char* key, std::vector<double>& scores,
		std::vector<const char*>& members);
	int zadd(const char* key, double scores[], const char* members[],
		size_t size);
	int zadd(const char* key, double scores[], const char* members[],
		size_t members_len[], size_t size);

	/**
	 * �����Ӧ�������򼯵ĳ�Ա����
	 * @param key {const char*} ���򼯼�ֵ
	 * @return {int} һ���������򼯵ĳ�Ա����
	 *  0���ü�������
	 *  -1�������ü������ݶ�������Ч�����򼯶���
	 *  > 0����ǰ��ֵ��Ӧ�����ݶ����еĳ�Ա����
	 */
	int zcard(const char* key);

	/**
	 * ��� key ��������ָ����ֵ����ĳ�Ա����
	 * @param key {const char*} ���򼯼�ֵ
	 * @param min {double} ��С��ֵ
	 * @param max {double} ����ֵ
	 * @return {int} ���������ĳ�Ա����
	 *  0���ü���Ӧ�����򼯲����ڻ�� KEY ���򼯵Ķ�Ӧ��ֵ�����ԱΪ��
	 *  -1: �����ü������ݶ�������Ч�����򼯶���
	 */
	int zcount(const char* key, double min, double max);

	/**
	 * �� key �������е�ĳ����Ա�ķ�ֵ�������� inc
	 * @param key {const char*} ���򼯼�ֵ
	 * @param inc {double} ����ֵ
	 * @param member{const char*} �����г�Ա��
	 * @param result {double*} �ǿ�ʱ�洢���ֵ
	 * @return {bool} �����Ƿ�ɹ�
	 */
	bool zincrby(const char* key, double inc,
		const char* member, double* result = NULL);
	bool zincrby(const char* key, double inc,
		const char* member, size_t len, double* result = NULL);

	/**
	 * �� key �������л��ָ��λ������ĳ�Ա���б���Ա����ֵ������ʽ����
	 * @param key {const char*} ���򼯼�ֵ
	 * @param start {int} ��ʼ�±�λ��
	 * @param end {int} �����±�λ�ã������ͬʱ����λ�ã�
	 * @param result {std::vector<string>&} �洢�����
	 *  ע�������±�λ�ã�0 ��ʾ��һ����Ա��1 ��ʾ�ڶ�����Ա��-1 ��ʾ���һ����Ա��
	 *     -2 ��ʾ�����ڶ�����Ա���Դ�����
	 */
	int zrange(const char* key, int start, int end,
		std::vector<string>& result);

	/**
	 * �������� key �У����� score ֵ���� min �� max ֮��(�������� min �� max )
	 * �ĳ�Ա�����򼯳�Ա�� score ֵ����(��С����)��������
	 * @param key {const char*} ���򼯼�ֵ
	 * @param min {double} ��С��ֵ
	 * @param max {double} ����ֵ
	 * @param out �洢����ֵ-��Ա�����ԵĽ����
	 * @param offset {const int*} �ǿ�ʱ��ʾ���������ʼ�±�
	 * @param count {const int*} �ǿ�ʱ��ʾ��ȡ�Ľ�����г�Ա����
	 * @return {int} ������г�Ա������
	 *  0: ��ʾ�����Ϊ�ջ� key ������
	 * -1: ��ʾ����� key ��������򼯶���
	 * >0: �����������
	 *  ע��offset �� count ����ͬʱΪ�ǿ�ָ��ʱ����Ч
	 */
	int zrangebyscore(const char* key, double min, double max,
		std::vector<string>& out, const int* offset = NULL,
		const int* count = NULL);
	int zrangebyscore_with_scores(const char* key, double min, double max,
		std::vector<std::pair<double, string> >& out,
		const int* offset = NULL, const int* count = NULL);

	/**
	 * ���ĳ����Ա�� key �������е��±�����λ�ã���0��ʼ��
	 * @param key {const char*} ���򼯼�ֵ
	 * @param member {const char*} ��Ա��
	 * @param len {size_t} member �ĳ���
	 * @return {int} �±�λ��ֵ��-1 -- ������ key �����򼯶��󣬻��Ա��������
	 */
	int zrank(const char* key, const char* member, size_t len);
	int zrank(const char* key, const char* member);

	/**
	 * ��������ɾ��ĳ����Ա
	 * @param key {const char*} ���򼯼�ֵ
	 * @param first_member {const char*} Ҫɾ���ĳ�Ա�б�ĵ�һ��
	 * @return {int} �ɹ�ɾ���ĳ�Ա��������-1 ��ʾ������ key �����򼯶���
	 *  0 ��ʾ�����򼯲����ڻ��Ա�����ڣ�> 0 ��ʾ�ɹ�ɾ���ĳ�Ա����
	 */
	int zrem(const char* key, const char* first_member, ...);
	int zrem(const char* key, const std::vector<string>& members);
	int zrem(const char* key, const std::vector<const char*>& members);
	int zrem(const char* key, const char* members[], const size_t lens[],
		size_t argc);
};

} // namespace acl
