#include "StdAfx.h"
#include "ping.h"

ping::ping(const char* filepath, ping_callback* callback,
	int npkt, int delay, int timeout)
: filepath_(filepath)
, callback_(callback)
, npkt_(npkt)
, delay_(delay)
, timeout_(timeout)
{

}

//////////////////////////////////////////////////////////////////////////
// 主线程中运行

void ping::rpc_onover()
{
	callback_->enable_ping();
	delete this;
}

void ping::rpc_wakeup(void* ctx)
{

}

//////////////////////////////////////////////////////////////////////////
// 子线程中运行

static ICMP_CHAT *__chat = NULL;

void ping::rpc_run()
{
	if (load_file() == true)
		ping_all();
}

bool ping::load_file()
{
	acl::ifstream in;
	if (in.open_read(filepath_) == false)
	{
		logger_error("open file(%s) failed: %s",
			filepath_.c_str(), acl::last_serror());
		return false;
	}

	acl::string line;
	while (in.eof() == false)
	{
		if (in.gets(line) == false)
			break;
		ip_list_.push_back(line);
	}

	if (ip_list_.empty())
	{
		logger_error("no ip in file %s", filepath_.c_str());
		return false;
	}
	return true;
}

static void display_res2(ICMP_CHAT *chat)
{
	if (chat) {
		/* 显示 PING 的结果总结 */
		icmp_stat(chat);
		printf(">>>max pkts: %d\r\n", icmp_chat_seqno(chat));
	}
}

static void display_res(void)
{
	if (__chat) {
		display_res2(__chat);

		/* 释放 ICMP 对象 */
		icmp_chat_free(__chat);
		__chat = NULL;
	}
}

void ping::ping_all()
{
	if (ip_list_.empty())
	{
		logger_error("ip empty");
		return;
	}

	ACL_AIO *aio;
	/* 创建非阻塞异步通信句柄 */
	aio = acl_aio_create(ACL_EVENT_SELECT);
	acl_aio_set_keep_read(aio, 0);
	/* 创建 ICMP 对象 */
	__chat = icmp_chat_create(aio, 1);

	// 添加目标 IP 地址
	std::list<acl::string>::const_iterator cit = ip_list_.begin();
	for (; cit != ip_list_.end(); ++cit)
	{
		icmp_ping_one(__chat, NULL, (*cit).c_str(),
			npkt_, delay_, timeout_);
	}

	while (1) {
		/* 如果 PING 结束，则退出循环 */
		if (icmp_chat_finish(__chat)) {
			printf("over now!, hosts' size=%d, count=%d\r\n",
				icmp_chat_size(__chat), icmp_chat_count(__chat));
			break;
		}

		/* 异步事件循环过程 */
		acl_aio_loop(aio);
	}

	/* 显示 PING 结果 */
	display_res();

	/* 销毁非阻塞句柄 */
	acl_aio_free(aio);
}