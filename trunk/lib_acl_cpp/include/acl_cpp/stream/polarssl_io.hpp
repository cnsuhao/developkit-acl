#pragma once

#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/stream/stream.hpp"

namespace acl {

class stream;
class polarssl_conf;

class ACL_CPP_API polarssl_io : public stream_hook
{
public:
	polarssl_io(bool server_side);

	polarssl_io& set_conf(polarssl_conf* conf);
	virtual void destroy();

private:
	~polarssl_io();

	virtual bool open(const stream* s);
	virtual bool on_close();
	virtual int read(void* buf, size_t len);
	virtual int send(const void* buf, size_t len);

private:
	bool server_side_;
	polarssl_conf* conf_;
	void* ssl_;
	void* ssn_;
	void* hs_;
	const stream* stream_;

	static int sock_read(void *ctx, unsigned char *buf, size_t len);
	static int sock_send(void *ctx, const unsigned char *buf, size_t len);
};

} // namespace acl
