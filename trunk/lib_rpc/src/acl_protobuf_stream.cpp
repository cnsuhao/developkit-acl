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
	printf("%s:%d\r\n", __FUNCTION__, __LINE__);
	return impl_.Next(data, size);
}

void AclInputStream::BackUp(int count)
{
	printf("%s:%d\r\n", __FUNCTION__, __LINE__);
	impl_.BackUp(count);
}

bool AclInputStream::Skip(int count)
{
	printf("%s:%d\r\n", __FUNCTION__, __LINE__);
	return impl_.Skip(count);
}

int64 AclInputStream::ByteCount() const
{
	printf("%s:%d\r\n", __FUNCTION__, __LINE__);
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
	printf("%s:%d, size; %d\r\n", __FUNCTION__, __LINE__, size);
	int ret = input_->read(buffer, (size_t) size, false);
	printf(">>>read: %d\r\n", ret);
	return ret;
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

bool AclOutputStream::Flush()
{
	return impl_.Flush();
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
	printf("write called, size: %d\r\n", size);
	return output_->write(buffer, (size_t) size) == size ? true : false;
}

}  // namespace io
}  // namespace protobuf
}  // namespace google
