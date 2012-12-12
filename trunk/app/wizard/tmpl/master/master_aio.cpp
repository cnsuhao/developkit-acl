#include "stdafx.h"
#include "aio_socket_stream.hpp"
#include "master_service.h"

master_service::master_service()
{
}

master_service::~master_service()
{
}

bool master_service::on_accept(acl::aio_socket_stream*)
{
	return true;
}
