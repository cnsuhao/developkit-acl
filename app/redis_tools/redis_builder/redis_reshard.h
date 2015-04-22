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

	void copy_slots(acl::redis_node& from, acl::redis_node& to);
};
