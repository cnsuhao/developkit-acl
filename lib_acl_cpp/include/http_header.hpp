#pragma once
#include "acl_cpp_define.hpp"
#include <list>

struct HTTP_HDR_RES;
struct HTTP_HDR_ENTRY;

namespace acl
{

	struct HTTP_PARAM 
	{
		char* name;
		char* value;
	};

	/**
	 * http 协议头中 cookie 对象类
	 */
	class ACL_CPP_API HttpCookie
	{
	public:
		/**
		 * 构造函数，该类对象必须动态创建，且必须调用 destroy 来释放类对象
		 * @param name {const char*} cookie 名，为非空字符串且字符串长度 > 0
		 * @param value {const char*} cookie 值，指针非空，字符串长度可以为 0
		 * 注：如果输入的两个参数不符合条件，内部将会产生断言
		 */
		HttpCookie(const char* name, const char* value);

		/**
		 * 当使用该构造函数时，可以使用 setCookie 来添加 cookie 项
		 */
		HttpCookie(void);

		/**
		 * 对于 Set-Cookie: xxx=xxx; domain=xxx; expires=xxx; path=xxx; max-age=xxx; ...
		 * 类的数据进行分析
		 * @param value {const char*} 类似于 xxx=xxx; domain=xxx; ... 内容
		 * @return {bool} 传入的数据是否合法
		 */
		bool setCookie(const char* value);

		/**
		 * 动态创建的类对象通过此函数释放
		 */
		void destroy();

		/**
		 * 设置 cookie 的作用域
		 * @param domain {const char*} cookie 作用域
		 * @return {HttpCookie&} 返回本对象的引用，便于用户连续操作
		 */
		HttpCookie& setDomain(const char* domain);

		/**
		 * 设置 cookie 的 path 字段
		 * @param path {const char*} path 字段值
		 * @return {HttpCookie&} 返回本对象的引用，便于用户连续操作
		 */
		HttpCookie& setPath(const char* path);

		/**
		 * 设置 cookie 的过期时间段，即用当前时间加输入的时间即为 cookie
		 * 的过期时间
		 * @param timeout {time_t} 过期时间值(单位为秒)，当前时间加该时间
		 * 即 cookie 的过期时间
		 * @return {HttpCookie&} 返回本对象的引用，便于用户连续操作
		 */
		HttpCookie& setExpires(time_t timeout);

		/**
		 * 设置 cookie 的过期时间截字符串
		 * @param expires {const char*} 过期时间截
		 * @return {HttpCookie&} 返回本对象的引用，便于用户连续操作
		 */
		HttpCookie& setExpires(const char* expires);

		/**
		 * 设置 cookie 的生存周期
		 * @param max_age {int} 生存秒数
		 * @return {HttpCookie&} 返回本对象的引用，便于用户连续操作
		 */
		HttpCookie& setMaxAge(int max_age);

		/**
		 * 添加与该 cookie 对象其它属性值
		 * @param name {const char*} 属性名
		 * @param value {const char*} 属性值
		 * @return {HttpCookie&} 返回本对象的引用，便于用户连续操作
		 */
		HttpCookie& add(const char* name, const char* value);

		/**
		 * 获得 cookie 名称，取决于构建函数输入值
		 * @return {const char*} 为长度大于 0 的字符串，永远非空指针
		 * 注：用户必须在调用 HttpCookie(const char*, const char*) 构造
		 *     或调用 setCookie(const char*) 成功后才可以调用该函数，
		 *     否则返回的数据是 "\0"
		 */
		const char* getName(void) const;

		/**
		 * 获得 cookie 值，取决于构造函数输入值
		 * @return {const char*} 非空指针，有可能是空字符串("\0")
		 */
		const char* getValue(void) const;

		/**
		 * 获得字符串格式的过期时间
		 * @return {const char*} 非空指针，返回值为 "\0" 表示不存在
		 */
		const char* getExpires(void) const;

		/**
		 * 获得 cookie 作用域
		 * @return {const char*} 非空指针，返回值为 "\0" 表示不存在
		 */
		const char* getDomain(void) const;

		/**
		 * 获得 cookie 的存储路径
		 * @return {const char*} 非空指针，返回值为 "\0" 表示不存在
		 */
		const char* getPath(void) const;

