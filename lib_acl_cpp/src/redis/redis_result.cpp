#include "acl_stdafx.hpp"
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/stdlib/dbuf_pool.hpp"
#include "acl_cpp/redis/redis_result.hpp"

namespace acl
{

redis_result::redis_result(dbuf_pool* pool)
: result_type_(REDIS_RESULT_NIL)
, pool_(pool)
, size_(0)
, idx_(0)
, argv_(NULL)
, lens_(NULL)
, children_(NULL)
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
	reset();
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

	if (children_ != NULL)
	{
		std::vector<redis_result*>::iterator it = children_->begin();
		for (; it != children_->end(); ++it)
			delete (*it);
		delete children_;
		children_ = NULL;
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

size_t redis_result::get_length() const
{
	if (lens_ == NULL)
		return 0;

	size_t len = 0;
	for (size_t i = 0; i < idx_; i++)
		len += lens_[i];
	return len;
}

size_t redis_result::argv_to_string(string& buf)
{
	if (idx_ == 0)
		return 0;

	size_t length = 0;
	for (size_t i = 0; i < idx_; i++)
	{
		buf.append(argv_[i], lens_[i]);
		length += lens_[i];
	}

	return length;
}

redis_result& redis_result::put(redis_result* rr)
{
	if (children_ == NULL)
		children_ = NEW std::vector<redis_result*>;
	children_->push_back(rr);
	return *this;
}

} // namespace acl
