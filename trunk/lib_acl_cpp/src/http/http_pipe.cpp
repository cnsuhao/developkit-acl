#include "acl_stdafx.hpp"
#include "charset_conv.hpp"
#include "pipe_stream.hpp"
#include "http_pipe.hpp"

namespace acl
{

http_pipe::http_pipe()
: conv_(NULL)
{

}

http_pipe::~http_pipe()
{
	if (conv_)
		delete conv_;
}

void http_pipe::set_charset(charset_conv* conv)
{
	manager_.push_back(conv);
}

bool http_pipe::set_charset(const char* from, const char* to)
{
	if (from == NULL || to == NULL || strcasecmp(from, to) == 0)
		return false;
	if (conv_ == NULL)
		conv_ = NEW charset_conv();
	if (conv_->update_begin(from, to) == false)
	{
		delete conv_;
		conv_ = NULL;
		return false;
	}
	set_charset(conv_);
	return true;
}

void http_pipe::append(pipe_stream* ps)
{
	manager_.push_back(ps);
}

void http_pipe::reset()
{
	manager_.clear();
}

bool http_pipe::update(const char* in, size_t len)
{
	return manager_.update(in, len);
}

bool http_pipe::update_end()
{
	return manager_.update_end();
}

pipe_manager& http_pipe::get_manager()
{
	return manager_;
}

} // namespace acl
