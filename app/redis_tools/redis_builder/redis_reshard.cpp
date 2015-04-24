#include "stdafx.h"
#include "redis_util.h"
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

void redis_reshard::copy_all(std::vector<acl::redis_node*>& src,
	const char* exclude)
{
	std::vector<acl::redis_node*>::iterator it;
	for (it = masters_.begin(); it != masters_.end(); ++it)
	{
		if (strcmp((*it)->get_id(), exclude) != 0)
			src.push_back(*it);
	}
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

	acl::redis_node* target = NULL;
	while (true)
	{
		printf("What is the receiving node ID?");
		fflush(stdout);
		int ret = acl_vstream_gets_nonl(ACL_VSTREAM_IN, buf, sizeof(buf));
		if (ret == ACL_VSTREAM_EOF)
			exit(1);
		target = find_node(buf);
		if (target != NULL)
			break;
		printf("...The specified node(%s) is not known or not "
			"a master, please try again.\r\n", buf);
	}
	assert(target != NULL);

	printf("Please input all the source node IDs.\r\n");
	printf("  Type 'all' to use all the nodes as source nodes for the hash slots\r\n");
	printf("  Type 'done' once you entered all the source node IDs.\r\n");

	std::vector<acl::redis_node*> sources;
	while (true)
	{
		printf("Source node #%d:", (int) sources.size() + 1);
		fflush(stdout);
		int ret = acl_vstream_gets_nonl(ACL_VSTREAM_IN, buf, sizeof(buf));
		if (ret == ACL_VSTREAM_EOF)
			exit(1);
		if (strcasecmp(buf, "done") == 0)
			break;
		if (strcasecmp(buf, "all") == 0)
		{
			copy_all(sources, target->get_id());
			break;
		}
		acl::redis_node* source = find_node(buf);
		if (source == NULL)
		{
			printf("...The source node(%s) is not known\r\n", buf);
			continue;
		}
		if (strcmp(target->get_id(), buf) == 0)
		{
			printf("... It is not possible to use the target node as source node\r\n");
			continue;
		}
		
		sources.push_back(source);
	}
	if (sources.empty())
	{
		printf("*** No source nodes given, operation aborted\r\n");
		exit(1);
	}

	move_slots(sources, *target, nslots);
}

void redis_reshard::move_slots(std::vector<acl::redis_node*>& from,
	acl::redis_node& to, int nslots)
{
	assert(from.empty() == true);

	int base = nslots / (int) from.size();
	std::vector<acl::redis_node*>::iterator it;
	for (it = from.begin(); it != from.end(); ++it)
	{
		if (move_slots(**it, to, base) == false)
		{
			printf("move failed, stop!\r\n");
			break;
		}
	}

	printf("move over!\r\n");
}

int redis_reshard::move_slots(acl::redis_node& from,
	acl::redis_node& to, int count)
{
	const std::vector<std::pair<size_t, size_t> >& slots = from.get_slots();
	std::vector<std::pair<size_t, size_t> >::const_iterator cit;

	acl::redis_client from_conn(from.get_addr());
	acl::redis from_redis(&from_conn);
	acl::redis_client to_conn(to.get_addr());
	acl::redis to_redis(&to_conn);
	int n = 0;

	acl::string from_id, to_id;
	if (redis_util::get_node_id(from_redis, from_id) == false)
		return -1;
	if (redis_util::get_node_id(to_redis, to_id) == false)
		return -1;

	for (cit = slots.begin(); cit != slots.end() && n < count; ++cit)
	{
		size_t min = cit->first;
		size_t max = cit->second;
		for (size_t i = min; i < max; i++)
		{
			if (move_slot(i, from_redis, from_id,
				to_redis, to_id) == false)
			{
				return -1;
			}
			n++;
			if (n >= count)
				break;
		}
		if (n >= count)
			break;
	}
	
	return n;
}

bool redis_reshard::move_slot(size_t slot, acl::redis& from,
	const char* from_id, acl::redis& to, const char* to_id)
{
	if (to.cluster_setslot_importing(slot, from_id) == false)
		return false;
	if (from.cluster_setslot_migrating(slot, to_id) == false)
		return false;

	std::vector<acl::redis_node*>::iterator it;
	for (it = masters_.begin(); it != masters_.end(); ++it)
	{
		acl::redis_client conn((*it)->get_addr());
		acl::redis redis(&conn);
		if (redis.cluster_setslot_node(slot, to_id) == false)
			return false;
	}
	
	return true;
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