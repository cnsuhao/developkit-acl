#pragma once
#include <vector>

// �û������б��࣬�����ж������û��Ƿ�����ʹ�ñ�����

class access_list : public acl::singleton<access_list>
{
public:
	access_list();
	~access_list();

	void init_users(const char* whitelist);
	void add_user(const char* user);
	bool check_user(const char* user);

	void set_allow_clients(const char* iplist);
	void set_allow_servers(const char* iplist);

	bool check_client(const char* ip);
	bool check_server(const char* ip);

private:
	bool allow_all_;
	std::vector<acl::string> white_list_;
	ACL_IPLINK* allow_clients_;
	ACL_IPLINK* allow_servers_;
};
