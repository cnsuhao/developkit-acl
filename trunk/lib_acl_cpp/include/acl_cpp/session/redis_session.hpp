#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <map>
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/session/session.hpp"

namespace acl
{

class redis;
class redis_client_cluster;

class ACL_CPP_API redis_session : public session
{
public:
	redis_session(const char* addrs, size_t max_conns,
		int conn_timeout = 180, int rw_timeout = 300,
		time_t ttl = 0, const char* sid = NULL);
	~redis_session();

protected:
	// 基类虚函数，向 redis 服务端设置哈希属性值
	bool set(const char* name, const void* value, size_t len);

	// 基类虚函数，从 redis 服务端的哈希对象中获得对应属性的值
	const session_string* get_buf(const char* name);

	// 基类虚函数，从 redis 服务端的哈希对象中删除某个属性值
	bool del(const char* name);

private:
	// 基类纯虚函数，从 redis 中获得数据
	bool get_attrs(const char* sid, std::map<string, session_string>& attrs);

	// 基类纯虚函数，向 redis 中添加或修改数据
	bool set_attrs(const char* sid, const std::map<string, session_string>& attrs,
		time_t ttl);

	// 基类纯虚函数，从 redis 中删除数据
	bool del_key(const char* sid);

	//重新设置 session 在 redis 上的缓存时间
	bool set_ttl(const char* sid, time_t ttl);

private:
	redis_client_cluster* cluster_;
	redis* command_;
	size_t max_conns_;
	session_string ss_;
};

} // namespace acl
