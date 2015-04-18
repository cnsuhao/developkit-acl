#include "stdafx.h"
#include "redis_builder.h"

//////////////////////////////////////////////////////////////////////////

static void show_slave_nodes(const std::vector<acl::redis_node*>& slaves)
{
	std::vector<acl::redis_node*>::const_iterator cit;
	for (cit = slaves.begin(); cit != slaves.end(); ++cit)
	{
		printf("slave, id: %s, addr: %s, master_id: %s\r\n",
			(*cit)->get_id(), (*cit)->get_addr(),
			(*cit)->get_master_id());
	}
}

static void show_master_slots(const acl::redis_node* master)
{
	const std::vector<std::pair<size_t, size_t> >& slots =
		master->get_slots();

	std::vector<std::pair<size_t, size_t> >::const_iterator cit;
	for (cit = slots.begin(); cit != slots.end(); ++cit)
		printf("slots range: %d-%d\r\n",
			(int) (*cit).first, (int) (*cit).second);
}

static bool show_nodes(acl::redis& redis)
{
	const std::map<acl::string, acl::redis_node*>* masters;
	if ((masters = redis.cluster_nodes())== NULL)
	{
		printf("can't get cluster nodes\r\n");
		return false;
	}

	const std::vector<acl::redis_node*>* slaves;
	std::map<acl::string, acl::redis_node*>::const_iterator cit;
	for (cit = masters->begin(); cit != masters->end(); ++cit)
	{
		printf("==========================================\r\n");
		printf("master, id: %s, addr: %s\r\n",
			cit->first.c_str(), cit->second->get_addr());
		show_master_slots(cit->second);
		slaves = cit->second->get_slaves();
		show_slave_nodes(*slaves);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

static void show_slaves_slots(const acl::redis_slot* slot)
{
	const std::vector<acl::redis_slot*>& slaves = slot->get_slaves();
	std::vector<acl::redis_slot*>::const_iterator cit;
	for (cit = slaves.begin(); cit != slaves.end(); ++cit)
	{
		printf("slave: ip: %s, port: %d, slots: %d - %d\r\n",
			(*cit)->get_ip(), (*cit)->get_port(),
			(int) (*cit)->get_slot_min(),
			(int) (*cit)->get_slot_max());
	}
}

static bool show_slots(acl::redis& redis)
{
	const std::vector<acl::redis_slot*>* slots = redis.cluster_slots();
	if (slots == NULL)
		return false;

	std::vector<acl::redis_slot*>::const_iterator cit;

	for (cit = slots->begin(); cit != slots->end(); ++cit)
	{
		printf("=========================================\r\n");
		printf("master: ip: %s, port: %d, slots: %d - %d\r\n",
			(*cit)->get_ip(), (*cit)->get_port(),
			(int) (*cit)->get_slot_min(),
			(int) (*cit)->get_slot_max());
		show_slaves_slots(*cit);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

static void usage(const char* procname)
{
	printf("usage: %s -h[help]\r\n"
		"-s redis_addr[ip:port]\r\n"
		"-a cmd[nodes|slots|build]\r\n"
		"-f configure_file\r\n",
		procname);
}

int main(int argc, char* argv[])
{
	// ³õÊ¼»¯ acl ¿â
	acl::acl_cpp_init();

	int  ch;
	acl::string addr, cmd, conf;

	while ((ch = getopt(argc, argv, "hs:a:f:")) > 0)
	{
		switch (ch)
		{
		case 'h':
			usage(argv[0]);
			return 0;
		case 's':
			addr = optarg;
			break;
		case 'a':
			cmd = optarg;
			break;
		case 'f':
			conf = optarg;
			break;
		default:
			break;
		}
	}

	if (addr.empty())
	{
		printf("redis_addr empty\r\n");
		usage(argv[0]);
		return 0;
	}

	int conn_timeout = 10, rw_timeout = 120;
	acl::redis_client client(addr, conn_timeout, rw_timeout);
	acl::redis redis(&client);

	if (cmd == "nodes")
		return show_nodes(redis);
	else if (cmd == "slots")
		return show_slots(redis);
	else if (cmd == "build")
	{
		if (conf.empty())
		{
			printf("usage: %s -a build -f nodes.cf\r\n", argv[0]);
			goto END;
		}
		redis_builder builder(conf.c_str());
		builder.build();
	}
	else
		printf("unknown cmd: %s\r\n", cmd.c_str());

END:

#ifdef WIN32
	printf("enter any key to exit\r\n");
	getchar();
#endif
	return 0;
}
