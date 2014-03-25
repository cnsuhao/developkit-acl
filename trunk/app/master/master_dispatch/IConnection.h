#pragma once

class IConnection
{
public:
	IConnection(acl::aio_socket_stream* conn) : conn_(conn) {}
	virtual ~IConnection() {}

	virtual void run() = 0;

	int   sock_handle() const;
	const char* get_peer(bool full = true) const;

protected:
	acl::aio_socket_stream* conn_;
};
