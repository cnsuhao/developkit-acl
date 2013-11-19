#pragma once
#include "acl_cpp/acl_cpp_define.hpp"

struct HTTP_HDR_RES;
struct HTTP_RES;
struct HTTP_HDR_REQ;
struct HTTP_REQ;

namespace acl {

class string;
class zlib_stream;
class socket_stream;
class ostream;
class istream;
class http_header;

/**
 * 该类的用处：1、当 HTTP 客户端向服务器请求数据时；2、当 HTTP 服务端接收
 * 到 HTTP 客户端连接时创建一个对应的 HTTP 客户端流对象
 * 该客户端流对象可以支持长连接
 */
class ACL_CPP_API http_client
{
public:

	/**
	 * 缺省的构造函数，使用此构造函数创建的 HTTP 客户端对象，需要显示地
	 * 调用 http_client::open 来打开数据流
	 */
	http_client(void);

	/**
	 * 根据已经连接成功的连接流对象创建 HTTP 客户端对象，但需要注意的是，
	 * 当该 http_client 对象销毁时，传入的 client 流对象并不会被销毁，需
	 * 要应用自己销毁，否则会造成资源泄露
	 * @param client {socket_stream*} HTTP 连接流对象，可以是请求端的流，
	 *  也可以是响应端的流；在本类对象被销毁时该流对象并不会被销毁，所以
	 *  用户需自行释放之
	 * @param rw_timeout {int} IO 读写超时时间(秒)
	 * @param is_request {bool} 是请求端还是响应端的客户端流
	 * @param unzip {bool} 当用来读取服务器的响应数据时，如果服务器返回的
	 *  数据体为压缩数据时，该参数控制在调用下面的函数时是否自动解压缩:
	 *  read_body(string&, bool, int*)
	 */
	http_client(socket_stream* client, int rw_timeout = 120,
		bool is_request = false, bool unzip = true);

	virtual ~http_client(void);

	/**
	 * 在支持长连接的多次请求中，可以手工调用此函数清除中间的数据对象，
	 * 当然这不是必须的，因为在多次调用 read_head 时，read_head 会自动
	 * 调用 reset 来清除上次请求过程中的是间对象
	 */
	void reset(void);

	/**
	 * 连接远程 HTTP 服务器
	 * @param addr {const char*} 服务器地址，格式：IP:PORT 或 DOMAIN:PORT
	 * @param conn_timeout {int} 连接超时时间(秒)
	 * @param rw_timeout {int} 读写超时时间(秒)
	 * @param unzip {bool} 当服务器返回的数据体为压缩数据时是否自动解压缩
	 * @param use_ssl {bool} 是否采用 SSL 加密传输方式
	 * @return {bool} 连接是否成功
	 */
	bool open(const char* addr, int conn_timeout = 60, int rw_timeout = 60,
		bool unzip = true, bool use_ssl = false);

	/**
	 * 写 HTTP 请求头数据至输出流中
	 * @param header {http_header&}
	 * @return {int} 真实写入的数据量, 返回 -1 表示出错
	 */
	int write(const http_header& header);

	/**
	 * 当调用 http_client(socket_stream*, bool) 构造函数创建
	 * 或用 http_client(void) 构建同时调用 open 打开数据流时
	 * 可以调用本函数获得输出数据流句柄
	 * @return {ostream&} 返回输出流的引用，如果该流并不存在，
	 *  则内部自动会产生断言，提示使用者应先将流打开
	 */
	ostream& get_ostream(void) const;

	/**
	 * 当调用 http_client(socket_stream*, bool) 构造函数创建
	 * 或用 http_client(void) 构建同时调用 open 打开数据流时
	 * 可以调用本函数获得输入数据流句柄
	 * @return {istream&} 返回输入流的引用，如果该流并不存在，
	 *  则内部自动会产生断言，提示使用者应先将流打开
	 */
	istream& get_istream(void) const;

