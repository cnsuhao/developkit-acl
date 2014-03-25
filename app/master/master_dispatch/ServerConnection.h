#pragma once
#include "IConnection.h"

//class ServerIOCallback;

class ServerConnection : public IConnection
{
public:
	ServerConnection(acl::aio_socket_stream* conn);
	~ServerConnection() {}

	void set_nconns(unsigned int nconns);
	unsigned int get_nconns() const
	{
		return nconns_;
	}

	void close();

protected:
	void run();

private:
//	ServerIOCallback* callback_;
	unsigned int nconns_;
};
