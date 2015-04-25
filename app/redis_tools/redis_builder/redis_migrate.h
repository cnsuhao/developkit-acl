#pragma once
#include <vector>

class redis_migrate
{
public:
	redis_migrate(std::vector<acl::redis_node*>& masters);
	~redis_migrate(void);

	void move_slots(std::vector<acl::redis_node*>& from,
		acl::redis_node& to, int nslots);
	int move_slots(acl::redis_node& from, acl::redis_node& to, int count);
	bool move_slot(size_t slot, acl::redis& from, const char* from_id,
		acl::redis& to, const char* to_id);
	bool notify_cluster(size_t slot, const char* id);
	bool move_key(const char* key, acl::redis& from, const char* to_addr);

private:
	std::vector<acl::redis_node*>& masters_;
};