	/**
	 * 当调用 http_client(socket_stream*, bool) 构造函数创建
	 * 或用 http_client(void) 构建同时调用 open 打开数据流时
	 * 可以调用本函数获得数据流句柄
	 * @return {socket_stream&} 返回流的引用，如果该流并不存在，
	 *  则内部自动会产生断言，提示使用者应先将流打开
	 */
	socket_stream& get_stream(void) const;

	/**
	 * 从 HTTP 服务器读取响应头数据或从 HTTP 客户端读取请求数据，
	 * 在长连接的多次请求中，后续的请求会自动清除上次的中间数据对象
	 * @return {bool} 是否成功
	 */
	bool read_head(void);

	/**
	 * 获得 HTTP 请求的数据体或响应的数据体长度
	 * @return {int64) 返回值若为 -1 则表明 HTTP 头不存在或没有长度字段
	 */
#ifdef WIN32
	__int64 body_length(void) const;
#else
	long long int body_length(void) const;
#endif

	/**
	 * HTTP 数据流(请求流或响应流是否允许保持长连接)
	 * @return {bool}
	 */
	bool keep_alive(void) const;

	/**
	 * 获得 HTTP 请求头或响应头中某个字段名的字段值
	 * @param name {const char*} 字段名
	 * @return {const char*} 字段值，为空时表示不存在
	 */
	const char* header_value(const char* name) const;

	/**
	 * 获得 HTTP 服务器返回的 HTTP 响应状态：
	 * 1xx, 2xx, 3xx, 4xx, 5xx
	 * @return {int} 若返回值为 -1 则表示出错，或该会话过程
	 *  不是向 HTTP 服务器请求数据过程
	 */
	int response_status(void) const;

	/**
	 * 获得 HTTP 客户端请求的 HOST 字段值
	 * @return {const char*} 返回 NULL 表示不存在该字段
	 */
	const char* request_host(void) const;

	/**
	 * 获得 HTTP 客户端请求的 PORT 端口号
	 * @return {int} 返回 -1 表示不存在
	 */
	int request_port(void) const;

	/**
	 * 获得 HTTP 客户端请求的 HTTP 方法：GET, POST, CONNECT
	 * @return {const char*} 返回值为空表示不存在
	 */
	const char* request_method(void) const;

	/**
	 * 获得 HTTP 客户端请求的 URL 中除去 HTTP://domain 后的内容
	 * 如：对于 http://test.com.cn/cgi-bin/test?name=value，则该
	 * 函数应该返回：/cgi-bin/test?name=value
	 * @return {const char*} 返回 NULL 表示不存在
	 */
	const char* request_url(void) const;

	/**
	 * 获得 HTTP 客户端请求的 URL 中的相对路径(不包含主机部分)，
	 * 如：对于 http://test.com.cn/cgi-bin/test?name=value，则该
	 * 函数应该返回：/path/test.cgi
	 * @return {const char*} 返回 NULL 表示不存在
	 */
	const char* request_path(void) const;

	/**
	 * 获得 HTTP 客户端请求的 URL 中的所有参数，如：
	 * http://test.com.cn/cgi-bin/test?name=value，则该函数应该返回：
	 * name=value
	 * @return {const char*} 返回 NULL 表示不存在
	 */
	const char* request_params(void) const;

	/**
	 * 获得 HTTP 客户端请求的 URL 中指定的参数值，如：
	 * http://test.com.cn/cgi-bin/test?name=value，则通过该函数可以
	 * 获得 name 参数的值为 value
	 * @param name {const char*} 参数名
	 * @return {const char*} 参数值，返回 NULL 表示不存在
	 */
	const char* request_param(const char* name) const;

	/**
	 * 获得 HTTP 客户端请求头中的 cookie 值
	 * @param name {const char*} cookie 名
	 * @return {const char*} cookie 值，返回 NULL 则表示不存在
	 */
	const char* request_cookie(const char* name) const;

