#pragma once
#include "acl_cpp/acl_cpp_define.hpp"

namespace acl
{

class dbuf_pool;

class ACL_CPP_API redis_result
{
public:
	redis_result(dbuf_pool* pool);
	~redis_result();

	redis_result& set_size(size_t size);
	redis_result& put(const char* buf, size_t len);
	const char* get(size_t i) const;
	const char* operator[] (size_t i) const;

	const char** gets_argv() const
	{
		return (const char**) argv_;
	}

	size_t* get_lens() const
	{
		return lens_;
	}

	size_t get_size() const
	{
		return size_;
	}

	void reset();

private:
	dbuf_pool* pool_;
	dbuf_pool* pool_internal_;
	size_t  size_;
	size_t  idx_;
	char**  argv_;
	size_t* lens_;
};

} // namespace acl
