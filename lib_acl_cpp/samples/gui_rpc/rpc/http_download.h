#pragma once

//////////////////////////////////////////////////////////////////////////

// �����࣬������ʵ�ָ����еĴ���ӿ�
class rpc_callback
{
public:
	rpc_callback() {}
	virtual ~rpc_callback() {}

	// ���� HTTP ����ͷ�����麯��
	virtual void SetRequestHdr(const char* hdr) = 0;
	// ���� HTTP ��Ӧͷ�����麯��
	virtual void SetResponseHdr(const char* hdr) = 0;
	// ���ع����еĻص������麯��
	virtual void OnDownloading(long long int content_length,
		long long int total_read) = 0;
	// �������ʱ�Ļص������麯��
	virtual void OnDownloadOver(long long int total_read,
		double spent) = 0;
};

//////////////////////////////////////////////////////////////////////////

class http_download : public acl::rpc_request
{
public:
	http_download(const char* addr, const char* url,
		rpc_callback* callback)
		: addr_(addr)
		, url_(url)
		, callback_(callback)
		, error_(false)
		, total_read_(0)
		, content_length_(0)
		, total_spent_(0)
	{}
protected:
	~http_download() {}

	// ���̴߳�����
	virtual void rpc_run();

	// ���̴߳�����̣��յ����߳�������ɵ���Ϣ
	virtual void rpc_onover();

	// ���̴߳�����̣��յ����̵߳�֪ͨ��Ϣ
	virtual void rpc_wakeup(void* ctx);
private:
	acl::string addr_;
	acl::string url_;
	acl::string req_hdr_;
	acl::string res_hdr_;
	bool  error_;
	long long int total_read_;
	long long int content_length_;
	double total_spent_;

	rpc_callback* callback_;
};
