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
// ���߳�������

void ping::rpc_onover()
{
	callback_->enable_ping();
	delete this;
}

void ping::rpc_wakeup(void* ctx)
{

}

//////////////////////////////////////////////////////////////////////////
// ���߳�������

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
		/* ��ʾ PING �Ľ���ܽ� */
		icmp_stat(chat);
		printf(">>>max pkts: %d\r\n", icmp_chat_seqno(chat));
	}
}

static void display_res(void)
{
	if (__chat) {
		display_res2(__chat);

		/* �ͷ� ICMP ���� */
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
	/* �����������첽ͨ�ž�� */
	aio = acl_aio_create(ACL_EVENT_SELECT);
	acl_aio_set_keep_read(aio, 0);
	/* ���� ICMP ���� */
	__chat = icmp_chat_create(aio, 1);

	// ���Ŀ�� IP ��ַ
	std::list<acl::string>::const_iterator cit = ip_list_.begin();
	for (; cit != ip_list_.end(); ++cit)
	{
		icmp_ping_one(__chat, NULL, (*cit).c_str(),
			npkt_, delay_, timeout_);
	}

	while (1) {
		/* ��� PING ���������˳�ѭ�� */
		if (icmp_chat_finish(__chat)) {
			printf("over now!, hosts' size=%d, count=%d\r\n",
				icmp_chat_size(__chat), icmp_chat_count(__chat));
			break;
		}

		/* �첽�¼�ѭ������ */
		acl_aio_loop(aio);
	}

	/* ��ʾ PING ��� */
	display_res();

	/* ���ٷ�������� */
	acl_aio_free(aio);
}