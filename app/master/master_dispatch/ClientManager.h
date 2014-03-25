#pragma once
#include <vector>

class ClientConnection;

class ClientManager : public acl::singleton<ClientManager>
{
public:
	ClientManager() {}
	~ClientManager() {}

	void set(ClientConnection* conn);
	void del(ClientConnection* conn);
	ClientConnection* pop();

private:
	std::vector<ClientConnection*> conns_;
};
