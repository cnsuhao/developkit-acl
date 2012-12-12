#include "stdafx.h"
#include "socket_stream.hpp"
#include "master_service.h"

master_service::master_service()
{
}

master_service::~master_service()
{
}

bool master_service::thread_on_read(acl::socket_stream*)
{
	return true;
}

bool master_service::thread_on_accept(acl::socket_stream*)
{
	return true;
}

void master_service::thread_on_close(acl::socket_stream*)
{
}

void master_service::thread_on_init()
{
}

void master_service::thread_on_exit()
{
}
