#pragma once
#include <vector>

class message
{
public:
	message(const acl::string& server, acl::string* buf);
	~message(void);

	const acl::string& get_server() const
	{
		return server_;
	}

	const std::vector<acl::string*>& get_result() const
	{
		return lines_;
	}

	void add(const char* data, size_t len);

private:
	acl::string server_;
	std::vector<acl::string*> lines_;
	acl::string* last_buf_;
};

