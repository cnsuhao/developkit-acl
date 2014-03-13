#include "acl_stdafx.hpp"
#include "acl_cpp/stream/aio_socket_stream.hpp"
#include "acl_cpp/stream/aio_timer_callback.hpp"
#include "acl_cpp/connpool/connect_manager.hpp"
#include "acl_cpp/connpool/connect_pool.hpp"
#include "acl_cpp/connpool/connect_monitor.hpp"

namespace acl
{

/**
 * �첽���ӻص�����������
 */
class check_client : public aio_open_callback
{
public:
	check_client(connect_manager& manager, aio_socket_stream* conn,
		const char* addr, std::map<string, int>& addrs);
	~check_client();

private:
	// �����麯��
	bool open_callback();
	void close_callback();
	bool timeout_callback();

private:
	bool connected_;
	connect_manager& manager_;
	aio_socket_stream* conn_;
	string addr_;
	std::map<string, int>& addrs_;
};

check_client::check_client(connect_manager& manager, aio_socket_stream* conn,
	const char* addr, std::map<string, int>& addrs)
: connected_(false)
, manager_(manager)
, conn_(conn)
, addr_(addr)
, addrs_(addrs)
{
}

check_client::~check_client()
{
}

bool check_client::open_callback()
{
	connected_ = true;
	return false;  // ���� false �û���ر�����
}

void check_client::close_callback()
{
	// ���δ�ɹ����ӷ������������ø����ӳ�״̬Ϊ������״̬��
	// ��������Ϊ���״̬
	//printf(">>>server: %s %s\r\n", addr_.c_str(),
	//	connected_ ? "alive" : "dead");

	// �ӵ�ǰ����������ַ�б���ɾ����ǰ�ļ���ַ
	std::map<string, int>::iterator it = addrs_.find(addr_);
	if (it != addrs_.end())
		addrs_.erase(it);

	manager_.set_pools_status(addr_, connected_ ? true : false);

	delete this;
}

bool check_client::timeout_callback()
{
	// ���ӳ�ʱ����ֱ�ӷ���ʧ��
	return false;
}

//////////////////////////////////////////////////////////////////////////

class check_timer : public aio_timer_callback
{
public:
	check_timer(connect_manager& manager, aio_handle& handle,
		int conn_timeout);
	~check_timer();

protected:
	// ���ി�麯��
	void timer_callback(unsigned int id);
	void destroy(void);

private:
	connect_manager& manager_;
	aio_handle& handle_;
	int   conn_timeout_;
	std::map<string, int> addrs_;
};

check_timer::check_timer(connect_manager& manager,
	aio_handle& handle, int conn_timeout)
: manager_(manager)
, handle_(handle)
, conn_timeout_(conn_timeout)
{
}

check_timer::~check_timer()
{
}

void check_timer::timer_callback(unsigned int)
{
	const char* addr;

	std::map<string, int>::const_iterator cit1;

	// ����ȡ���з�������ַ

	manager_.lock();

	const std::vector<connect_pool*>& pools = manager_.get_pools();
	std::vector<connect_pool*>::const_iterator cit2 = pools.begin();

	for (; cit2 != pools.end(); ++cit2)
	{
		addr = (*cit2)->get_addr();
		if (addr == NULL || *addr == 0)
			continue;

		cit1 = addrs_.find(addr);
		if (cit1 != addrs_.end())
			addrs_[addr] = 1;
	}

	manager_.unlock();

	// �������з�������ַ

	aio_socket_stream* conn;
	check_client* checker;
	std::map<string, int>::const_iterator cit1_next;

	for (cit1 = addrs_.begin(); cit1 != addrs_.end(); cit1 = cit1_next)
	{
		cit1_next = cit1;
		++cit1_next;

		addr = cit1->first.c_str();
		conn = aio_socket_stream::open(&handle_,
			addr, conn_timeout_);
		if (conn == NULL)
		{
			manager_.set_pools_status(addr, false);
			addrs_.erase(cit1);
		}
		else
		{
			checker = new check_client(manager_, conn,
				addr, addrs_);
			conn->add_open_callback(checker);
			conn->add_close_callback(checker);
			conn->add_timeout_callback(checker);
		}
	}
}

void check_timer::destroy()
{

}

//////////////////////////////////////////////////////////////////////////

connect_monitor::connect_monitor(connect_manager& manager,
	int check_inter /* = 1 */, int conn_timeout /* = 10 */)
: stop_(false)
, manager_(manager)
, check_inter_(check_inter)
, conn_timeout_(conn_timeout)
{

}

connect_monitor::~connect_monitor()
{

}

void connect_monitor::stop()
{
	stop_ = true;
}

void* connect_monitor::run()
{
	// �����������״̬��ʱ��
	check_timer timer(manager_, handle_, conn_timeout_);

	timer.keep_timer(true);  // ���ֶ�ʱ��
	handle_.set_timer(&timer, check_inter_ * 1000000);

	while (!stop_)
		handle_.check();

	handle_.check();

	return NULL;
}

} // namespace acl
