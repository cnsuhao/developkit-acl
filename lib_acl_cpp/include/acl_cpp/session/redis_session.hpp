#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/session/session.hpp"

namespace acl
{

class redis;
class redis_cluster;

class ACL_CPP_API redis_session : public session
{
public:
	redis_session(const char* addrs, size_t max_conns = 127,
		int conn_timeout = 180, int rw_timeout = 300,
		time_t ttl = 0, const char* sid = NULL);
	~redis_session();

private:
	// 基类纯虚函数，从 redis 中获得数据
	bool get_attrs(const char* sid, std::map<string, VBUF*>& attrs);

	// 基类纯虚函数，向 redis 中添加或修改数据
	bool set_attrs(const char* sid, std::map<string, VBUF*>& attrs,
		time_t ttl);

	// 基类纯虚函数，从 redis 中删除数据
	bool del_key(const char* sid);

	//重新设置 session 在 redis 上的缓存时间
	bool set_timeout(const char* sid, time_t ttl);

private:
	redis_cluster* cluster_;
	redis* client_;
	size_t max_conns_;
};

} // namespace acl
