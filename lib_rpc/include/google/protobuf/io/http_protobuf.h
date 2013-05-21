#pragma once
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include <google/protobuf/stubs/common.h>
#include "acl_cpp/http/http_request.hpp"
#include "acl_cpp/http/http_response.hpp"

namespace google {
namespace protobuf {

class MessageLite;

namespace io {

class LIBPROTOBUF_EXPORT http_request
{
public:
	explicit http_request(acl::http_request* request);
	explicit http_request(const char* addr, int conn_timeout = 60,
		int rw_timeout = 60);
	~http_request();

	bool rpc_request(const MessageLite& in, MessageLite* out);
private:
	acl::http_request* request_;
	acl::http_request* request_inner_;
	char* addr_;
	int   conn_timeout_;
	int   rw_timeout_;
};

class LIBPROTOBUF_EXPORT http_response
{
public:
	explicit http_response(acl::http_response* response);
	~http_response();

	bool read_request(MessageLite* out);
	bool send_response(const MessageLite& in);
private:
	acl::http_response* response_;
};

}  // namespace io
}  // namespace protobuf
}  // namespace google
