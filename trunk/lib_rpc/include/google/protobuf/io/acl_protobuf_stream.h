#pragma once

#include <google/protobuf/io/zero_copy_stream.h>
#include "acl_cpp/stream/istream.hpp"
#include "acl_cpp/stream/ofstream.hpp"

namespace google {
namespace protobuf {
namespace io {

class LIBPROTOBUF_EXPORT acl_protobuf_istream : public ZeroCopyInputStream
{
public:
	acl_protobuf_istream(acl::istream* in);
	~acl_protobuf_istream();

	//基类 ZeroCopyInputStream 虚函数实现
	bool Next(const void** data, int* size);
	void BackUp(int count);
	bool Skip(int count);
	int64 ByteCount() const;
private:
	acl::istream* in_;
};

class LIBPROTOBUF_EXPORT acl_protobuf_ostream : public ZeroCopyOutputStream
{
public:
	acl_protobuf_ostream(acl::ostream* out);
	~acl_protobuf_ostream();

	//基类 ZeroCopyInputStream 虚函数实现
	bool Next(void** data, int* size);
	void BackUp(int count);
	int64 ByteCount() const;
private:
	acl::ostream* out_;
};

}  // namespace io
}  // namespace protobuf
}  // namespace google
