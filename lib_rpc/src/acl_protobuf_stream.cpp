#include "stdafx.h"
#include "google/protobuf/io/acl_protobuf_stream.h"

namespace google {
namespace protobuf {
namespace io {

AclInputStream::AclInputStream(acl::istream* input, int block_size)
: copying_input_(input)
, impl_(&copying_input_, block_size)
{
}

AclInputStream::~AclInputStream()
{
}

bool AclInputStream::Next(const void** data, int* size)
{
	return impl_.Next(data, size);
}

void AclInputStream::BackUp(int count)
{
	impl_.BackUp(count);
}

bool AclInputStream::Skip(int count)
{
	return impl_.Skip(count);
}

int64 AclInputStream::ByteCount() const
{
	return impl_.ByteCount();
}

AclInputStream::CopyingAclInputStream::CopyingAclInputStream(acl::istream* input)
	: input_(input)
{
}

AclInputStream::CopyingAclInputStream::~CopyingAclInputStream()
{
}

int AclInputStream::CopyingAclInputStream::Read(void* buffer, int size)
{
	return input_->read(buffer, (size_t) size, false);
}
//////////////////////////////////////////////////////////////////////////

AclOutputStream::AclOutputStream(acl::ostream* output,
	int block_size /* = -1 */)
: copying_output_(output)
, impl_(&copying_output_, block_size)
{

}

AclOutputStream::~AclOutputStream()
{
	impl_.Flush();
}

void AclOutputStream::Flush()
{
	impl_.Flush();
}

bool AclOutputStream::Next(void** data, int* size)
{
	return impl_.Next(data, size);
}

void AclOutputStream::BackUp(int count)
{
	impl_.BackUp(count);
}

int64 AclOutputStream::ByteCount() const
{
	return impl_.ByteCount();
}

AclOutputStream::CopyingAclOutputStream::CopyingAclOutputStream(
	acl::ostream* output)
	: output_(output)
{
}

AclOutputStream::CopyingAclOutputStream::~CopyingAclOutputStream()
{
}

bool AclOutputStream::CopyingAclOutputStream::Write(
	const void* buffer, int size)
{
	return output_->write(buffer, (size_t) size) == size ? true : false;
}

}  // namespace io
}  // namespace protobuf
}  // namespace google
