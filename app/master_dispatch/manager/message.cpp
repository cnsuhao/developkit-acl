#include "stdafx.h"
#include "message.h"

message::message(const acl::string& server, acl::string* buf)
	: server_(server)
	, last_buf_(NULL)
{
	if (buf)
		lines_.push_back(buf);
}

message::~message(void)
{
	std::vector<acl::string*>::iterator it = lines_.begin();
	for (; it != lines_.end(); ++it)
		delete (*it);
}

void message::add(const char* data, size_t len)
{
	if (last_buf_ == NULL)
	{
		last_buf_ = new acl::string(1024);
		lines_.push_back(last_buf_);
	}

	acl_assert(last_buf_);
	const char* end = data + len - 1;

	while (data <= end)
	{
		const char* ln = strchr(data, '\n');
		if (ln == NULL)
		{
			last_buf_->append(data, len);
			return;
		}

		if (ln > data)
		{
			size_t n = ln - data + 1;
			last_buf_->append(data, n);
			len -= n;
		}

		data = ln + 1;

		last_buf_ = new acl::string(1024);
		lines_.push_back(last_buf_);
	}
}
