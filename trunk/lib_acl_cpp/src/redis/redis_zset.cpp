#include "acl_stdafx.hpp"
#include "acl_cpp/stdlib/dbuf_pool.hpp"
#include "acl_cpp/redis/redis_client.hpp"
#include "acl_cpp/redis/redis_zset.hpp"

namespace acl
{

#define BUFLEN	64
#define INTLEN	11

redis_zset::redis_zset(redis_client* conn /* = NULL */)
: redis_command(conn)
{

}

redis_zset::~redis_zset()
{

}

int redis_zset::zadd(const char* key,
	std::vector<std::pair<double, string> >&members)
{
	dbuf_pool* pool = conn_->get_pool();
	size_t argc = 2 + members.size();
	const char** argv = (const char**)
		pool->dbuf_alloc(argc * sizeof(char*));
	size_t* lens = (size_t*) pool->dbuf_alloc(argc * sizeof(size_t));

	argv[0] = "ZADD";
	lens[0] = sizeof("ZADD") - 1;

	argv[1] = key;
	lens[1] = strlen(key);

	char* buf;
	size_t i = 2;
	std::vector<std::pair<double, string> >::const_iterator cit;
	for (cit = members.begin(); cit != members.end(); ++cit)
	{
		buf = (char*) pool->dbuf_alloc(BUFLEN);
		safe_snprintf(buf, BUFLEN, "%.8f", (*cit).first);
		argv[i] = buf;
		lens[i] = strlen(buf);
		i++;

		argv[i] = (*cit).second.c_str();
		lens[i] = (*cit).second.length();
		i++;
	}

	const string& req = conn_->build_request(argc, argv, lens);
	return conn_->get_number(req);
}

int redis_zset::zadd(const char* key,
	std::vector<std::pair<double, const char*> >&members)
{
	dbuf_pool* pool = conn_->get_pool();
	size_t argc = 2 + members.size();
	const char** argv = (const char**)
		pool->dbuf_alloc(argc * sizeof(char*));
	size_t* lens = (size_t*) pool->dbuf_alloc(argc * sizeof(size_t));

	argv[0] = "ZADD";
	lens[0] = sizeof("ZADD") - 1;

	argv[1] = key;
	lens[1] = strlen(key);

	char* buf;
	size_t i = 2;
	std::vector<std::pair<double, const char*> >::const_iterator cit;

	for (cit = members.begin(); cit != members.end(); ++cit)
	{
		buf = (char*) pool->dbuf_alloc(BUFLEN);
		safe_snprintf(buf, BUFLEN, "%.8f", (*cit).first);
		argv[i] = buf;
		lens[i] = strlen(buf);
		i++;

		argv[i] = (*cit).second;
		lens[i] = strlen(argv[i]);
		i++;
	}

	const string& req = conn_->build_request(argc, argv, lens);
	return conn_->get_number(req);
}

int redis_zset::zadd(const char* key, std::vector<double>& scores,
	std::vector<string>& members)
{
	size_t size = scores.size();
	if (size != members.size())
		return -1;

	dbuf_pool* pool = conn_->get_pool();
	size_t argc = 2 + scores.size() * 2;
	const char** argv = (const char**)
		pool->dbuf_alloc(argc * sizeof(char*));
	size_t* lens = (size_t*) pool->dbuf_alloc(argc * sizeof(size_t));

	argv[0] = "ZADD";
	lens[0] = sizeof("ZADD") - 1;

	argv[1] = key;
	lens[1] = strlen(key);

	size_t j = 2;
	char* buf;

	for (size_t i = 0; i < size; i++)
	{
		buf = (char*) pool->dbuf_alloc(BUFLEN);
		safe_snprintf(buf, BUFLEN, "%.8f", scores[i]);
		argv[j] = buf;
		lens[j] = strlen(buf);
		j++;

		argv[j] = members[i].c_str();
		lens[j] = strlen(argv[j]);
		j++;
	}

	const string& req = conn_->build_request(argc, argv, lens);
	return conn_->get_number(req);
}

int redis_zset::zadd(const char* key, std::vector<double>& scores,
	std::vector<const char*>& members)
{
	size_t size = scores.size();
	if (size != members.size())
		return -1;

	dbuf_pool* pool = conn_->get_pool();
	size_t argc = 2 + scores.size() * 2;
	const char** argv = (const char**)
		pool->dbuf_alloc(argc * sizeof(char*));
	size_t* lens = (size_t*) pool->dbuf_alloc(argc * sizeof(size_t));

	argv[0] = "ZADD";
	lens[0] = sizeof("ZADD") - 1;

	argv[1] = key;
	lens[1] = strlen(key);

	size_t j = 2;
	char* buf;

	for (size_t i = 0; i < size; i++)
	{
		buf = (char*) pool->dbuf_alloc(BUFLEN);
		safe_snprintf(buf, BUFLEN, "%.8f", scores[i]);
		argv[j] = buf;
		lens[j] = strlen(buf);
		j++;

		argv[j] = members[i];
		lens[j] = strlen(argv[j]);
		j++;
	}

	const string& req = conn_->build_request(argc, argv, lens);
	return conn_->get_number(req);
}

int redis_zset::zadd(const char* key, double scores[], const char* members[],
	size_t size)
{
	dbuf_pool* pool = conn_->get_pool();
	size_t argc = 2 + size * 2;
	const char** argv = (const char**)
		pool->dbuf_alloc(argc * sizeof(char*));
	size_t* lens = (size_t*) pool->dbuf_alloc(argc * sizeof(size_t));

	argv[0] = "ZADD";
	lens[0] = sizeof("ZADD") - 1;

	argv[1] = key;
	lens[1] = strlen(key);

	size_t j = 2;
	char* buf;

	for (size_t i = 0; i < size; i++)
	{
		buf = (char*) pool->dbuf_alloc(BUFLEN);
		safe_snprintf(buf, BUFLEN, "%.8f", scores[i]);
		argv[j] = buf;
		lens[j] = strlen(buf);
		j++;

		argv[j] = members[i];
		lens[j] = strlen(argv[j]);
		j++;
	}

	const string& req = conn_->build_request(argc, argv, lens);
	return conn_->get_number(req);
}

int redis_zset::zadd(const char* key, double scores[], const char* members[],
	size_t members_len[], size_t size)
{
	dbuf_pool* pool = conn_->get_pool();
	size_t argc = 2 + size * 2;
	const char** argv = (const char**)
		pool->dbuf_alloc(argc * sizeof(char*));
	size_t* lens = (size_t*) pool->dbuf_alloc(argc * sizeof(size_t));

	argv[0] = "ZADD";
	lens[0] = sizeof("ZADD") - 1;

	argv[1] = key;
	lens[1] = strlen(key);

	size_t j = 2;
	char* buf;

	for (size_t i = 0; i < size; i++)
	{
		buf = (char*) pool->dbuf_alloc(BUFLEN);
		safe_snprintf(buf, BUFLEN, "%.8f", scores[i]);
		argv[j] = buf;
		lens[j] = strlen(buf);
		j++;

		argv[j] = members[i];
		lens[j] = members_len[i];
		j++;
	}

	const string& req = conn_->build_request(argc, argv, lens);
	return conn_->get_number(req);
}

int redis_zset::zcard(const char* key)
{
	const char* argv[2];
	size_t lens[2];

	argv[0] = "ZCARD";
	lens[0] = sizeof("ZCARD") - 1;

	argv[1] = key;
	lens[1] = strlen(key);

	const string& req = conn_->build_request(2, argv, lens);
	return conn_->get_number(req);
}

int redis_zset::zcount(const char* key, double min, double max)
{
	const char* argv[4];
	size_t lens[4];

	argv[0] = "ZCOUNT";
	lens[0] = sizeof("ZCOUNT") - 1;

	argv[1] = key;
	lens[1] = strlen(key);

	char min_buf[BUFLEN], max_buf[BUFLEN];
	safe_snprintf(min_buf, sizeof(min_buf), "%.8f", min);
	safe_snprintf(max_buf, sizeof(max_buf), "%.8f", max);

	argv[2] = min_buf;
	lens[2] = strlen(min_buf);

	argv[3] = max_buf;
	lens[3] = strlen(max_buf);

	const string& req = conn_->build_request(4, argv, lens);
	return conn_->get_number(req);
}

bool redis_zset::zincrby(const char* key, double inc,
	const char* member, double* result /* = NULL */)
{
	return zincrby(key, inc, member, strlen(member), result);
}

bool redis_zset::zincrby(const char* key, double inc,
	const char* member, size_t len, double* result /* = NULL */)
{
	const char* argv[4];
	size_t lens[4];

	argv[0] = "ZINCRBY";
	lens[0] = sizeof("ZINCRBY") - 1;

	argv[1] = key;
	lens[1] = strlen(key);

	char score[BUFLEN];
	safe_snprintf(score, sizeof(score), "%.8f", inc);
	argv[2] = score;
	lens[2] = strlen(score);

	argv[3] = member;
	lens[3] = len;

	const string& req = conn_->build_request(4, argv, lens);
	int ret = conn_->get_string(req, score, sizeof(score));
	if (ret < 0)
		return false;
	if (result)
		*result = atof(score);
	return true;
}

int redis_zset::zrange(const char* key, int start, int end,
	std::vector<string>& result)
{
	const char* argv[4];
	size_t lens[4];

	argv[0] = "ZRANGE";
	lens[0] = sizeof("ZRANGE") - 1;

	argv[1] = key;
	lens[1] = strlen(key);

	char start_s[INTLEN], end_s[INTLEN];
	safe_snprintf(start_s, sizeof(start_s), "%d", start);
	safe_snprintf(end_s, sizeof(end_s), "%d", end);

	argv[2] = start_s;
	lens[2] = strlen(start_s);

	argv[3] = end_s;
	lens[3] = strlen(end_s);

	const string& req = conn_->build_request(4, argv, lens);
	return conn_->get_strings(req, result);
}

int redis_zset::zrangebyscore(const char* key, double min,
	double max, std::vector<string>& out,
	const int* offset /* = NULL */, const int* count /* = NULL */)
{
	const char* argv[8];
	size_t lens[8];
	size_t argc = 4;

	argv[0] = "ZRANGEBYSCORE";
	lens[0] = sizeof("ZRANGEBYSCORE") - 1;

	argv[1] = key;
	lens[1] = strlen(key);

	char min_s[BUFLEN], max_s[BUFLEN];
	safe_snprintf(min_s, sizeof(min_s), "%.8f", min);
	safe_snprintf(max_s, sizeof(max_s), "%.8f", max);

	argv[2] = min_s;
	lens[2] = strlen(min_s);

	argv[3] = max_s;
	lens[3] = strlen(max_s);

	char offset_s[INTLEN], count_s[INTLEN];
	if (offset && count)
	{
		safe_snprintf(offset_s, sizeof(offset_s), "%d", *offset);
		safe_snprintf(count_s, sizeof(count_s), "%d", count);

		argv[4] = "LIMIT";
		lens[4] = sizeof("LIMIT") - 1;

		argv[5] = offset_s;
		lens[5] = strlen(offset_s);

		argv[6] = count_s;
		lens[6] = strlen(count_s);

		argc += 2;
	}

	const string& req = conn_->build_request(argc, argv, lens);
	return conn_->get_strings(req, out);
}

int redis_zset::zrangebyscore_with_scores(const char* key, double min,
	double max, std::vector<std::pair<double, string> >& out,
	const int* offset /* = NULL */, const int* count /* = NULL */)
{
	const char* argv[8];
	size_t lens[8];
	size_t argc = 5;

	argv[0] = "ZRANGEBYSCORE";
	lens[0] = sizeof("ZRANGEBYSCORE") - 1;

	argv[1] = key;
	lens[1] = strlen(key);

	char min_s[BUFLEN], max_s[BUFLEN];
	safe_snprintf(min_s, sizeof(min_s), "%.8f", min);
	safe_snprintf(max_s, sizeof(max_s), "%.8f", max);

	argv[2] = min_s;
	lens[2] = strlen(min_s);

	argv[3] = max_s;
	lens[3] = strlen(max_s);

	argv[4] = "WITHSCORES";
	lens[4] = sizeof("WITHSCORES") - 1;

	char offset_s[INTLEN], count_s[INTLEN];
	if (offset && count)
	{
		safe_snprintf(offset_s, sizeof(offset_s), "%d", *offset);
		safe_snprintf(count_s, sizeof(count_s), "%d", count);

		argv[5] = "LIMIT";
		lens[5] = sizeof("LIMIT") - 1;

		argv[6] = offset_s;
		lens[6] = strlen(offset_s);

		argv[7] = count_s;
		lens[7] = strlen(count_s);

		argc += 2;
	}

	const string& req = conn_->build_request(argc, argv, lens);
	const redis_result* result = conn_->run(req);
	if (result == NULL || result->get_type() != REDIS_RESULT_ARRAY)
		return -1;

	size_t size;
	const redis_result** children = result->get_children(&size);
	if (children == NULL || size == 0)
		return 0;
	if (size % 2 != 0)
		return -1;

	size /= 2;
	out.reserve(size);
	double score;
	const redis_result* child;
	string buf(128);

	for (size_t i = 0, j = 0; i < size; i++)
	{
		child = children[i * 2];
		if (child == NULL)
			continue;
		child->argv_to_string(buf);
		score = atof(buf.c_str());

		child = children[(i + 1) * 2];
		if (child == NULL)
			continue;
		buf.clear();
		child->argv_to_string(buf);

		out.push_back(std::make_pair(score, buf));
	}

	return size;
}

int redis_zset::zrank(const char* key, const char* member)
{
	return zrank(key, member, strlen(member));
}

int redis_zset::zrank(const char* key, const char* member, size_t len)
{
	const char* argv[3];
	size_t lens[3];

	argv[0] = "ZRANK";
	lens[0] = sizeof("ZRANK") - 1;

	argv[1] = key;
	lens[1] = strlen(key);

	argv[2] = member;
	lens[2] = len;

	const string& req = conn_->build_request(3, argv, lens);
	return conn_->get_number(req);
}

int redis_zset::zrem(const char* key, const char* first_member, ...)
{
	std::vector<const char*> members;
	members.push_back(first_member);
	va_list ap;
	va_start(ap, first_member);
	const char* member;
	while ((member = va_arg(ap, const char*)) != NULL)
		members.push_back(member);

	return zrem(key, members);
}

int redis_zset::zrem(const char* key, const std::vector<string>& members)
{
	const string& req = conn_->build("ZREM", key, members);
	return conn_->get_number(req);
}

int redis_zset::zrem(const char* key, const std::vector<const char*>& members)
{
	const string& req = conn_->build("ZREM", key, members);
	return conn_->get_number(req);
}

int redis_zset::zrem(const char* key, const char* members[],
	const size_t lens[], size_t argc)
{
	const string& req = conn_->build("ZREM", key, members, lens, argc);
	return conn_->get_number(req);
}

} // namespace acl
