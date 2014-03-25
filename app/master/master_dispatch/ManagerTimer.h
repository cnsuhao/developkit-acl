#pragma once

class ClientConnection;

class ManagerTimer : public acl::event_timer
{
public:
	ManagerTimer() {}

	virtual void destroy();

	static bool transfer(ClientConnection* client);

protected:
	// 基类虚函数
	virtual void timer_callback(unsigned int id);

private:
	~ManagerTimer() {}
};
