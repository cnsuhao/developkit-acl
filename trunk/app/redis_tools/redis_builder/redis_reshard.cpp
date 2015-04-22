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

void redis_reshard::run()
{
	acl::redis_client client(addr_, 30, 30);
	acl::redis redis(&client);

	const std::map<acl::string, acl::redis_node*>* masters;
	if ((masters = redis.cluster_nodes()) == NULL)
	{
		printf("%s: master nodes empty\r\n", __FUNCTION__);
		return;
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
}
