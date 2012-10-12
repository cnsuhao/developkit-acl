#pragma once
#include "acl_cpp_define.hpp"
#include <map>
#include "string.hpp"

namespace acl
{

	// 用来存储属性值的缓冲区对象定义，这主要是为了兼容属性值
	// 可以为二进制的情形而增加的结构类型
	struct VBUF
	{
		size_t len;
		char  buf[1];
	};

	/**
	 * session 类，该类使用 memcached 存储 session 数据
	 */
	class ACL_CPP_API session
	{
	public:
		/**
		 * 在调用本构造函数后，内部会自动生成一个唯一 ID,
		 * 用户应保留该 ID (通过 get_sid() 获得)，然后以后每次
		 * 再用时应该先调用 set_sid(const char*) 再设置一次与本
		 * session 关联的 ID
		 */
		session(time_t ttl = 0);
		virtual ~session(void);
		
		/**
		 * 重置内部状态，清理掉一些临时数据
		 */
		void reset(void);

		/**
		 * 获得本 session 对象的唯一 ID 标识
		 * @return {const char*} 非空
		 */
		virtual const char* get_sid(void) const;

		/**
		 * 设置本 session 对象的唯一 ID 标识，当调用以下函数时，
		 * 应该必须先调用 set_sid(const char*) 设置本 session 对象
		 * 对应的数据对象标识 id
		 * @param sid {const char*} 非空
		 */
		void set_sid(const char* sid);

		/**
		 * 向 session 中添加新的字符串属性，同时设置该
		 * session 的过期时间间隔(秒)
		 * @param name {const char*} session 名，非空
		 * @param value {const char*} session 值，非空
		 * @return {bool} 返回 false 表示出错
		 */
		bool set(const char* name, const char* value);

		/**
		 * 向 session 中添加新的属性对象，同时设置该
		 * session 的过期时间间隔(秒)
		 * @param name {const char*} session 属性名，非空
		 * @param value {const char*} session 属性值，非空
		 * @param len {size_t} value 值长度
		 * @return {bool} 返回 false 表示出错
		 */
		bool set(const char* name, const void* value, size_t len);
		
		/**
		 * 从 session 中取得字符串类型属性值
		 * @param name {const char*} session 属性名，非空
		 * @return {const char*} session 属性值，返回空时表示出错或不存在
		 */
		const char* get(const char* name);

		/**
		 * 从 session 中取得二进制数据类型的属性值
		 * @param name {const char*} session 属性名，非空
		 * @return {const VBUF*} session 属性值，返回空时表示出错或不存在
		 */
		const VBUF* get_vbuf(const char* name);

		/**
		 * 从 session 中删除指定属性值，当所有的变量都删除
		 * 时会将整个对象从 memcached 中删除
		 * @param name {const char*} session 属性名，非空
		 * @return {bool} true 表示成功(含不存在情况)，false 表示删除失败
		 */
		bool del(const char* name);

		/**
		 * 重新设置 session 在缓存服务器上的缓存时间
		 * @param ttl {time_t} 生存周期(秒)
		 * @return {bool} 设置是否成功
		 */
		bool set_ttl(time_t ttl);

		/**
		 * 获得本 session 对象中记录的 session 生存周期；该值有可能
		 * 与真正存储在缓存服务器的时间不一致，因为有可能其它的实例
		 * 重新设置了 session 在缓存服务器上的生存周期
		 * @return {time_t}
		 */
		time_t get_ttl(void) const;

		/**
		 * 使 session 从服务端的缓存中删除即使 session 失效
		 * @return {bool} 是否使 session 失效
		 */
		bool remove(void);

	protected:
		// 获得对应 sid 的数据
		virtual bool get_data(string& buf) = 0;

		// 设置对应 sid 的数据
		virtual bool set_data(const char* buf, size_t len, time_t ttl) = 0;

		// 删除对应 sid 的数据
		virtual bool del_data(void) = 0;

		// 设置对应 sid 数据的过期时间
		virtual bool set_timeout(time_t ttl) = 0;

	private:
		time_t ttl_;
		char sid_[256];
		std::map<string, VBUF*> attrs_;

		// 将 session 数据序列化
		void serialize(string& buf);

		void serialize(const char* name, const void* value,
			size_t len, string& buf);

		// 将 session 数据反序列化
		bool deserialize(string& buf);

		// 分配内存对象
		static VBUF* vbuf_new(const void* str, size_t len);

		// 释放内存对象
		static void vbuf_free(VBUF* buf);
	};

} // namespace acl
