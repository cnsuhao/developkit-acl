#include "stdafx.h"

static acl::string __queue("greeting");
static acl::string __jobpre("test_job");

static bool test_addjob(acl::disque& cmd, int n)
{
	acl::string job;
	int timeout = 0;
	const char* jobid;

	for (int i = 0; i < n; i++)
	{
		job.format("job_%s_%d", __jobpre.c_str(), i);
		cmd.clear();
		jobid = cmd.addjob(__queue, job, timeout);
		if (jobid == NULL)
		{
			printf("addjob queue: %s error: %s\r\n",
				__queue.c_str(), cmd.result_error());
			return false;
		}
		else if (i < 10)
			printf("addjob queue: %s ok, jobid: %s\r\n",
				__queue.c_str(), jobid);
	}

	return true;
}

static bool test_getjob(acl::disque& cmd, int n)
{
	std::vector<acl::string> queues;
	std::vector<acl::string> jobs;
	size_t timeout = 10, count = 100;

	queues.push_back(__queue);

	for (int i = 0; i < n; i++)
	{
		cmd.clear();
		int n = cmd.getjob(queues, jobs, timeout, count);
		if (n < 0)
		{
			printf("getjob queue: %s error: %s\r\n",
				__queue.c_str(), cmd.result_error());
			return false;
		}
		else if (i < 10)
		{
			printf("getjob queue: %s ok\r\n", __queue.c_str());
			for (int j = 0; j < n; j++)
				printf("job(%d): %s\r\n", j, jobs[j].c_str());
		}
		jobs.clear();
	}

	return true;
}

static bool test_qlen(acl::disque& cmd, int n)
{
	for (int i = 0; i < n; i++)
	{
		cmd.clear();
		int ret = cmd.qlen(__queue.c_str());
		if (ret < 0)
		{
			printf("qlen queue: %s error: %s\r\n",
				__queue.c_str(), cmd.result_error());
			return false;
		}
		else if (i < 10)
			printf("qlen: %d, queue: %s\r\n",
				ret, __queue.c_str());
	}

	return true;
}

static bool test_qpeek(acl::disque& cmd, int n)
{
	int count = 100;
	std::vector<acl::string> jobs;

	for (int i = 0; i < n; i++)
	{
		cmd.clear();
		int n = cmd.qpeek(__queue.c_str(), count, jobs);
		if (n < 0)
		{
			printf("qpeek queue: %s error: %s\r\n",
				__queue.c_str(), cmd.result_error());
			return false;
		}
		else if (i < 10)
		{
			printf("qpeek queue: %s ok\r\n", __queue.c_str());
			for (int j = 0; j < n; j++)
				printf("job(%d): %s\r\n", j, jobs[j].c_str());
		}
		jobs.clear();
	}

	return true;
}

static bool test_show(acl::disque& cmd, int n)
{
	acl::string jobid("dddd");
	acl::string key;

	for (int i = 0; i < n; i++)
	{
		key.format("%s_%d", __jobpre.c_str(), i);
		cmd.clear();
		const acl::disque_job* job = cmd.show(jobid.c_str());
		if (job == NULL)
		{
			printf("show jobid: %s error: %s\r\n", jobid.c_str(),
				cmd.result_error());
			return false;
		}
		else if (i < 10)
			printf("show ok, jobid: %s\r\n", jobid.c_str());
	}

	return true;
}

static bool test_ackjob(acl::disque& cmd, int n)
{
	std::vector<acl::string> job_ids;

	for (int i = 0; i < n; i++)
	{
		cmd.clear();
		int ret = cmd.ackjob(job_ids);
		if (ret < 0)
			printf("ackjob error: %s\r\n", cmd.result_error());
		else if (i < 10)
			printf("ackjob ok\r\n");
	}

	return true;
}

static bool test_enqueue(acl::disque& cmd, int n)
{
	std::vector<acl::string> job_ids;

	for (int i = 0; i < n; i++)
	{
		cmd.clear();
		int ret = cmd.enqueue(job_ids);
		if (ret < 0)
			printf("enqueue error: %s\r\n", cmd.result_error());
		else if (i < 10)
			printf("enqueue ok\r\n");
	}

	return true;
}

static void usage(const char* procname)
{
	printf("usage: %s -h[help]\r\n"
		"-s disque_addr[127.0.0.1:7711]\r\n"
		"-n count\r\n"
		"-C connect_timeout[default: 10]\r\n"
		"-T rw_timeout[default: 10]\r\n"
		"-a cmd[addjob|getjob|qlen|qpeek|show|ackjob|fastack|enqueue|dequeue|deljob|info|hello|all]\r\n",
		procname);
}

int main(int argc, char* argv[])
{
	int  ch, n = 1, conn_timeout = 10, rw_timeout = 10;
	acl::string addr("127.0.0.1:7711"), command;

	while ((ch = getopt(argc, argv, "hs:n:C:T:a:")) > 0)
	{
		switch (ch)
		{
		case 'h':
			usage(argv[0]);
			return 0;
		case 's':
			addr = optarg;
			break;
		case 'n':
			n = atoi(optarg);
			break;
		case 'C':
			conn_timeout = atoi(optarg);
			break;
		case 'T':
			rw_timeout = atoi(optarg);
			break;
		case 'a':
			command = optarg;
			break;
		default:
			break;
		}
	}

	acl::acl_cpp_init();
	acl::log::stdout_open(true);
	acl::redis_client client(addr.c_str(), conn_timeout, rw_timeout);
	acl::disque cmd(&client);

	bool ret;

	if (command == "addjob")
		ret = test_addjob(cmd, n);
	else if (command == "getjob")
		ret = test_getjob(cmd, n);
	else if (command == "qlen")
		ret = test_qlen(cmd, n);
	else if (command == "qpeek")
		ret = test_qpeek(cmd, n);
	else if (command == "show")
		ret = test_show(cmd, n);
	else if (command == "ackjob")
		ret = test_ackjob(cmd, n);
	else if (command == "enqueue")
		ret = test_enqueue(cmd, n);
	else if (command == "dequeue")
		;
	else if (command == "deljob")
		;
	else if (command == "info")
		;
	else if (command == "hello")
		;
	else if (command == "all")
	{
		ret = test_addjob(cmd, n)
			&& test_getjob(cmd, n)
			&& test_qlen(cmd, n)
			&& test_qpeek(cmd, n)
			&& test_show(cmd, n)
			&& test_ackjob(cmd, n)
			&& test_enqueue(cmd, n);
	}
	else
	{
		ret = false;
		printf("unknown cmd: %s\r\n", command.c_str());
	}

	if (ret == true)
		printf("test OK!\r\n");
	else
		printf("test failed!\r\n");

#ifdef WIN32
	printf("enter any key to exit\r\n");
	getchar();
#endif
	return 0;
}
