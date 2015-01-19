#include "acl_stdafx.hpp"
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/redis/redis_client.hpp"
#include "acl_cpp/redis/redis_result.hpp"
#include "acl_cpp/redis/redis_command.hpp"

namespace acl
{

redis_command::redis_command(redis_client* conn /* = NULL */)
: conn_(conn)
, result_(NULL)
{

}

redis_command::~redis_command()
{

}

void redis_command::reset()
{
	if (conn_)
		conn_->reset();
}

void redis_command::set_client(redis_client* conn)
{
	conn_ = conn;
}

int redis_command::get_number(const string& req, bool* success /* = NULL */)
{
	result_ = conn_->run(req);
	if (result_ == NULL || result_->get_type() != REDIS_RESULT_INTEGER)
	{
		if (success)
			*success = false;
		return -1;
	}
	if (success)
		*success = true;
	return result_->get_integer();
}

long long int redis_command::get_number64(const string& req
	, bool* success /* = NULL */)
{
	result_ = conn_->run(req);
	if (result_ == NULL || result_->get_type() != REDIS_RESULT_INTEGER)
	{
		if (success)
			*success = false;
		return -1;
	}
	if (success)
		*success = true;
	return result_->get_integer64();
}

bool redis_command::get_status(const string& req, const char* success /* = "OK" */)
{
	result_ = conn_->run(req);
	if (result_ == NULL || result_->get_type() != REDIS_RESULT_STATUS)
		return false;
	const char* status = result_->get_status();
	if (status == NULL || strcasecmp(status, success) != 0)
		return false;
	return true;
}

long redis_command::get_string(const string& req, string& buf)
{
	result_ = conn_->run(req);
	if (result_ == NULL || result_->get_type() != REDIS_RESULT_STRING)
		return -1;
	return (long) result_->argv_to_string(buf);
}

long redis_command::get_string(const string& req, string* buf)
{
	result_ = conn_->run(req);
	if (result_ == NULL || result_->get_type() != REDIS_RESULT_STRING)
		return -1;
	if (buf == NULL)
		return (long) result_->get_length();
	return (long) result_->argv_to_string(*buf);
}

long redis_command::get_string(const string& req, char* buf, size_t size)
{
	result_ = conn_->run(req);
	if (result_ == NULL || result_->get_type() != REDIS_RESULT_STRING)
		return -1;
	return (long) result_->argv_to_string(buf, size);
}

long redis_command::get_strings(const string& req, std::vector<string>& result)
{
	result_ = conn_->run(req);
	if (result_ == NULL || result_->get_type() != REDIS_RESULT_ARRAY)
		return -1;

	size_t size;
	const redis_result** children = result_->get_children(&size);
	if (children == NULL)
		return 0;

	const redis_result* rr;
	string buf;
	for (size_t i = 0; i < size; i++)
	{
		rr = children[i];
		if (rr->get_type() != REDIS_RESULT_STRING)
			continue;
		rr->argv_to_string(buf);
		result.push_back(buf);
		buf.clear();
	}

	return (long) size;
}

long redis_command::get_strings(const string& req,
	std::map<string, string>& result)
{
	result_ = conn_->run(req);
	if (result_ == NULL)
		return -1;
	if (result_->get_type() != REDIS_RESULT_ARRAY)
		return -1;
	if (result_->get_size() == 0)
		return 0;

	size_t size;
	const redis_result** children = result_->get_children(&size);
	if (children == NULL)
		return -1;
	if (size % 2 != 0)
		return -1;

	string name_buf, value_buf;

	const redis_result* rr;
	for (size_t i = 0; i < size;)
	{
		rr = children[i];
		if (rr->get_type() != REDIS_RESULT_STRING)
		{
			i += 2;
			continue;
		}
		name_buf.clear();
		value_buf.clear();
		rr->argv_to_string(name_buf);
		i++;
		rr->argv_to_string(value_buf);
		i++;
		result[name_buf] = value_buf;
	}

	return (long) result.size();
}

long redis_command::get_strings(const string& req, std::vector<string>& names,
	std::vector<string>& values)
{
	result_ = conn_->run(req);
	if (result_ == NULL)
		return -1;
	if (result_->get_type() != REDIS_RESULT_ARRAY)
		return -1;
	if (result_->get_size() == 0)
		return 0;

	size_t size;
	const redis_result** children = result_->get_children(&size);

	if (children == NULL)
		return -1;
	if (size % 2 != 0)
		return -1;

	string buf;
	const redis_result* rr;

	for (size_t i = 0; i < size;)
	{
		rr = children[i];
		if (rr->get_type() != REDIS_RESULT_STRING)
		{
			i += 2;
			continue;
		}
		buf.clear();
		rr->argv_to_string(buf);
		i++;
		names.push_back(buf);

		buf.clear();
		rr->argv_to_string(buf);
		i++;
		values.push_back(buf);
	}

	return (long) names.size();
}

long redis_command::get_strings(const string& req,
	std::vector<const char*>& names, std::vector<const char*>& values)
{
	result_ = conn_->run(req);
	if (result_ == NULL)
		return -1;
	if (result_->get_type() != REDIS_RESULT_ARRAY)
		return -1;
	if (result_->get_size() == 0)
		return 0;

	size_t size;
	const redis_result** children = result_->get_children(&size);

	if (children == NULL)
		return -1;
	if (size % 2 != 0)
		return -1;

	char* buf;
	size_t len;
	const redis_result* rr;
	dbuf_pool* pool = conn_->get_pool();
	std::vector<const redis_result*>::const_iterator cit;

	for (size_t i = 0; i < size;)
	{
		rr = children[i];
		if (rr->get_type() != REDIS_RESULT_STRING)
		{
			i += 2;
			continue;
		}

		len = rr->get_length() + 1;
		buf = (char*) pool->dbuf_alloc(len);
		rr->argv_to_string(buf, len);
		i++;
		names.push_back(buf);

		len = rr->get_length() + 1;
		buf = (char*) pool->dbuf_alloc(len);
		rr->argv_to_string(buf, len);
		i++;
		values.push_back(buf);
	}

	return (long) names.size();
}

} // namespace acl
