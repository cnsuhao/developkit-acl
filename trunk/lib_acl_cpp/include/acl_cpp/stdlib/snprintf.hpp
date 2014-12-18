#pragma once
#include "acl_cpp/acl_cpp_define.hpp"

namespace acl {

#ifdef WIN32

#include <stdarg.h>

ACL_CPP_API int ACL_CPP_PRINTF(3, 4) snprintf(char *buf, size_t size,
	const char *fmt, ...);

ACL_CPP_API int vsnprintf(char *buf, size_t size,
	const char *fmt, va_list ap);

#else

ACL_CPP_API int ACL_CPP_PRINTF(3, 4) snprintf(char *buf, size_t size,
	const char *fmt, ...);

ACL_CPP_API int vsnprintf(char *buf, size_t size,
	const char *fmt, va_list ap);

#endif

}  // namespace acl
