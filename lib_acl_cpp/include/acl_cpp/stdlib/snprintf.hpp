#pragma once
#include "acl_cpp/acl_cpp_define.hpp"

namespace acl {

#ifdef WIN32

#include <stdarg.h>

/**
 * 标准 C snprintf API 封装，同时兼容 WIN32 高版本 VC 下的安全模式，可以保证结果缓冲区
 * 中的最后一个字节为 '\0'
 * @param buf {char*} 存储结果的缓冲区
 * @param size {size_t} buf 缓冲区长度
 * @param fmt {const char*} 变参格式字符串
 * @return {int} 当缓冲区长度(即 size 值) 能够容纳所有数据且能保证尾部以 '\0' 结束时，
 *  则返回存储到缓冲区中的实际数据长度；否则返回 -1，同时缓冲区的最后一个字节被置 '\0'；
 *  需要注意该返回值的含义与 UNIX 下的不同
 */
ACL_CPP_API int ACL_CPP_PRINTF(3, 4) snprintf(char *buf, size_t size,
	const char *fmt, ...);

/**
 * 标准 C snprintf API 封装，同时兼容 WIN32 高版本 VC 下的安全模式，可以保证结果缓冲区
 * 中的最后一个字节为 '\0'
 * @param buf {char*} 存储结果的缓冲区
 * @param size {size_t} buf 缓冲区长度
 * @param fmt {const char*} 变参格式字符串
 * @param ap {va_list} 变参变量
 * @return {int} 当缓冲区长度(即 size 值) 能够容纳所有数据且能保证尾部以 '\0' 结束时，
 *  则返回存储到缓冲区中的实际数据长度；否则返回 -1，同时缓冲区的最后一个字节被置 '\0'；
 *  需要注意该返回值的含义与 UNIX 下的不同
 */
ACL_CPP_API int vsnprintf(char *buf, size_t size,
	const char *fmt, va_list ap);

#else  // UNIX/LINUX

/**
 * 标准 C snprintf API 封装，可以保证结果缓冲区中的最后一个字节为 '\0'
 * @param buf {char*} 存储结果的缓冲区
 * @param size {size_t} buf 缓冲区长度
 * @param fmt {const char*} 变参格式字符串
 * @return {int} 当缓冲区长度够用时返回实际拷贝的数据长度，否则返回实际需要的缓冲区长度，
 *  即当缓冲区长度不够时返回值 >= size；需要注意该返回值的含义与 WIN32 下的不同
 */
ACL_CPP_API int ACL_CPP_PRINTF(3, 4) snprintf(char *buf, size_t size,
	const char *fmt, ...);

/**
 * 标准 C snprintf API 封装，可以保证结果缓冲区中的最后一个字节为 '\0'
 * @param buf {char*} 存储结果的缓冲区
 * @param size {size_t} buf 缓冲区长度
 * @param fmt {const char*} 变参格式字符串
 * @param ap {va_list} 变参变量
 * @return {int} 当缓冲区长度够用时返回实际拷贝的数据长度，否则返回实际需要的缓冲区长度，
 *  即当缓冲区长度不够时返回值 >= size；需要注意该返回值的含义与 WIN32 下的不同
 */
ACL_CPP_API int vsnprintf(char *buf, size_t size,
	const char *fmt, va_list ap);

#endif

}  // namespace acl
