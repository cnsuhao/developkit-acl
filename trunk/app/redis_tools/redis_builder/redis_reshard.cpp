#include "stdafx.h"
#include "redis_reshard.h"

redis_reshard::redis_reshard(const char* addr)
	: addr_(addr)
{
}

redis_reshard::~redis_reshard()
{
	std::vector<acl::redis_node*>::iterator it = masters_.begin();
	for (; it != masters_.end(); ++it)
		delete *it;
}

void redis_reshard::copy_slots(acl::redis_node& from, acl::redis_node& to)
{
	const std::vector<std::pair<size_t, size_t> >& slots = from.get_slots();
	std::vector<std::pair<size_t, size_t> >::const_iterator cit;
	for (cit = slots.begin(); cit != slots.end(); ++cit)
		to.add_slot_range(cit->first, cit->second);
}

acl::redis_node* redis_reshard::find_node(const char* id)
{
	std::vector<acl::redis_node*>::iterator it;
	for (it = masters_.begin(); it != masters_.end(); ++it)
	{
		if (strcmp(id, (*it)->get_id()) == 0)
			return *it;
	}
	return NULL;
}

void redis_reshard::run()
{
	if (get_masters_info() == false)
		return;

	show_nodes();
	fflush(stdout);
	char buf[1024];

	int nslots = 0;
	while (true)
	{
		printf("How many slots do you want to move (from 1 to 16384)?");
		int ret = acl_vstream_gets_nonl(ACL_VSTREAM_IN, buf, sizeof(buf));
		if (ret == ACL_VSTREAM_EOF)
			exit(1);
		nslots = atoi(buf);
		if (nslots > 0 && nslots < 16384)
			break;
		printf("invalid value: %d\r\n", ret);
	}

	while (true)
	{
		printf("What is the receiving node ID?");
		fflush(stdout);
		int ret = acl_vstream_gets_nonl(ACL_VSTREAM_IN, buf, sizeof(buf));
		if (ret == ACL_VSTREAM_EOF)
			exit(1);
		acl::redis_node* node = find_node(buf);
		if (node != NULL)
			break;
		printf("invalid node: %s\r\n", buf);
	}
}

bool redis_reshard::get_masters_info()
{
	acl::redis_client client(addr_, 30, 30);
	acl::redis redis(&client);

	const std::map<acl::string, acl::redis_node*>* masters;
	if ((masters = redis.cluster_nodes()) == NULL)
	{
		printf("%s: master nodes empty\r\n", __FUNCTION__);
		return false;
	}

	std::map<acl::string, acl::redis_node*>::const_iterator cit;
	for (cit = masters->begin(); cit != masters->end(); ++cit)
	{
		acl::redis_node* master = new acl::redis_node;
		master->set_id(cit->second->get_id());
		master->set_addr(cit->second->get_addr());
		copy_slots(*cit->second, *master);
		masters_.push_back(master);
	}

	return true;
}

void redis_reshard::show_nodes()
{
	std::vector<acl::redis_node*>::const_iterator cit;
	for (cit = masters_.begin(); cit != masters_.end(); ++cit)
	{
		printf("-----------------------------------------------\r\n");
		printf("addr: %s\r\nid: %s\r\n", (*cit)->get_addr(),
			(*cit)->get_id());
		show_slots(**cit);
	}
}

void redis_reshard::show_slots(const acl::redis_node& node)
{
	const std::vector<std::pair<size_t, size_t> > slots = node.get_slots();
	std::vector<std::pair<size_t, size_t> >::const_iterator cit;
	for (cit = slots.begin(); cit != slots.end(); ++cit)
		printf("slots: %d - %d\r\n",
			(int) cit->first, (int) cit->second);
}
