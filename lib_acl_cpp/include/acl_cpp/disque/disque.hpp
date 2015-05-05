#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <vector>
#include <map>
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/redis/redis_command.hpp"

namespace acl
{

class redis_client;
class redis_client_cluster;
class disque_node;
class disque_job;

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
	const disque_job* show(const char* job_id);
	int ackjob(const std::vector<string>& job_ids);
	int fastack(const std::vector<string>& job_ids);
	int enqueue(const std::vector<string>& job_ids);
	int dequeue(const std::vector<string>& job_ids);
	int deljob(const std::vector<string>& job_ids);
	bool info(std::map<string, string>& out);
	bool hello(std::map<string, string>& result);
	const std::vector<disque_node*>* hello();

private:
	int jobs_bat(const std::vector<string>& job_ids, const char* cmd);

private:
	disque_job* job_;
	int version_;
	string myid_;
	std::vector<disque_node*> nodes_;

	disque_node* create_node(const redis_result* rr);
	void current_node(const redis_result* rr);
	void free_nodes();
};

} // namespace acl
