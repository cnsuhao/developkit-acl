#include "acl_stdafx.hpp"
#ifdef HAS_POLARSSL
# include "polarssl/ssl.h"
# include "polarssl/havege.h"
#endif
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/stream/stream.hpp"
#include "acl_cpp/stream/polarssl_conf.hpp"
#include "acl_cpp/stream/polarssl_io.hpp"

namespace acl {

polarssl_io::polarssl_io(bool server_side)
{
	server_side_ = server_side;
	conf_ = NULL;
	ssl_ = NULL;
	ssn_ = NULL;
	hs_ = NULL;
	stream_ = NULL;
}

polarssl_io::~polarssl_io()
{
#ifdef HAS_POLARSSL
	if (ssl_)
	{
		ssl_free((ssl_context*) ssl_);
		acl_myfree(ssl_);
		ssl_ = NULL;
	}
	if (ssn_)
	{
		acl_myfree(ssn_);
		ssn_ = NULL;
	}
	if (hs_)
	{
		acl_myfree(hs_);
		hs_ = NULL;
	}
#endif
}

polarssl_io& polarssl_io::set_conf(polarssl_conf* conf)
{
	conf_ = conf;
	return *this;
}

void polarssl_io::destroy()
{
	delete this;
}

/*
static void my_mutexed_debug( void *ctx, int level, const char *str )
{
	fprintf( (FILE *) ctx, "%s", str );
	fflush(  (FILE *) ctx  );
}
*/

bool polarssl_io::open(const stream* s)
{
	if (s == NULL)
	{
		logger_error("stream null");
		return false;
	}

	stream_ = s;

#ifdef HAS_POLARSSL

	// 如果打开已经是 SSL 模式的流，则直接返回
	if (ssl_ != NULL)
	{
		acl_assert(ssn_);
		acl_assert(hs_);
		return true;
	}

	ssl_ = acl_mycalloc(1, sizeof(ssl_context));
	ssn_ = acl_mycalloc(1, sizeof(ssl_session));
	hs_  = acl_mymalloc(sizeof(havege_state));

	// Initialize the RNG and the session data
	::havege_init((havege_state*) hs_);

	int   ret;

	// Setup stuff
	if ((ret = ::ssl_init((ssl_context*) ssl_)) != 0)
	{
		logger_error("failed, ssl_init returned %d", ret);
		return false;
	}

	if (server_side_)
		::ssl_set_endpoint((ssl_context*) ssl_, SSL_IS_SERVER);
	else
		::ssl_set_endpoint((ssl_context*) ssl_, SSL_IS_CLIENT);

	::ssl_set_authmode((ssl_context*) ssl_, SSL_VERIFY_NONE);

	::ssl_set_rng((ssl_context*) ssl_, havege_random, hs_);
	//ssl_set_dbg(ssl_, my_debug, stdout);
	//ssl_set_dbg((ssl_context*) ssl_, my_mutexed_debug, stdout);
	
	const int* cipher_suites = ::ssl_list_ciphersuites();
	if (cipher_suites == NULL)
	{
		logger_error("ssl_list_ciphersuites null");
		return false;
	}

	::ssl_set_ciphersuites((ssl_context*) ssl_, cipher_suites);
	::ssl_set_session((ssl_context*) ssl_, (ssl_session*) ssn_);

	// 如果有证书配置项，则调用配置过程
	if (conf_)
		conf_->setup_certs(ssl_);

	::ssl_set_bio((ssl_context*) ssl_, sock_read, this, sock_send, this);

	// Handshake
	while((ret = ::ssl_handshake((ssl_context*) ssl_)) != 0)
	{
		if (ret != POLARSSL_ERR_NET_WANT_READ
			&& ret != POLARSSL_ERR_NET_WANT_WRITE)
		{
			logger_error("ssl_handshake failed: -0x%04x", ret);
			return false;
		}
	}

	return true;
#else
	logger_error("define HAS_POLARSSL first!");
	return false;
#endif
}

bool polarssl_io::on_close()
{
#ifdef HAS_POLARSSL
	if (ssl_ == NULL)
	{
		logger_error("ssl_ null");
		return false;
	}
	if (stream_ == NULL)
	{
		logger_error("stream_ null");
		return false;
	}

	int   ret;
	while((ret = ssl_close_notify((ssl_context*) ssl_ )) < 0)
	{
		if( ret != POLARSSL_ERR_NET_WANT_READ &&
			ret != POLARSSL_ERR_NET_WANT_WRITE )
		{
			logger_warn("ssl_close_notify error: %d", ret);
			return false;
		}
	}
#endif

	return true;
}

int polarssl_io::sock_read(void *ctx, unsigned char *buf, size_t len)
{
#ifdef HAS_POLARSSL
	polarssl_io* io = (polarssl_io*) ctx;
	int   ret, timeout = 120;
	ACL_VSTREAM* vs = io->stream_->get_vstream();
	ACL_SOCKET fd = ACL_VSTREAM_SOCK(vs);

	ret = acl_socket_read(fd, buf, len, timeout, vs, NULL);
	if (ret < 0)
	{
		int   errnum = acl_last_error();
		if (ret == ACL_EINTR || ret == ACL_EWOULDBLOCK
#if ACL_EWOULDBLOCK != ACL_EAGAIN
			|| ret == ACL_EAGAIN
#endif
			)
			return POLARSSL_ERR_NET_WANT_READ;
		else if (errnum == ACL_ECONNRESET || errno == EPIPE)
			return POLARSSL_ERR_NET_CONN_RESET;
		else
			return POLARSSL_ERR_NET_RECV_FAILED;
	}

	return ret;
#else
	(void) ctx;
	(void) buf;
	(void) len;
	return -1;
#endif
}

int polarssl_io::sock_send(void *ctx, const unsigned char *buf, size_t len)
{
#ifdef HAS_POLARSSL
	polarssl_io* io = (polarssl_io*) ctx;
	int   ret, timeout = 120;
	ACL_VSTREAM* vs = io->stream_->get_vstream();
	ACL_SOCKET fd = ACL_VSTREAM_SOCK(vs);

	ret = acl_socket_write(fd, buf, len, timeout, vs, NULL);
	if (ret < 0)
	{
		int   errnum = acl_last_error();
		if (ret == ACL_EINTR || ret == ACL_EWOULDBLOCK
#if ACL_EWOULDBLOCK != ACL_EAGAIN
			|| ret == ACL_EAGAIN
#endif
			)
			return POLARSSL_ERR_NET_WANT_WRITE;
		else if (errnum == ACL_ECONNRESET || errno == EPIPE)
			return POLARSSL_ERR_NET_CONN_RESET;
		else
		{
			logger_error("write error: %s", acl::last_serror());
			return POLARSSL_ERR_NET_SEND_FAILED;
		}
	}

	return ret;
#else
	(void) ctx;
	(void) buf;
	(void) len;
	return -1;
#endif
}

int polarssl_io::read(void* buf, size_t len)
{
#ifdef HAS_POLARSSL
	int   ret;

	while ((ret = ::ssl_read((ssl_context*) ssl_,
		(unsigned char*) buf, len)) < 0)
	{
		if (ret != POLARSSL_ERR_NET_WANT_READ
			&& ret != POLARSSL_ERR_NET_WANT_WRITE)
		{
			return ACL_VSTREAM_EOF;
		}
	}

	return ret;
#else
	(void) buf;
	(void) len;
	return -1;
#endif
}

int polarssl_io::send(const void* buf, size_t len)
{
#ifdef HAS_POLARSSL
	int   ret;

	while ((ret = ::ssl_write((ssl_context*) ssl_,
		(unsigned char*) buf, len)) < 0)
	{
		if (ret != POLARSSL_ERR_NET_WANT_READ
			&& ret != POLARSSL_ERR_NET_WANT_WRITE)
		{
			return ACL_VSTREAM_EOF;
		}
	}

	return ret;
#else
	(void) buf;
	(void) len;
	return -1;
#endif
}

} // namespace acl