	/**
	 * 从 HTTP 服务器读取响应体数据或从 HTTP 客户端读取请求体数据，
	 * 只有调用此函数才能进行解压操作
	 * @param out {string&} 存储数据体的缓冲区
	 * @param clean {bool} 在接收数据前是否自动清空 buf 缓冲区
	 * @param real_size {int*} 若该指针非空，则记录真正读到的数据长度，
	 *  通过该指针返回的数据值永远 >= 0
	 * @return {int} 返回值含义如下：
	 *  > 0: 表示已经读到的数据，并且数据还未读完
	 *  == 0: 如果返回值为此值，则必须调用 body_finish 函数来判断是否
	 *  已经读完 HTTP 响应体数据，如果已经读完，则同时表明还可以继续
	 *  保持长连接
	 *  < 0: 表示连接关闭
	 * 注：read_body 的两个函数不能混用；
	 *     当为解压缩数据时，则返回的值为解压缩后的数据长度
	 */
	int read_body(string& out, bool clean = true, int* real_size = NULL);
	
	/**
	 * 从 HTTP 服务器读取响应体数据或从 HTTP 客户端读取请求体数据，
	 * 该函数不能对数据进行解压
	 * @param buf {char*} 存储数据体的缓冲区，不能为空
	 * @param size {size_t} buf 缓冲区长度
	 * @return {int} 返回值含义如下：
	 *  > 0: 表示已经读到的数据，并且数据还未读完
	 *  == 0: 表示已经读完 HTTP 响应体数据，但连接并未关闭
	 *  < 0: 表示连接关闭
	 */
	int read_body(char* buf, size_t size);

	/**
	 * 判断是否已经读完 HTTP 响应数据体
	 * @return {bool}
	 */
	bool body_finish(void) const;

	/**
	 * 取得通过 read_head 读到的 HTTP 响应头对象，且当传入缓冲区
	 * 非空时，将 HTTP 响应头数据拷贝至缓冲区
	 * @param buf {string*} 非空时用来存储 HTTP 响应头数据
	 * @return {const HTTP_HDR_RES*} HTTP 响应头对象，如果为空，则说明
	 *  未读到响应头数据
	 */
	HTTP_HDR_RES* get_respond_head(string* buf);

	/**
	 * 取得通过 read_head 读到的 HTTP 请求头对象，且当传入缓冲区
	 * 非空时，将 HTTP 请求头数据拷贝至缓冲区
	 * @param buf {string*} 非空时用来存储 HTTP 请求头数据
	 * @return {const HTTP_HDR_REQ*} HTTP 请求头对象，如果为空，则说明
	 *  未读到请求头数据
	 */
	HTTP_HDR_REQ* get_request_head(string* buf);

	/**
	 * 输出服务器返回的 HTTP 响应头信息至标准输出
	 * @param prompt {const char*} 若非空则随同 HTTP 头信息一起输出
	 */
	void print_header(const char* prompt);

	/**
	 * 输出服务器返回的 HTTP 响应头信息至输出流中
	 * @param out {ostream&} 输出流，可以是文件流，也可以是网络流
	 * @param prompt {const char*} 若非空则随同 HTTP 头信息一起输出
	 */
	void fprint_header(ostream& out, const char* prompt);

private:
	socket_stream* stream_;     // HTTP 数据流
	bool stream_fixed_;         // 是否允许释放 stream_ 流对象

	HTTP_HDR_RES* hdr_res_;     // HTTP 头响应对象
	struct HTTP_RES* res_;      // HTTP 响应对象
	HTTP_HDR_REQ* hdr_req_;     // HTTP 头请求对象
	struct HTTP_REQ* req_;      // HTTP 请求对象
	int  rw_timeout_;           // IO 读写超时时间
	bool unzip_;                // 是否对压缩数据进行解压缩
	zlib_stream* zstream_;      // 解压对象
	bool is_request_;           // 是否是客户请求端
	int  gzip_header_left_;     // gzip 头剩余的长度
	int  last_ret_;             // 数据读完后记录最后的返回值
	bool body_finish_;          // 是否已经读完 HTTP 响应体数据

	bool read_request_head(void);
	bool read_response_head(void);
	int  read_request_body(char* buf, size_t size);
	int  read_response_body(char* buf, size_t size);
	int  read_request_body(string& out, bool clean, int* real_size);
	int  read_response_body(string& out, bool clean, int* real_size);
};

}  // namespace acl
