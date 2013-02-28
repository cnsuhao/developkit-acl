#pragma once

//////////////////////////////////////////////////////////////////////////

class upload_callback
{
public:
	upload_callback() {}
	virtual ~upload_callback() {}

	virtual void upload_report() = 0;
protected:
private:
};

//////////////////////////////////////////////////////////////////////////

class upload : public acl::rpc_request
{
public:
	upload(upload_callback* callback, const char* pingDb,
		const char* dnsDb, const char* smtpAddr,
		int connectTimeout, int rwTimeout);
protected:
	~upload();

	// �����麯�������̴߳�����
	virtual void rpc_run();

	// �����麯�������̴߳�����̣��յ����߳�������ɵ���Ϣ
	virtual void rpc_onover();

	// �����麯�������̴߳�����̣��յ����̵߳�֪ͨ��Ϣ
	virtual void rpc_wakeup(void* ctx);
private:
	upload_callback* callback_;
};
//////////////////////////////////////////////////////////////////////////
