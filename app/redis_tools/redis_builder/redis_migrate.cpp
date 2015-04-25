#include "stdafx.h"
#include "redis_util.h"
#include "redis_migrate.h"

redis_migrate::redis_migrate(std::vector<acl::redis_node*>& masters)
	: masters_(masters)
{
}

redis_migrate::~redis_migrate(void)
{
}

void redis_migrate::move_slots(std::vector<acl::redis_node*>& froms,
	acl::redis_node& to, int nslots)
{
	assert(!froms.empty());

	int base = nslots / (int) froms.size(), nslots_moved = 0;
	std::vector<acl::redis_node*>::iterator it;
	for (it = froms.begin(); it != froms.end(); ++it)
	{
		// move some slots from one source node to the target node
		int ret = move_slots(**it, to, base);
		if (ret < 0)
		{
			printf("move failed, stop!\r\n");
			break;
		}

		nslots_moved += ret;

		// check if all the slots have been moved
		if (nslots_moved >= nslots)
		{
			printf("moved %d slots ok\r\n");
			break;
		}
	}

	printf("move over!\r\n");
}

int redis_migrate::move_slots(acl::redis_node& from,
	acl::redis_node& to, int count)
{
	acl::redis_client from_conn(from.get_addr());
	acl::redis from_redis(&from_conn);
	acl::redis_client to_conn(to.get_addr());
	acl::redis to_redis(&to_conn);

	acl::string from_id, to_id;
	if (redis_util::get_node_id(from_redis, from_id) == false)
	{
		printf("can't get source node id, addr: %s\r\n",
			from.get_addr());
		return -1;
	}
	if (redis_util::get_node_id(to_redis, to_id) == false)
	{
		printf("can't get target node id, addr: %s\r\n",
			to.get_addr());
		return -1;
	}

	// get all the specified source node's slots
	const std::vector<std::pair<size_t, size_t> >& slots = from.get_slots();
	std::vector<std::pair<size_t, size_t> >::const_iterator cit;
	int nslots_moved = 0;

	// iterate the source node's slots, and move them to the target
	for (cit = slots.begin(); cit != slots.end(); ++cit)
	{
		size_t min = cit->first;
		size_t max = cit->second;
		for (size_t slot = min; slot < max; slot++)
		{
			// move the specified slot from source to target
			if (move_slot(slot, from_redis, from_id,
				to_redis, to_id) == false)
			{
				printf("move slots error, slot: %d\r\n",
					(int) slot);
				return -1;
			}
			nslots_moved++;

			// if the specified number slots have been moved ?
			if (nslots_moved >= count)
				return nslots_moved;
		}
	}
	
	return nslots_moved;
}

bool redis_migrate::move_slot(size_t slot, acl::redis& from,
	const char* from_id, acl::redis& to, const char* to_id)
{
	// set the slot in migrating status for the source node
	if (from.cluster_setslot_migrating(slot, to_id) == false)
		return false;
	// set the slot in importing status for the target node
	if (to.cluster_setslot_importing(slot, from_id) == false)
		return false;

	const char* from_addr = from.get_client()->get_stream()->get_peer(true);
	const char* to_addr = to.get_client()->get_stream()->get_peer(true);

	// the number of keys to be moved in each moving
	size_t max = 10;
	std::list<acl::string> keys;

	std::list<acl::string>::const_iterator cit;

	while (true)
	{
		keys.clear();
		int nkeys = from.cluster_getkeysinslot(slot, max, keys);
		if (nkeys == 0)
			break;
		if (nkeys < 0)
		{
			printf("cluster_getkeysinslot error: %s\r\n",
				from.result_error());
			return false;
		}

		// move all the keys stored by the specifed key
		for (cit = keys.begin(); cit != keys.end(); ++cit)
		{
			if (move_key((*cit).c_str(), from, to_addr) == false)
			{
				printf("move key: %s error: %s, from: %s"
					", to: %s\r\n", (*cit).c_str(),
					from_addr, to_addr);
				return false;
			}
		}
	}

	return notify_cluster(slot, to_id);
}

bool redis_migrate::notify_cluster(size_t slot, const char* id)
{
	acl::redis redis;
	std::vector<acl::redis_node*>::const_iterator cit;

	for (cit = masters_.begin(); cit != masters_.end(); ++cit)
	{
		acl::redis_client client((*cit)->get_addr());
		redis.set_client(&client);
		redis.clear();

		if (redis.cluster_setslot_node(slot, id) == false)
		{
			printf("cluster_setslot_node error: %s, slot: %d, "
				"addr: %s\r\n", redis.result_error(),
				(int) slot, (*cit)->get_addr());
			return false;
		}
	}
	
	printf("slot: %d, moved to %s ok\r\n", (int) slot, id);
	return true;
}

bool redis_migrate::move_key(const char* key, acl::redis& from,
	const char* to_addr)
{
	bool ret = from.migrate(key, to_addr, 0, 15000);
	if (ret == true)
		return true;
	const char* from_addr = from.get_client()->get_stream()->get_peer(true);
	const char* error = from.result_error();
	if (strcasestr((char*) error, "BUSYKEY") == false)
	{
		printf("move key: %s error: %s, from: %s, to: %s\r\n",
			key, error, from_addr, to_addr);
		return false;
	}

	printf("*** Target key: %s exists, Replace it fix? yes/no: ", key);
	char buf[256];
	int n = acl_vstream_gets_nonl(ACL_VSTREAM_IN, buf, sizeof(buf));
	if (n == ACL_VSTREAM_EOF)
	{
		printf("Input error, key: %s\r\n", key);
		return false;
	}

#define NQ(x, y) strcasecmp((x), (y))

	if (NQ(buf, "yes") && NQ(buf, "y") && NQ(buf, "true"))
	{
		printf("No replace key: %s in target: %s\r\n", key, to_addr);
		return false;
	}

	ret = from.migrate(key, to_addr, 0, 15000, "REPLACE");
	if (ret == true)
		return true;

	printf("move key: %s error: %s, from: %s, to: %s\r\n",
		key, from.result_error(), from_addr, to_addr);
	return false;
}
