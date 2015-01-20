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
	 * 添加对应 key 的有序集
	 * @param key {const char*} 有序集键值
	 * @param members "分值-成员" 对数组
	 * @return {int} 新成功添加的 "分值-成员" 对的数量
	 *  0：表示一个也未添加，可能因为该成员已经存在于有序集中
	 * -1：表示出错或 key 对象非有序集对象
	 * >0：新添加的成员数量
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
	 * 获得相应键的有序集的成员数量
	 * @param key {const char*} 有序集键值
	 * @return {int} 一个键的有序集的成员数量
	 *  0：该键不存在
	 *  -1：出错或该键的数据对象不是有效的有序集对象
	 *  > 0：当前键值对应的数据对象中的成员个数
	 */
	int zcard(const char* key);

	/**
	 * 获得 key 的有序集中指定分值区间的成员个数
	 * @param key {const char*} 有序集键值
	 * @param min {double} 最小分值
	 * @param max {double} 最大分值
	 * @return {int} 符合条件的成员个数
	 *  0：该键对应的有序集不存在或该 KEY 有序集的对应分值区间成员为空
	 *  -1: 出错或该键的数据对象不是有效的有序集对象
	 */
	int zcount(const char* key, double min, double max);

	/**
	 * 将 key 的有序集中的某个成员的分值加上增量 inc
	 * @param key {const char*} 有序集键值
	 * @param inc {double} 增量值
	 * @param member{const char*} 有序集中成员名
	 * @param result {double*} 非空时存储结果值
	 * @return {bool} 操作是否成功
	 */
	bool zincrby(const char* key, double inc,
		const char* member, double* result = NULL);
	bool zincrby(const char* key, double inc,
		const char* member, size_t len, double* result = NULL);

	/**
	 * 从 key 的有序集中获得指定位置区间的成员名列表，成员按分值递增方式排序
	 * @param key {const char*} 有序集键值
	 * @param start {int} 起始下标位置
	 * @param end {int} 结束下标位置（结果集同时含该位置）
	 * @param result {std::vector<string>&} 存储结果集
	 *  注：对于下标位置，0 表示第一个成员，1 表示第二个成员；-1 表示最后一个成员，
	 *     -2 表示倒数第二个成员，以此类推
	 */
	int zrange(const char* key, int start, int end,
		std::vector<string>& result);

	/**
	 * 返回有序集 key 中，所有 score 值介于 min 和 max 之间(包括等于 min 或 max )
	 * 的成员。有序集成员按 score 值递增(从小到大)次序排列
	 * @param key {const char*} 有序集键值
	 * @param min {double} 最小分值
	 * @param max {double} 最大分值
	 * @param out 存储“分值-成员名”对的结果集
	 * @param offset {const int*} 非空时表示结果集的起始下标
	 * @param count {const int*} 非空时表示截取的结果集中成员个数
	 * @return {int} 结果集中成员的数量
	 *  0: 表示结果集为空或 key 不存在
	 * -1: 表示出错或 key 对象非有序集对象
	 * >0: 结果集的数量
	 *  注：offset 和 count 必须同时为非空指针时才有效
	 */
	int zrangebyscore(const char* key, double min, double max,
		std::vector<string>& out, const int* offset = NULL,
		const int* count = NULL);
	int zrangebyscore_with_scores(const char* key, double min, double max,
		std::vector<std::pair<double, string> >& out,
		const int* offset = NULL, const int* count = NULL);

	/**
	 * 获得某个成员在 key 的有序集中的下标排名位置（从0开始）
	 * @param key {const char*} 有序集键值
	 * @param member {const char*} 成员名
	 * @param len {size_t} member 的长度
	 * @return {int} 下标位置值，-1 -- 出错，或 key 非有序集对象，或成员名不存在
	 */
	int zrank(const char* key, const char* member, size_t len);
	int zrank(const char* key, const char* member);

	/**
	 * 从有序集中删除某个成员
	 * @param key {const char*} 有序集键值
	 * @param first_member {const char*} 要删除的成员列表的第一个
	 * @return {int} 成功删除的成员的数量，-1 表示出错或该 key 非有序集对象，
	 *  0 表示该有序集不存在或成员不存在，> 0 表示成功删除的成员数量
	 */
	int zrem(const char* key, const char* first_member, ...);
	int zrem(const char* key, const std::vector<string>& members);
	int zrem(const char* key, const std::vector<const char*>& members);
	int zrem(const char* key, const char* members[], const size_t lens[],
		size_t argc);
};

} // namespace acl
