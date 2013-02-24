#pragma once

//////////////////////////////////////////////////////////////////////////

// 纯虚类，子类须实现该类中的纯虚接口
class rpc_callback
{
public:
	rpc_callback() {}
	virtual ~rpc_callback() {}

	// 设置 HTTP 请求头数据虚函数
	virtual void SetRequestHdr(const char* hdr) = 0;
	// 设置 HTTP 响应头数据虚函数
	virtual void SetResponseHdr(const char* hdr) = 0;
	// 下载过程中的回调函数虚函数
	virtual void OnDownloading(long long int content_length,
		long long int total_read) = 0;
	// 下载完成时的回调函数虚函数
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

	// 子线程处理函数
	virtual void rpc_run();

	// 主线程处理过程，收到子线程任务完成的消息
	virtual void rpc_onover();

	// 主线程处理过程，收到子线程的通知消息
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
