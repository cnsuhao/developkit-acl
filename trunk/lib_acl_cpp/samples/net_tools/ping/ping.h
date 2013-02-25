#pragma once

//////////////////////////////////////////////////////////////////////////

// �����࣬������ʵ�ָ����еĴ���ӿ�
class rpc_callback
{
public:
	rpc_callback() {}
	virtual ~rpc_callback() {}

	virtual void enable_ping() = 0;
};

//////////////////////////////////////////////////////////////////////////

class ping : public acl::rpc_request
{
public:
	ping(const char* filepath, rpc_callback* callback,
		int npkt = 10, int delay = 1, int timeout = 5);
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
	rpc_callback* callback_;
	int   npkt_;
	int   delay_;
	int   timeout_;
	std::list<acl::string> ip_list_;

	bool load_file();
	void ping_all();
};
