#include "StdAfx.h"
#include "dns_store.h"
#include "rpc/rpc_manager.h"
#include "nslookup.h"

//////////////////////////////////////////////////////////////////////////

domain_info::domain_info(nslookup& ns, const char* domain)
: ns_(ns)
, begin_(0)
, end_(0)
{
	ACL_SAFE_STRNCPY(domain_, domain, sizeof(domain_));
}

domain_info::~domain_info()
{
	std::vector<IP_INFO*>::iterator it = ip_list_.begin();
	for (; it != ip_list_.end(); ++it)
		acl_myfree(*it);
}

void domain_info::add_ip(const char* ip, int ttl)
{
	IP_INFO* info = (IP_INFO*) acl_mycalloc(1, sizeof(IP_INFO));
	ACL_SAFE_STRNCPY(info->ip, ip, sizeof(info->ip));
	info->ttl = ttl;
	ip_list_.push_back(info);
}

void domain_info::set_begin()
{
	time(&begin_);
}

void domain_info::set_end()
{
	time(&end_);
}

//////////////////////////////////////////////////////////////////////////

nslookup::nslookup(const char* filepath, nslookup_callback* callback,
	const char* dns_ip, int dns_port, int timeout)
	: filepath_(filepath)
	, callback_(callback)
	, dns_ip_(dns_ip)
	, dns_port_(dns_port)
	, timeout_(timeout)
	, nresult_(0)
	, domain_list_(NULL)
{
}

nslookup::~nslookup()
{
	if (domain_list_)
		delete domain_list_;
}

//////////////////////////////////////////////////////////////////////////
// 主线程运行

void nslookup::rpc_onover()
{
	callback_->enable_nslookup();
	dns_store* ds = new dns_store(domain_list_);
	domain_list_ = NULL;
	rpc_manager::get_instance().fork(ds);
	delete this;
}

void nslookup::rpc_wakeup(void* ctx)
{

}

//////////////////////////////////////////////////////////////////////////
// 子线程运行

void nslookup::rpc_run()
{
	if (load_file() == true)
		lookup_all();
}

bool nslookup::load_file()
{
	acl::ifstream in;
	if (in.open_read(filepath_) == false)
	{
		logger_error("open file(%s) failed: %s",
			filepath_.c_str(), acl::last_serror());
		return false;
	}

	domain_list_ = new std::vector<domain_info*>;
	acl::string line;
	while (in.eof() == false)
	{
		if (in.gets(line) == false)
			break;
		domain_info* di = new domain_info(*this, line.c_str());
		domain_list_->push_back(di);
	}

	if (domain_list_->empty())
	{
		logger_error("no ip in file %s", filepath_.c_str());
		return false;
	}
	return true;
}

void nslookup::lookup_all()
{
	if (domain_list_ == NULL || domain_list_->empty())
	{
		logger_error("domain empty");
		return;
	}

	ACL_AIO *aio;
	/* 创建非阻塞异步通信句柄 */
	aio = acl_aio_create(ACL_EVENT_SELECT);
//	acl_aio_set_keep_read(aio, 0);

	// 创建 DNS 查询句柄
	ACL_DNS* dns = acl_dns_create(aio, timeout_);
	acl_dns_add_dns(dns, dns_ip_.c_str(), dns_port_, 24);

	// 添加目标 domain 地址
	std::vector<domain_info*>::iterator it = domain_list_->begin();
	for (; it != domain_list_->end(); ++it)
	{
		(*it)->set_begin();
		acl_dns_lookup(dns, (*it)->get_domain(), dns_result, *it);
	}

	while (1) {
		/* 异步事件循环过程 */
		acl_aio_loop(aio);

		// 如果所有查询过程都完成，则退出异步事件循环过程
		if (nresult_ >= domain_list_->size())
		{
			logger("DNS lookup over: %d, %d",
				(int) domain_list_->size(), (int) nresult_);
			break;
		}
	}

	/* 显示域名查询结果 */

	acl_dns_close(dns);

	/* 销毁非阻塞句柄 */
	acl_aio_free(aio);
}

void nslookup::dns_result(ACL_DNS_DB *dns_db, void *ctx, int errnum)
{
	domain_info* info = (domain_info*) ctx;

	info->set_end();
	if (dns_db == NULL) {
		logger("ERROR: %s, domain: %s, spent: %ld",
			acl_dns_serror(errnum), info->get_domain(),
			info->end_time() - info->begin_time());
		info->add_ip("0.0.0.0", 0);
		info->get_nslookup().nresult_++;
		return;
	}

	ACL_ITER iter;
	acl::string buf;
	buf.format("OK, domain: %s, spent: %ld, ip_list: ",
		info->get_domain(), info->end_time() - info->begin_time());

	// 遍历该域名的所有查询结果
	const ACL_HOST_INFO *hi;
	acl_foreach(iter, dns_db) {

		hi = (const ACL_HOST_INFO*) iter.data;
		if (iter.i > 0)
			buf << ", ";
		buf.format_append("ip=%s, ttl=%d", hi->ip, hi->ttl);
		info->add_ip(hi->ip, hi->ttl);
	}

	logger("%s", buf.c_str());
	info->get_nslookup().nresult_++;
}
