#include "acl_stdafx.hpp"
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/stdlib/dbuf_pool.hpp"
#include "acl_cpp/redis/redis_result.hpp"

namespace acl
{

redis_result::redis_result(dbuf_pool* pool)
	: pool_(pool)
	, size_(0)
	, idx_(0)
	, argv_(NULL)
	, lens_(NULL)
{
	if (pool_ == NULL)
	{
		pool_internal_ = NEW dbuf_pool();
		pool_ = pool_internal_;
	}
	else
		pool_internal_ = NULL;
}

redis_result::~redis_result()
{
	delete pool_internal_;
}

void redis_result::reset()
{
	size_ = 0;
	idx_  = 0;
	argv_ = NULL;
	lens_ = NULL;

	if (pool_ == pool_internal_)
	{
		delete pool_internal_;
		pool_internal_ = NEW dbuf_pool();
		pool_ = pool_internal_;
	}
}

redis_result& redis_result::set_size(size_t size)
{
	if (idx_ > 0)
	{
		logger_error("set size when putting, idx_: %d", (int) idx_);
		return *this;
	}
	size_ = size;
	return *this;
}

redis_result& redis_result::put(const char* buf, size_t len)
{
	if (size_ == 0)
	{
		logger_error("size_ is 0, call set_size first!");
		return *this;
	}
	if (idx_ >= size_)
	{
		logger_error("overflow, idx_(%d) >= size_(%d)",
			(int) idx_, (int) size_);
		return *this;
	}

	if (argv_ == NULL)
	{
		argv_ = (char**) pool_->dbuf_alloc(sizeof(char*) * size_);
		lens_ = (size_t*) pool_->dbuf_alloc(sizeof(size_t*) * size_);
	}

	argv_[idx_] = (char*) pool_->dbuf_memdup(buf, len + 1);
	argv_[idx_][len] = 0;
	lens_[idx_] = len;
	idx_++;

	return *this;
}

const char* redis_result::get(size_t i) const
{
	if (i >= idx_)
		return NULL;
	return argv_[i];
}

const char* redis_result::operator [](size_t i) const
{
	return get(i);
}

} // namespace acl
