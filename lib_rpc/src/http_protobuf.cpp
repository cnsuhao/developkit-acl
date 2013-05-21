#include "stdafx.h"
#include "google/protobuf/message_lite.h"
#include "google/protobuf/io/http_protobuf.h"
#include "lib_acl.h"
#include "acl_cpp/stdlib/string.hpp"

namespace google {
namespace protobuf {
namespace io {

http_request::http_request(acl::http_request* request)
: request_(request)
, request_inner_(NULL)
, addr_(NULL)
, conn_timeout_(0)
, rw_timeout_(0)
{
	assert(request);
}

http_request::http_request(const char* addr, int conn_timeout /* = 60 */,
	int rw_timeout /* = 60 */)
: conn_timeout_(conn_timeout)
, rw_timeout_(rw_timeout)
, request_(NULL)
, request_inner_(NULL)
{
	addr_ = acl_mystrdup(addr);
}

http_request::~http_request()
{
	if (request_inner_)
		delete request_inner_;
	if (addr_)
		acl_myfree(addr_);
}

bool http_request::rpc_request(const MessageLite& in, MessageLite* out)
{
	if (request_ == NULL)
	{
		if (addr_ == NULL)
			return false;
		request_inner_ = new acl::http_request(addr_,
			conn_timeout_, rw_timeout_);
		request_ = request_inner_;
	}

	std::string buf;

	in.SerializeToString(&buf);
	if (request_->request(buf.c_str(), buf.length()) == false)
		return false;
	if (out == NULL)
		return true;

	buf.clear();
	acl::string tmp;
	int   ret;
	while (true)
	{
		ret = request_->read_body(tmp, true);
		if (ret < 0)
			return false;
		else if (ret == 0)
			break;
		buf.append(tmp.c_str(), (size_t) ret);
	}

	if (out->ParseFromString(buf) == false)
		return false;
	return true;
}

//////////////////////////////////////////////////////////////////////////

http_response::http_response(acl::http_response* response)
: response_(response)
{
	assert(response);
}

http_response::~http_response()
{
}

bool http_response::read_request(MessageLite* out)
{
	if (out == NULL)
		return false;
	if (response_->read_header() == false)
		return false;
	acl::string buf;
	if (response_->get_body(buf) == false)
		return false;
	std::string data(buf.c_str(), (int) buf.length());
	return out->ParseFromString(data);
}

bool http_response::send_response(const MessageLite& in)
{
	std::string buf;

	in.SerializeToString(&buf);
	return response_->response(buf.c_str(), buf.length());
}

}  // namespace io
}  // namespace protobuf
}  // namespace google
