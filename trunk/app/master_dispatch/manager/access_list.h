#pragma once
#include <vector>

// �û������б��࣬�����ж������û��Ƿ�����ʹ�ñ�����

class access_list : public acl::singleton<access_list>
{
public:
	access_list();
	~access_list() {}

	void init(const char* whitelist);
	void add_one(const char* user);
	bool check(const char* user);

private:
	bool allow_all_;
	std::vector<acl::string> white_list_;
};
