#include "acl_stdafx.hpp"
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/stdlib/util.hpp"
#include "acl_cpp/stream/socket_stream.hpp"
#include "acl_cpp/stream/server_socket.hpp"

namespace acl {

server_socket::server_socket(int backlog /* = 128 */, bool block /* = true */)
: backlog_(backlog)
, block_(block)
, unix_sock_(false)
, fd_(ACL_SOCKET_INVALID)
{
}

server_socket::~server_socket()
{
	if (fd_ != ACL_SOCKET_INVALID)
		acl_socket_close(fd_);
}

bool server_socket::open(const char* addr)
{
	snprintf(addr_, sizeof(addr_), "%s", addr);

#ifndef WIN32
	if (strchr(addr, '/') != NULL)
	{
		fd_ = acl_unix_listen(addr, backlog_, block_);
		unix_sock_ = true;
	}
	else
#endif
		fd_ = acl_inet_listen(addr, backlog_, block_);

	if (fd_ == ACL_SOCKET_INVALID)
	{
		logger_error("listen %s error %s", addr, last_serror());
		unix_sock_ = false;
		return false;
	}

	return true;
}

socket_stream* server_socket::accept(int timeout /* = 0 */)
{
	if (fd_ == ACL_SOCKET_INVALID)
	{
		logger_error("server socket not opened!");
		return NULL;
	}

	if (block_ && timeout > 0)
	{
		if (acl_read_wait(fd_, timeout) == -1)
			return NULL;
	}

	ACL_SOCKET fd;
#ifndef WIN32
	if (unix_sock_)
	{
		fd = acl_unix_accept(fd_);
	}
	else
#endif
		fd = acl_inet_accept(fd_);
	if (fd == ACL_SOCKET_INVALID)
	{
		if (block_)
			logger_error("accept error %s", last_serror());
		else if (last_error() != ACL_EAGAIN
			&& last_error() != ACL_EWOULDBLOCK)
		{
			logger_error("accept error %s", last_serror());
		}
		return NULL;
	}

	socket_stream* client = new socket_stream();
	if (client->open(fd) == false)
	{
		logger_error("create socket_stream error!");
		return NULL;
	}

	return client;
}

} // namespace acl
