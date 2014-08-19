#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <vector>

namespace acl
{

class polarssl_conf
{
public:
	polarssl_conf();
	~polarssl_conf();

	bool load_ca(const char* ca_file, const char* ca_path);
	bool add_server_cert(const char* crt_file, const char* key_file);

	bool setup_certs(void* ssl);

private:
	void* cacert_;
	void* entropy_;
	std::vector<void*> cert_chain_;
	std::vector<void*> pkey_chain_;
	void* cache_;
	void free_ca();
	void free_certs();
	void free_pkeys();
};

} // namespace acl
