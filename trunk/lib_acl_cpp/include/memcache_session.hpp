#pragma once
#include "acl_cpp_define.hpp"
#include "session.hpp"

namespace acl
{

	class mem_cache;

	/**
	 * session 类，该类使用 memcached 存储 session 数据
	 */
	class ACL_CPP_API memcache_session : public session
	{
	public:
		/**
		 * 构建函数
		 * @param cache_addr {const char*} memcached 服务地址，格式：
		 *  IP:PORT，不能为空
		 * @param prefix {const char*} 在 memcached 存储的键值的前缀
		 * @param ttl {time_t} 生存周期(秒)
		 * @param sid {const char*} session 对应的 sid，当为空时，内部
		 *  会自动生成一个，其它说明请参考基类 session 的说明
		 */
		memcache_session(const char* cache_addr, const char* prefix = NULL,
			time_t ttl = 0, const char* sid = NULL);
		~memcache_session(void);

	private:
		// 基类纯虚函数，从 memcached 中获得数据
		virtual bool get_data(string& buf);

		// 基类纯虚函数，向 memcached 中添加或修改数据
		virtual bool set_data(const char* buf, size_t len, time_t ttl);

		// 基类纯虚函数，从 memcached 中删除数据
		virtual bool del_data(void);

		//重新设置 session 在缓存服务器上的缓存时间
		virtual bool set_timeout(time_t ttl);
	private:
		mem_cache* cache_;
	};

} // namespace acl
