#include "acl_stdafx.hpp"
#include "acl_cpp/mime/mime_builder.hpp"

namespace acl
{

mime_builder::mime_builder()
{

}

mime_builder::~mime_builder()
{
	std::vector<char*>::iterator it = attachs_.begin();
	for (; it != attachs_.end(); ++it)
		acl_myfree(*it);
}

mime_builder& mime_builder::set_body_text(const char* src, size_t len)
{
	return *this;
}

mime_builder& mime_builder::set_body_html(const char* src, size_t len)
{
	return *this;
}

mime_builder& mime_builder::add_attach(const char* filepath)
{
	return *this;
}

} // namespace acl
