#include "acl_stdafx.hpp"
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/stdlib/snprintf.hpp"
#include "acl_cpp/stdlib/dbuf_pool.hpp"
#include "acl_cpp/redis/disque.hpp"

namespace acl
{

#define INT_LEN		11

disque::disque()
: redis_command(NULL)
{

}

disque::disque(redis_client* conn)
: redis_command(conn)
{

}

disque::disque(redis_client_cluster* cluster, size_t max_conns)
: redis_command(cluster, max_conns)
{

}

disque::~disque()
{

}

const char* disque::addjob(const char* name, const char* job,
	int timeout, const std::map<string, int>* args /* = NULL */)
{
	return addjob(name, job, strlen(job), timeout, args);
}

const char* disque::addjob(const char* name, const string& job,
	int timeout, const std::map<string, int>* args /* = NULL */)
{
	return addjob(name, job.c_str(), job.length(), timeout, args);
}

const char* disque::addjob(const char* name, const void* job, size_t job_len,
	int timeout, const std::map<string, int>* args /* = NULL */)
{
	size_t argc = 4;
	if (args != NULL && args->empty() == false)
		argc += args->size() * 2;

	const char** argv = (const char**) pool_->dbuf_alloc(argc * sizeof(char*));
	size_t* lens = (size_t*) pool_->dbuf_alloc(argc * sizeof(size_t));

	argv[0] = "ADDJOB";
	lens[0] = sizeof("ADDJOB") - 1;

	argv[1] = name;
	lens[1] = strlen(name);

	argv[2] = (const char*) job;
	lens[2] = job_len;

	char buf[INT_LEN];
	safe_snprintf(buf, sizeof(buf), "%d", timeout);
	argv[3] = buf;
	lens[3] = strlen(buf);

	size_t i = 4;

	if (args == NULL || args->empty())
	{
		build_request(i, argv, lens);
		return get_status();
	}

	std::map<string, int>::const_iterator cit = args->begin();
	for (; cit != args->end(); ++cit)
	{
		if (cit->first.compare("REPLICATE", false) == 0
			|| cit->first.compare("DELAY", false) == 0
			|| cit->first.compare("RETRY", false) == 0
			|| cit->first.compare("TTL", false) == 0
			|| cit->first.compare("MAXLEN", false) == 0)
		{
			argv[i] = cit->first.c_str();
			lens[i] = cit->first.length();
			i++;

			char* tmp = (char*) pool_->dbuf_alloc(INT_LEN);
			safe_snprintf(tmp, INT_LEN, "%d", cit->second);
			argv[i] = tmp;
			lens[i] = strlen(tmp);
			i++;
		}
		else if (cit->first.compare("ASYNC", false) == 0
			&& cit->second != 0)
		{
			argv[i] = cit->first.c_str();
			lens[i] = cit->first.length();
			i++;
		}
	}

	build_request(i, argv, lens);
	return get_status();
}

int disque::getjob(const std::vector<string>& names, std::vector<string>& out,
	size_t timeout, size_t count)
{
	size_t argc = 2 + names.size() + 4;

	const char** argv = (const char**) pool_->dbuf_alloc(argc * sizeof(char*));
	size_t* lens = (size_t*) pool_->dbuf_alloc(argc * sizeof(size_t));

	argv[0] = "GETJOB";
	lens[0] = sizeof("GETJOB") - 1;

	size_t i = 1;
	if (timeout > 0)
	{
		argv[i] = "TIMEOUT";
		lens[i] = sizeof("TIMEOUT") - 1;
		i++;

		char* tmp = (char*) pool_->dbuf_alloc(INT_LEN);
		safe_snprintf(tmp, INT_LEN, "%d", timeout);
		argv[i] = tmp;
		lens[i] = strlen(tmp);
		i++;
	}

	if (count > 0)
	{
		argv[i] = "COUNT";
		lens[i] = sizeof("COUNT") - 1;
		i++;

		char* tmp = (char*) pool_->dbuf_alloc(INT_LEN);
		safe_snprintf(tmp, INT_LEN, "%d", count);
		argv[i] = tmp;
		lens[i] = strlen(tmp);
		i++;
	}

	argv[i] = "FROM";
	lens[i] = sizeof("FROM") - 1;
	i++;

	for (std::vector<string>::const_iterator cit = names.begin();
		cit != names.end(); ++cit)
	{
		argv[i] = (*cit).c_str();
		lens[i] = (*cit).length();
		i++;
	}

	build_request(i, argv, lens);
	return get_strings(out);
}

int disque::qlen(const char* name)
{
	size_t argc = 2;
	const char* argv[2];
	size_t lens[2];

	argv[0] = "QLEN";
	lens[0] = sizeof("QLEN") - 1;

	argv[1] = name;
	lens[1] = strlen(name);

	build_request(argc, argv, lens);
	return get_number();
}

int disque::qpeek(const char* name, int count, std::vector<string>& out)
{
	size_t argc = 3;
	const char* argv[3];
	size_t lens[3];

	argv[0] = "QPEEK";
	lens[0] = sizeof("QPEEK") - 1;

	argv[1] = name;
	lens[1] = strlen(name);

	char tmp[INT_LEN];
	safe_snprintf(tmp, sizeof(tmp), "%d", count);
	argv[2] = tmp;
	lens[2] = strlen(tmp);

	build_request(argc, argv, lens);
	return get_strings(out);
}

bool disque::show(const char* job_id, std::map<string, string>& out)
{
	size_t argc = 2;
	const char* argv[2];
	size_t lens[2];

	argv[0] = "SHOW";
	lens[0] = sizeof("SHOW") - 1;

	argv[1] = job_id;
	lens[1] = strlen(job_id);

	build_request(argc, argv, lens);
	const redis_result* result = run();
	if (result == NULL)
		return false;

	return true;
}

int disque::ackjob(const std::vector<string>& jobs)
{
	return true;
}

int disque::fastack(const std::vector<string>& jobs)
{
	return true;
}

bool disque::info(std::map<string, string>& out)
{
	return true;
}

bool disque::hello(std::map<string, string>& result)
{
	return true;
}

int disque::enqueue(const std::vector<string>& jobs)
{
	return true;
}

int disque::dequeue(const std::vector<string>& jobs)
{
	return true;
}

int disque::deljob(const std::vector<string>& jobs)
{
	return true;
}

} // namespace acl
