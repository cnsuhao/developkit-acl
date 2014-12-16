#pragma once
#include <vector>

// 用户访问列表类，用来判断所给用户是否允许使用本服务

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
