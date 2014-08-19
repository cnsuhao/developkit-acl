#include "acl_stdafx.hpp"
#ifdef HAS_POLARSSL
# include "polarssl/ssl.h"
# include "polarssl/havege.h"
# include "polarssl/entropy.h"
# include "polarssl/ctr_drbg.h"
# include "polarssl/certs.h"
# include "polarssl/x509_crt.h"
# include "polarssl/ssl_cache.h"
#endif
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/stream/polarssl_conf.hpp"

namespace acl
{

polarssl_conf::polarssl_conf()
{
#ifdef HAS_POLARSSL
	cacert_ = NULL;
	entropy_ = acl_mycalloc(1, sizeof(entropy_context));
	entropy_init((entropy_context*) entropy_);
# ifdef HAS_SSLCACHE
	cache_ = acl_mycalloc(1, sizeof(ssl_cache_context));
# endif
#endif
}

polarssl_conf::~polarssl_conf()
{
#ifdef HAS_POLARSSL
	free_ca();
	free_certs();
	free_pkeys();
	entropy_free((entropy_context*) entropy_);
	acl_myfree(entropy_);

# ifdef HAS_SSLCACHE
	if (cache_)
	{
		ssl_cache_free((ssl_cache_context*) cache_);
		acl_myfree(cache_);
	}
# endif

#endif
}

void polarssl_conf::free_ca()
{
#ifdef HAS_POLARSSL
	if (cacert_)
	{
		x509_crt_free((x509_crt*) cacert_);
		acl_myfree(cacert_);
		cacert_ = NULL;
	}
#endif
}

void polarssl_conf::free_certs()
{
#ifdef HAS_POLARSSL
	std::vector<void*>::iterator iter = cert_chain_.begin();
	for (; iter != cert_chain_.end(); ++iter)
	{
		x509_crt_free((x509_crt*) (*iter));
		acl_myfree(*iter);
	}
	cert_chain_.clear();
#endif
}

void polarssl_conf::free_pkeys()
{
#ifdef HAS_POLARSSL
	std::vector<void*>::iterator iter = pkey_chain_.begin();
	for (; iter != pkey_chain_.end(); ++iter)
	{
		pk_free((pk_context*) (*iter));
		acl_myfree(*iter);
	}
	pkey_chain_.clear();
#endif
}

bool polarssl_conf::load_ca(const char* ca_file, const char* ca_path)
{
#ifdef HAS_POLARSSL
	free_ca();

	int  ret;
	cacert_ = acl_mycalloc(1, sizeof(x509_crt));
	x509_crt_init((x509_crt*) cacert_);

	if (ca_path && *ca_path)
	{
		ret = x509_crt_parse_path((x509_crt*) cacert_, ca_path);
		if (ret != 0)
		{
			logger_error("x509_crt_parse_path(%s) error: %d",
				ca_path, ret);
			free_ca();
			return false;
		}
		else
			return true;
	}

	if (ca_file == NULL || *ca_file == 0)
	{
		logger_error("ca_file null");
		free_ca();
		return false;
	}

	ret = x509_crt_parse_file((x509_crt*) cacert_, ca_file);
	if (ret != 0)
	{
		logger_error("x509_crt_parse_path(%s) error: %d", ca_path, ret);
		free_ca();
		return false;
	}
	else
		return true;
#else
	logger_error("HAS_POLARSSL not defined!");
	return false;
#endif
}

bool polarssl_conf::add_server_cert(const char* crt_file, const char* key_file)
{
	if (crt_file == NULL || *crt_file == 0)
	{
		logger_error("crt_file null");
		return false;
	}

	if (key_file == NULL || *key_file == 0)
	{
		logger_error("key_file null");
		return false;
	}

#ifdef HAS_POLARSSL
	int   ret;
	void* cert = acl_mycalloc(1, sizeof(x509_crt));
	void* pkey = acl_mycalloc(1, sizeof(pk_context));

	x509_crt_init((x509_crt*) cert);
	pk_init((pk_context*) pkey);

	ret = x509_crt_parse_file((x509_crt*) cert, crt_file);
	if (ret != 0)
	{
		logger_error("x509_crt_parse_file(%s) error: %d", crt_file, ret);

		x509_crt_free((x509_crt*) cert);
		pk_free((pk_context*) pkey);
		acl_myfree(cert);
		acl_myfree(pkey);
		return false;
	}

	ret = pk_parse_keyfile((pk_context*) pkey, key_file, "");
	if (ret != 0)
	{
		logger_error("pk_parse_keyfile(%s) error: %d", key_file, ret);

		x509_crt_free((x509_crt*) cert);
		pk_free((pk_context*) pkey);
		acl_myfree(cert);
		acl_myfree(pkey);
		return false;
	}

	cert_chain_.push_back(cert);
	pkey_chain_.push_back(pkey);

	return true;
#else
	logger_error("HAS_POLARSSL not defined!");
	return false;
#endif
}

bool polarssl_conf::setup_certs(void* ssl_in)
{
#ifdef HAS_POLARSSL
	char pers[50];
	ctr_drbg_context ctr_drbg;
	int   ret;

	snprintf(pers, sizeof(pers), "SSL Pthread Thread %lu",
		(unsigned long) acl_pthread_self());

	if((ret = ctr_drbg_init(&ctr_drbg, entropy_func,
		(entropy_context*) entropy_,
		(const unsigned char *) pers,
		strlen( pers ) ) ) != 0 )
	{
		logger_error("ctr_drbg_init error: %04x\n", ret);
		return false;
	}

	ssl_context* ssl = (ssl_context*) ssl_in;
	if (cacert_)
		ssl_set_ca_chain(ssl, (x509_crt*) cacert_, NULL, NULL);

	size_t n = cert_chain_.size();
	acl_assert(n == pkey_chain_.size());

# ifdef HAS_SSLCACHE
	ssl_set_session_cache(ssl, ssl_cache_get, (ssl_cache_context*) cache_,
		ssl_cache_set, (ssl_cache_context*) cache_);
# endif

	for (size_t i = 0; i < n; ++i)
	{
		ret = ssl_set_own_cert(ssl, (x509_crt*) cert_chain_[i],
			(pk_context*) pkey_chain_[i]);
		if (ret != 0)
		{
			logger_error("ssl_set_own_cert failed, i: %d", i);
			return false;
		}
	}

	return true;
#else
	return false;
#endif
}

} // namespace acl
