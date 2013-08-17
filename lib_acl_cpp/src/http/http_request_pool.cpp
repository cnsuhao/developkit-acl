#include "acl_stdafx.hpp"
#include "acl_cpp/http/http_request_pool.h"

namespace acl
{

http_reuqest_pool::http_reuqest_pool(const char* addr,
	int count, int retry_inter /* = 0 */)
: connect_pool(addr, count, retry_inter)
{

}

http_reuqest_pool::~http_reuqest_pool()
{

}

connect_client* http_reuqest_pool::create_connect()
{
	http_request* request = new http_request();
	return request;
}

} // namespace acl
