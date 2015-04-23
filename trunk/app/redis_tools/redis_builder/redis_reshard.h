#pragma once

class redis_reshard
{
public:
	redis_reshard(const char* addr);
	~redis_reshard();

	void run();

private:
	acl::string addr_;
	std::vector<acl::redis_node*> masters_;

	acl::redis_node* find_node(const char* id);
	bool get_masters_info();
	void copy_all(std::vector<acl::redis_node*>& src, const char* exclude);
	void show_nodes();
	void show_slots(const acl::redis_node& node);
	void copy_slots(acl::redis_node& from, acl::redis_node& to);
	void move_slots(std::vector<acl::redis_node*>& from,
		acl::redis_node& to, int nslots);
	int move_slots(acl::redis_node& from, acl::redis_node& to, int count);
	bool move_slot(size_t slot, acl::redis& from, const char* from_id,
		acl::redis& to, const char* to_id);
};
