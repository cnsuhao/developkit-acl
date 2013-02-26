#pragma once

//////////////////////////////////////////////////////////////////////////

// �����࣬������ʵ�ָ����еĴ���ӿ�
class ping_callback
{
public:
	ping_callback() {}
	virtual ~ping_callback() {}

	virtual void enable_ping() = 0;
};

//////////////////////////////////////////////////////////////////////////

class ping : public acl::rpc_request
{
public:
	ping(const char* filepath, ping_callback* callback,
		int npkt, int delay, int timeout);
protected:
	~ping() {}

	// �����麯�������̴߳�����
	virtual void rpc_run();

	// �����麯�������̴߳�����̣��յ����߳�������ɵ���Ϣ
	virtual void rpc_onover();

	// �����麯�������̴߳�����̣��յ����̵߳�֪ͨ��Ϣ
	virtual void rpc_wakeup(void* ctx);
private:
	acl::string filepath_;
	ping_callback* callback_;
	int   npkt_;
	int   delay_;
	int   timeout_;
	std::list<acl::string> ip_list_;

	bool load_file();
	void ping_all();
};