		/**
		 * 获得 cookie 的生存周期
		 * @return {int} 返回 -1 时表示没有该 Max-Age 字段
		 */
		int  getMaxAge(void) const;

		/**
		 * 获得对应参数名的参数值
		 * @param name {const char*} 参数名
		 * @param case_insensitive {bool} 是否区分大小写，true 表示
		 *  不区分大小写
		 * @return {const char*} 非空指针，返回值为 "\0" 表示不存在
		 */
		const char* getParam(const char* name,
			bool case_insensitive = true) const;

		/**
		 * 获得该 cookie 对象的除 cookie 名及 cookie 值之外的
		 * 所有属性及属性值
		 * @return {const std::list<HTTP_PARAM*>&}
		 */
		const std::list<HTTP_PARAM*>& getParams(void) const;
	private:
		char  dummy_[1];
		char* name_;
		char* value_;
		std::list<HTTP_PARAM*> params_;

		~HttpCookie(void);
		bool splitNameValue(char* data, HTTP_PARAM* param);
	};

	// HTTP 响应状态
	typedef enum
	{
		HTTP_OK,                // 一切正常
		HTTP_ERR_DNS,           // 域名解析失败
		HTTP_ERR_CONN,          // 连接服务器失败
		HTTP_ERR_REQ,           // 创建请求协议失败
		HTTP_ERR_READ,          // 读数据失败
		HTTP_ERR_SEND,          // 写数据失败
		HTTP_ERR_TIMO,          // 读写数据超时
		HTTP_ERR_READ_HDR,      // 读 HTTP 响应头失败
		HTTP_ERR_READ_BODY,     // 读 HTTP 响应体失败
		HTTP_ERR_INVALID_HDR,   // HTTP 响应头无效
		HTTP_ERR_UNKNOWN,       // 出现了未知错误
		HTTP_ERR_REDIRECT_MAX,	// HTTP 响应头中重定向次数太多
	} http_status_t;

	// HTTP 请求方法
	typedef enum
	{
		HTTP_METHOD_UNKNOWN,    // 未知方法
		HTTP_METHOD_GET,        // GET 方法
		HTTP_METHOD_POST,       // POST 方法
		HTTP_METHOD_PUT,        // PUT 方法
		HTTP_METHOD_CONNECT,    // CONNECT 方法
		HTTP_METHOD_PURGE       // PURGE 方法
	} http_method_t;

	class string;

	/**
	 * HTTP 头类，可以构建请求头或响应头
	 */
	class ACL_CPP_API http_header
	{
	public:
		http_header(void);

		/**
		 * HTTP 请求头构造函数
		 * @param url {const char*} 请求的 url，
		 * 该 url 不能包含 ? 以及 ? 后面的参数部分，如果想添加该 url 的参数，
		 * 应该通过调用 add_param 完成，url 格式
		 * 如: http://www.test.com/, /cgi-bin/test.cgi,
		 *     http://www.test.com/cgi-bin/test.cgi
		 * 不能为: http://www.test.com/cgi-bin/test.cgi?name=value 或
		 * /cgi-bin/test.cgi?name=value，因为其中的 name=value 参数
		 * 必须由 add_param 来添加
		 */
		http_header(const char* url);

		/**
		 * HTTP 响应头构造函数
		 * @param status {int} 状态字如：1xx, 2xx, 3xx, 4xx, 5xx
		 */
		http_header(int status);
		virtual ~http_header(void);

		/**
		 * 重置 HTTP 头信息同时将上次的临时资源释放
		 */
		void reset(void);

		//////////////////////////////////////////////////////////////////////////
		//            HTTP 请求与 HTTP 响应通用的方法函数
		//////////////////////////////////////////////////////////////////////////

		/**
		 * 设置 HTTP 头是客户端的请求头还是服务器的响应头
		 * @param onoff {bool} true 表示是请求头，否则表示响应头
		 * @return {http_header&} 返回本对象的引用，便于用户连续操作
		 */
		http_header& set_request_mode(bool onoff);

		/**
		 * 向 HTTP 头中添加字段
		 * @param name {const char*} 字段名
		 * @param value {const char*} 字段值
		 * @return {http_header&} 返回本对象的引用，便于用户连续操作
		 */
		http_header& add_entry(const char* name, const char* value);
		
