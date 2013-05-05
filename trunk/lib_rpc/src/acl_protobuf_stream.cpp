#include "stdafx.h"
#include "google/protobuf/io/acl_protobuf_stream.h"

namespace google {
namespace protobuf {
namespace io {

acl_protobuf_istream::acl_protobuf_istream(acl::istream* in)
: in_(in)
{

}

acl_protobuf_istream::~acl_protobuf_istream()
{

}

bool acl_protobuf_istream::Next(const void** data, int* size)
{
	return true;
}

void acl_protobuf_istream::BackUp(int count)
{

}

bool acl_protobuf_istream::Skip(int count)
{
	return true;
}

int64 acl_protobuf_istream::ByteCount() const
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////

acl_protobuf_ostream::acl_protobuf_ostream(acl::ostream* out)
: out_(out)
{

}

acl_protobuf_ostream::~acl_protobuf_ostream()
{

}

bool acl_protobuf_ostream::Next(void** data, int* size)
{
	return true;
}

void acl_protobuf_ostream::BackUp(int count)
{

}

int64 acl_protobuf_ostream::ByteCount() const
{
	return 0;
}

}  // namespace io
}  // namespace protobuf
}  // namespace google
