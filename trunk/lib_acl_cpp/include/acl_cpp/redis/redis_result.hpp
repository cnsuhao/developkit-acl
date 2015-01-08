#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <vector>

namespace acl
{

typedef enum
{
	REDIS_RESULT_NIL,
	REDIS_RESULT_ERROR,
	REDIS_RESULT_STATUS,
	REDIS_RESULT_INTEGER,
	REDIS_RESULT_STRING,
	REDIS_RESULT_ARRAY,
} redis_result_t;

class dbuf_pool;
class redis_client;

class ACL_CPP_API redis_result
{
public:
	redis_result(dbuf_pool* pool);
	~redis_result();

	static void *operator new(size_t size, dbuf_pool* pool);
	static void operator delete(void* ptr, dbuf_pool* pool);

	redis_result_t get_type() const
	{
		return result_type_;
	}

	size_t get_size() const
	{
		return size_;
	}

	const char* get(size_t i, size_t* len = NULL) const;
	const char** gets_argv() const
	{
		return (const char**) argv_;
	}
	size_t argv_to_string(string& buf) const;
	size_t* get_lens() const
	{
		return lens_;
	}
	size_t get_length() const;

	const std::vector<redis_result*>* get_children() const
	{
		return children_;
	}

	const redis_result* get_child(size_t i) const;

	dbuf_pool* get_pool()
	{
		return pool_;
	}

	void reset();

private:
	friend class redis_client;
	redis_result& set_type(redis_result_t type);
	redis_result& set_size(size_t size);
	redis_result& put(const char* buf, size_t len);
	redis_result& put(redis_result* rr);

private:
	redis_result_t result_type_;
	dbuf_pool* pool_;

	size_t  size_;
	size_t  idx_;
	char**  argv_;
	size_t* lens_;

	std::vector<redis_result*>* children_;
};

} // namespace acl
