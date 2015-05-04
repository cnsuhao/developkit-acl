#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <vector>
#include <map>
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/redis/redis_command.hpp"

namespace acl
{

class redis_client;

class ACL_CPP_API disque : virtual public redis_command
{
public:
	/**
	 * see redis_command::redis_command()
	 */
	disque();

	/**
	 * see redis_command::redis_command(redis_client*)
	 */
	disque(redis_client* conn);

	/**
	 * see redis_command::redis_command(redis_client_cluster*£¬ size_t)
	 */
	disque(redis_client_cluster* cluster, size_t max_conns);

	virtual ~disque();

	/////////////////////////////////////////////////////////////////////

	const char* addjob(const char* name, const char* job,
		int timeout, const std::map<string, int>* args = NULL);
	const char* addjob(const char* name, const string& job,
		int timeout, const std::map<string, int>* args = NULL);
	const char* addjob(const char* name, const void* job, size_t job_len,
		int timeout, const std::map<string, int>* args = NULL);

	int getjob(const std::vector<string>& names, std::vector<string>& out,
		size_t timeout, size_t count);
	int qlen(const char* name);
	int qpeek(const char* name, int count, std::vector<string>& out);
	bool show(const char* job_id, std::map<string, string>& out);
	int ackjob(const std::vector<string>& jobs);
	int fastack(const std::vector<string>& jobs);
	bool info(std::map<string, string>& out);
	bool hello(std::map<string, string>& result);
	int enqueue(const std::vector<string>& jobs);
	int dequeue(const std::vector<string>& jobs);
	int deljob(const std::vector<string>& jobs);

private:

};

} // namespace acl
