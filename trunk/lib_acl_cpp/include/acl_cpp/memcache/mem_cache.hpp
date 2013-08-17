#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <time.h>
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/mime/rfc2047.hpp"

namespace acl {

class socket_stream;

/**
 * memcached 客户端通信协议库，支持长连接与自动重连
 */
class ACL_CPP_API mem_cache : public connect_client
{
public:
	/**
	* 构造函数
	* @param key_pre {const char*} 每一个应用都应该有自己的键值前缀，
	*  以便于在使用memcache时和其它的应用区别开，也免得与其它应用的
	*  键产生重复，该键前缀必须针对所有的应用程序来说是唯一的，如对
	*  于 receiver_server 程序来说，最好设成 receiver_server，可以方便
	*  地看出该类应用的含义来
	* @param addr {const char*} memcached 服务器监听地址，格式为：
	*  ip:port，如: 127.0.0.1:11211
	* @param conn_timeout {int} 连接超时时间(秒)
	* @param rw_timeout {int} IO 读写超时时间(秒)
	*/
	mem_cache(const char* addr = "127.0.0.1:11211",
		int conn_timeout = 180, int rw_timeout = 300);

	/**
	 * 构造函数，调用者在使用此构造函数时，必须首先调用 open 函数连接服务器
	 */
	mem_cache();

	~mem_cache();

	/**
	 * 基类 connect_client 的纯虚函数，只有当调用默认的构造函数 mem_cache()
	 * 时才需要显式地调用本函数用来打开与服务端的连接
	 * @param addr {const char*} 服务器地址
	 * @param conn_timeout {int} 连接服务器的超时时间(秒)
	 * @param rw_timeout {int} IO 读写超时时间(秒)
	 */
	virtual bool open(const char* addr, int conn_timeout = 30,
		int rw_timemout = 60);

	/**
	 * 设置 key 的前缀，即实际的 key 将由 该前缀+原始key 组成，缺省时不设前缀，
	 * 当多个应用共用同一个 memcached 服务时，建议应用设置自身的 key 前缀，这样
	 * 可以避免与其它应用的 key 产生重复问题
	 * @param keypre {const char*} 非空时设置 key 前缀，否则取消 key 前缀
	 * @return {mem_cache&}
	 */
	mem_cache& set_prefix(const char* keypre);

	/**
	 * 在保持的长连接中断时是否要求自动重连，缺省为自动重连
	 * @param onoff {bool} 为 true 时表示长连接意外断开后自动重连
	 * @return {mem_cache&}
	 */
	mem_cache& auto_retry(bool onoff);

	/**
	 * 设置是否针对 KEY 键值进行编码，缺少时不对 key 编码，当应用的 key 中可能
	 * 会有特殊字符或二进制值时，建议调用此函数对 key 进行编码
	 * @parma onoff {bool} 为 true 表示内部需要对 key 进行编码
	 * @return {mem_cache&}
	 */
	mem_cache& encode_key(bool onoff);

	/**
	* 向 memcached 中修改或添加新的数据缓存对象
	* @param key {const char*} 键值
	* @param klen {size_t} key 键值长度
	* @param dat {const void*} 数据
	* @param dlen {size_t} data 数据长度
	* @param timeout {time_t} 缓存超时时间(秒)
	* @param flags {unsigned short} 附属的标志位
	* @return {bool} 是否成功
	*/
	bool set(const char* key, size_t klen,
		const void* dat, size_t dlen,
		time_t timeout = 0, unsigned short flags = 0);

	/**
	* 向 memcached 中修改或添加新的数据缓存对象
	* @param key {const char*} 字符串键值
	* @param dat {const void*} 数据
	* @param dlen {size_t} data 数据长度
	* @param timeout {time_t} 缓存超时时间(秒)
	* @param flags {unsigned short} 附属的标志位
	* @return {bool} 是否成功
	*/
	bool set(const char* key, const void* dat, size_t dlen,
		time_t timeout = 0, unsigned short flags = 0);

	/**
	* 更新 memcached 中已经存在的键的过期日期，因为目前 libmemcached 没有
	* 提供此接口，所以该函数实现的方式是先调用 get 取出对应键的值，然后再
	* 调用 set 重新设置该键的值及过期时间
	* @param key {const char*} 键值
	* @param klen {size_t} key 键值长度
	* @param timeout {time_t} 过期时间(秒)
	* @return {bool} 是否成功
	*/
	bool set(const char* key, size_t klen, time_t timeout = 0);

	/**
	* 更新 memcached 中已经存在的键的过期日期，因为目前 libmemcached 没有
	* 提供此接口，所以该函数实现的方式是先调用 get 取出对应键的值，然后再
	* 调用 set 重新设置该键的值及过期时间
	* @param key {const char*} 字符串键值
	* @param timeout {time_t} 过期时间(秒)
	* @return {bool} 是否成功
	*/
	bool set(const char* key, time_t timeout = 0);

	/**
	* 从 memcached 中获得对应键值的缓存数据
	* @param key {const char*} 字符串键值
	* @param klen {size_t} 键值长度
	* @param buf {string&} 存储结果的缓冲区，内部首先会清空该缓冲区
	* @param flags {unsigned short*} 存储附属的标志位
	* @return {bool} 返回 true 表示正确获得结果值，否则表示键值对应的
	*  数据不存在或出错
	*/
	bool get(const char* key, size_t klen, string& buf,
		unsigned short* flags = NULL);

	/**
	* 从 memcached 中获得对应键值的缓存数据
	* @param key {const char*} 字符串键值
	* @param buf {string&} 存储结果的缓冲区，内部首先会清空该缓冲区
	* @param flags {unsigned short*} 存储附属的标志位
	* @return {bool} 返回 true 表示正确获得结果值，否则表示键值对应的
	*  数据不存在或出错
	*/
	bool get(const char* key, string& buf, unsigned short* flags = NULL);

	/**
	* 从 memcached 中删除数据
	* @param key {const char*} 键值
	* @param klen {size_t} 键值长度
	* @return {bool} 删除是否成功
	*/
	bool del(const char* key, size_t klen);

	/**
	* 从 memcached 中删除数据
	* @param key {const char*} 字符串键值
	* @return {bool} 删除是否成功
	*/
	bool del(const char* key);

	/**
	* 获得上次操作 memcached 错误描述信息
	* @return {const char*} 错误描述信息，永不为空
	*/
	const char* last_serror() const;

	/**
	* 获得上次操作 memcached 的错误号
	* @return {int} 错误号
	*/
	int  last_error() const;

	/**
	* 打开与 memcached 的连接, 因为 set/get/del 操作都会自动打开与
	* memcached 的连接，所以不必显示地调用此函数来打开与 memcached
	* 的连接
	* @return {bool} 打开是否成功
	*/
	bool open();

	/**
	* 关闭与 memcached 的连接，一般该函数不需要调用，因为类对象在
	* 析构时会自动调用此函数
	*/
	void close();

	/**
	* 列出 memcached 连接的一些属性，调试用
	*/
	void property_list();

private:
	bool set(const string& key, const void* dat, size_t dlen,
		time_t timeout, unsigned short flags);
	bool get(const string& key, string& buf, unsigned short* flags);
	const string& get_key(const char* key, size_t klen);

	string* keypre_;
	rfc2047 coder_;
	int   conn_timeout_;
	int   rw_timeout_;
	bool  encode_key_;

	bool  opened_;
	bool  retry_;
	char* addr_;
	char* ip_;
	int   port_;
	int   enum_;
	string ebuf_;
	string kbuf_;

	socket_stream* conn_;
	string line_;
	bool error_happen(const char* line);
};

} // namespace acl
