#include "acl_stdafx.hpp"
#include "mime_image.hpp"

namespace acl {

	const char* mime_image::get_location() const
	{
		return (header_value("Content-Location"));
	}

} // namespace acl
