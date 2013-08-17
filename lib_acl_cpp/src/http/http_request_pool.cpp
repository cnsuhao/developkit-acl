#include "acl_stdafx.hpp"
#include "acl_cpp/http/http_request_pool.hpp"

namespace acl
{

http_reuqest_pool::http_reuqest_pool(const char* addr,
	int count, int retry_inter /* = 0 */)
: connect_pool(addr, count, retry_inter)
{
	conn_timeout_ = 30;
	rw_timeout_ = 30;
}

http_reuqest_pool::~http_reuqest_pool()
{

}

http_reuqest_pool& http_reuqest_pool::set_timeout(int conn_timeout /* = 30 */,
	int rw_timeout /* = 60 */)
{
	conn_timeout_ = conn_timeout;
	rw_timeout_ = rw_timeout;
	return *this;
}

connect_client* http_reuqest_pool::create_connect()
{
	http_request* request = NEW http_request(addr_,
		conn_timeout_, rw_timeout_);
	return request;
}

} // namespace acl
