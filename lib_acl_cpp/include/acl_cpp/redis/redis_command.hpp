#pragma once
#include "acl_cpp/acl_cpp_define.hpp"

namespace acl
{

class redis_client;
class redis_result;

class ACL_CPP_API redis_command
{
public:
	redis_command(redis_client* conn = NULL);
	virtual ~redis_command() = 0;

	void reset();
	void set_client(redis_client* conn);
	redis_client* get_client() const
	{
		return conn_;
	}

	const redis_result* get_result() const
	{
		return result_;
	}

protected:
	redis_client* conn_;
	const redis_result* result_;

	int get_number(const string& req, bool* success = NULL);
	long long int get_number64(const string& req, bool* success = NULL);
	bool get_status(const string& req, const char* success = "OK");
	long get_string(const string& req, string& buf);
	long get_string(const string& req, string* buf);
	long get_string(const string& req, char* buf, size_t size);
	long get_strings(const string& req, std::vector<string>& result);
	long get_strings(const string& req, std::map<string, string>& result);
	long get_strings(const string& req, std::vector<string>& names,
		std::vector<string>& values);
	long get_strings(const string& req, std::vector<const char*>& names,
		std::vector<const char*>& values);
};

} // namespace acl
