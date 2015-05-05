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
class disque_cond;
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
	const char* addjob(const char* name, const char* job,
		int timeout, const disque_cond* cond);
	const char* addjob(const char* name, const string& job,
		int timeout, const disque_cond* cond);
	const char* addjob(const char* name, const void* job, size_t job_len,
		int timeout, const disque_cond* cond);

	const std::vector<disque_job*>* getjob(const std::vector<string>& names,
		size_t timeout, size_t count);
	const std::vector<disque_job*>* qpeek(const char* name, int count);
	int qlen(const char* name);
	const disque_job* show(const char* job_id);
	int ackjob(const std::vector<string>& job_ids);
	int fastack(const std::vector<string>& job_ids);
	int enqueue(const std::vector<string>& job_ids);
	int dequeue(const std::vector<string>& job_ids);
	int deljob(const std::vector<string>& job_ids);
	bool info(std::map<string, string>& out);
	const std::vector<disque_node*>* hello();

private:
	int jobs_bat(const std::vector<string>& job_ids, const char* cmd);

private:
	disque_job* job_;
	std::vector<disque_job*> jobs_;

	const std::vector<disque_job*>* get_jobs(const char* name);
	void free_jobs();

private:
	int version_;
	string myid_;
	std::vector<disque_node*> nodes_;

	disque_node* create_node(const redis_result* rr);
	void current_node(const redis_result* rr);
	void free_nodes();
};

} // namespace acl
