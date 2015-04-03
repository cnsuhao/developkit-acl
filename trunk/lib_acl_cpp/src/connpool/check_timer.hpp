#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <map>
#include <vector>
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/stream/aio_timer_callback.hpp"

namespace acl
{

class connect_monitor;
class aio_handle;
class aio_socket_stream;

class check_timer : public aio_timer_callback
{
public:
	check_timer(connect_monitor& monitor, aio_handle& handle,
		int conn_timeout);
	~check_timer() {}

	connect_monitor& get_monitor() const
	{
		return monitor_;
	}

	void remove_client(const char* addr, aio_socket_stream& conn);
	bool finish(bool graceful);

protected:
	// »ùÀà´¿Ðéº¯Êý
	void timer_callback(unsigned int id);
	void destroy(void) {}

private:
	int id_;
	bool stopping_;
	connect_monitor& monitor_;
	aio_handle& handle_;
	int   conn_timeout_;
	std::map<string, int> addrs_;
	std::vector<aio_socket_stream*> conns_;
};

} // namespace acl