		/**
		 * 设置 HTTP 头中的 Content-Length 字段
		 * @param n {int64} 设置值
		 * @return {http_header&} 返回本对象的引用，便于用户连续操作
		 */
#ifdef WIN32
		http_header& set_content_length(__int64 n);
#else
		http_header& set_content_length(long long int n);
#endif

		/**
		 * 设置 HTTP 头中的 Content-Type 字段
		 * @param value {const char*} 设置值
		 * @return {http_header&} 返回本对象的引用，便于用户连续操作
		 */
		http_header& set_content_type(const char* value);

		/**
		 * 设置 HTTP 头中的 Connection 字段，是否保持长连接
		 * 不过，目前并未真正支持长连接，即使设置了该标志位，
		 * 则得到响应数据后也会主动关闭连接
		 * @param on {bool} 是否保持长连接
		 * @return {http_header&} 返回本对象的引用，便于用户连续操作
		 */
		http_header& set_keep_alive(bool on);

		/**
		 * 向 HTTP 头中添加 cookie
		 * @param name {const char*} cookie 名
		 * @param value {const char*} cookie 值
		 * @param domain {const char*} 所属域
		 * @param path {const char*} 存储路径
		 * @param expires {time_t} 过期时间，当该值为 0 时表示不过期，
		 *  > 0 时，则从现在起再增加 expires 即为过期时间，单位为秒
		 * @return {http_header&} 返回本对象的引用，便于用户连续操作
		 */
		http_header& add_cookie(const char* name, const char* value,
			const char* domain = NULL, const char* path = NULL,
			time_t expires = 0);

		/**
		 * 向 HTTP 头中添加 cookie
		 * @param cookie {http_cookie*} 必须是动态分配的 cookie 对象
		 * @return {http_header&} 返回本对象的引用，便于用户连续操作
		 */
		http_header& add_cookie(HttpCookie* cookie);

		/**
		 * 将整型的日期转换为 rfc1123 字符串格式的日期
		 */
		static void date_format(char* out, size_t size, time_t t);

		/**
		 * 判断是否是 HTTP 请求头
		 * @return {bool} 返回 false 表明是 HTTP 响应头
		 */
		bool is_request(void) const;

		//////////////////////////////////////////////////////////////////////////
		//            HTTP 请求方法函数
		//////////////////////////////////////////////////////////////////////////
		
		/**
		 * 创建 HTTP 请求头数据
		 * @param buf {string&} 存储结果数据
		 * @return {bool} 创建请求头中否成功
		 */
		bool build_request(string& buf) const;

		/**
		 * 设置请求的 URL，该 URL 不能包含 ? 以及 ? 后面的参数部分，
		 * 如果想添加该 URL 的参数，应该通过调用 add_param 完成，url 格式
		 * 如: http://www.test.com/, /cgi-bin/test.cgi,
		 *     http://www.test.com/cgi-bin/test.cgi
		 * 不能为: http://www.test.com/cgi-bin/test.cgi?name=value 或
		 * /cgi-bin/test.cgi?name=value，因为其中的 name=value 参数必须由 add_param
		 * 来添加
		 * @param url {const char*} 请求的 url
		 * @return {http_header&} 返回本对象的引用，便于用户连续操作
		 */
		http_header& set_url(const char* url);

		/**
		 * 设置 HTTP 请求头的 HOST 字段
		 * @param value {const char*} 请求头的 HOST 字段值
		 * @return {http_header&} 返回本对象的引用，便于用户连续操作
		 */
		http_header& set_host(const char* value);

		/**
		 * 设置 HTTP 协议的请求方法，如果不调用此函数，则默认用 GET 方法
		 * @param method {http_method_t} HTTP 请求方法
		 * @return {http_header&} 返回本对象的引用，便于用户连续操作
		 */
		http_header& set_method(http_method_t method);

		/**
		 * 设置 HTTP 请求头中 Range 字段，用于分段请求数据，多用于
		 * 支持断点续传的 WEB 服务器中
		 * @param from {http_off_t} 起始偏移位置，下标从 0 开始，该
		 *  值当 >= 0 时才有效
		 * @param to {http_off_t} 请求结束偏移位置，下标从 0 开始，
		 *  当该值输入 < 0 时，则认为是请求从起始位置开始至最终长度
		 *  位置
		 * @return {http_header&} 返回本对象的引用，便于用户连续操作
		 */
#ifdef WIN32
		http_header& set_range(__int64 from, __int64 to);
#else
		http_header& set_range(long long int from, long long int to);
#endif

