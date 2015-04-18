#pragma once
#include <vector>
#include <map>

class redis_builder
{
public:
	redis_builder(const char* conf, int meet_wait = 100);
	~redis_builder(void);

	bool build();

private:
	acl::string conf_;
	int meet_wait_;
	std::vector<acl::redis_node*> masters_;

	bool load();
	acl::redis_node* create_master(acl::xml_node& node);
	acl::redis_node* create_slave(acl::xml_node& node);
	bool build_cluster();
	bool build_master(acl::redis_node& master);
	bool cluster_meet(acl::redis& redis, const acl::redis_node& master);
	bool cluster_meeting(acl::redis& redis, const char* master_id);
	const char* myself_id(acl::redis& redis);
	bool master_set_slots(acl::redis& redis, acl::redis_node& master);
	bool add_slave(const acl::redis_node& master,
		const acl::redis_node& slave);
	acl::redis_node* find_slave(const acl::redis_node* node,
		const char* addr, size_t& nslaves);

	bool show_nodes(const std::map<acl::string, acl::redis_node*>* masters);
	void show_master_slots(const acl::redis_node* master);
	void show_slave_nodes(const std::vector<acl::redis_node*>& slaves);
};