		/**
		 * 获得由 set_range 设置的分段请求位置值
		 * @param from {http_off_t*} 非空时存储起始位置偏移
		 * @param to {http_off_t*} 非空时存储结束位置偏移
		 */
#ifdef WIN32
		void get_range(__int64* from, __int64* to);
#else
		void get_range(long long int* from, long long int* to);
#endif

		/**
		 * 设置 HTTP 请求头中是否允许接收压缩数据，对应的 HTTP 头字段为：
		 * Accept-Encoding: gzip, deflate，但目前仅支持 gzip 格式
		 * @param on {bool} 如果为 true 则自动添加 HTTP 压缩头请求
		 * @return {http_header&} 返回本对象的引用，便于用户连续操作
		 */
		http_header& accept_gzip(bool on);

		/**
		 * 向请求的 URL 中添加参数对
		 * @param name {const char*} 参数名
		 * @param value {const char*} 参数值
		 * @return {http_header&} 返回本对象的引用，便于用户连续操作
		 */
		http_header& add_param(const char* name, const char* value);

		/**
		 * url 重定向
		 * @param url {const char*} 重定向的 URL，格式为：
		 *  http://xxx.xxx.xxx/xxx 或 /xxx
		 *  如果是前者，则自动从中取出 HOST 字段，如果是后者，则
		 *  延用之前的 HOST
		 */
		bool redirect(const char* url);

		/**
		 * 设置重定向次数，如果该值 == 0 则不主动进行重定向，否则
		 * 进行重定向且重定向的次数由该值决定
		 * @param n {int} 允许重定向的次数
		 * @return {http_header&} 返回本对象的引用，便于用户连续操作
		 */
		http_header& set_redirect(unsigned int n = 5);

		/**
		 * 获取通过 set_redirect 设置的允许的最大重定向次数
		 * @return {unsigned int}
		 */
		unsigned int get_redirect(void) const;

		/**
		 * 当需要重定向时，会主动调用此函数允许子类做一些重置工作
		 */
		virtual void redicrect_reset(void) {}

		//////////////////////////////////////////////////////////////////////////
		//            HTTP 响应方法函数
		//////////////////////////////////////////////////////////////////////////

		/**
		 * 创建 HTTP 响应头数据
		 * @param buf {string&} 存储结果数据
		 * @return {bool} 创建响应头中否成功
		 */
		bool build_response(string& buf) const;

		/**
		 * 设置 HTTP 响应头中的响应状态字
		 * @param status {int} 状态字如：1xx, 2xx, 3xx, 4xx, 5xx
		 * @return {http_header&} 返回本对象的引用，便于用户连续操作
		 */
		http_header& set_status(int status);

		/**
		 * 设置是否用来生成 CGI 格式的响应头
		 * @param on {bool} 是否 CGI 格式响应头
		 * @return {http_header&} 返回本对象的引用，便于用户连续操作
		 */
		http_header& set_cgi_mode(bool on);

	protected:
	private:
		//char* domain_;  // HTTP 服务器域名
		unsigned short port_;                 // HTTP 服务器端口
		char* url_;                           // HTTP 请求的 URL
		std::list<HTTP_PARAM*> params_;       // 请求参数集合
		std::list<HttpCookie*> cookies_;      // cookies 集合
		std::list<HTTP_HDR_ENTRY*> entries_;  // HTTP 请求头中各字段集合
		http_method_t method_;                // HTTP 请求的方法
		bool keep_alive_;                     // 是否保持长连接
		unsigned int nredirect_;              // 最大重定向的次数限制
		bool accept_compress_;                // 是否接收压缩数据
		int  status_;                         // 响应头的状态字
		bool is_request_;                     // 是请求头还是响应头
		bool cgi_mode_;                       // 是否 CGI 响应头
#ifdef WIN32
		__int64 range_from_;                  // 请求头中，range 起始位置
		__int64 range_to_;                    // 请求头中，range 结束位置
#else
		long long int range_from_;            // 请求头中，range 起始位置
		long long int range_to_;              // 请求头中，range 结束位置
#endif

		void init(void);                      // 初始化
		void clear(void);
		void build_common(string& buf) const; // 构建通用头
	};

}
