#include "acl_stdafx.hpp"
#include "log.hpp"
#include "string.hpp"
#include "escape.hpp"
#include "session.hpp"

namespace acl
{

VBUF* session::vbuf_new(const void* str, size_t len)
{
	acl_assert(len > 0);

	// 这样可以减少分配内存的次数
	VBUF* buf = (VBUF*) acl_mymalloc(sizeof(VBUF) + len + 1);
	memcpy(buf->buf, str, len);
	// 必须保证尾部以 \0 结束以允许返回字符串属性值
	buf->buf[len] = 0;
	buf->len = len;
	return buf;
}

void session::vbuf_free(VBUF* buf)
{
	acl_myfree(buf);
}

session::session(time_t ttl /* = 0 */)
: ttl_(ttl)
{
	struct timeval tv;

	(void) gettimeofday(&tv, NULL);
	snprintf(sid_, sizeof(sid_), "acl.%d.%d.%d", (int) tv.tv_sec,
		(int) tv.tv_usec, rand());
}

session::~session()
{
	reset();
}

const char* session::get_sid() const
{
	return sid_;
}

void session::set_sid(const char* sid)
{
	ACL_SAFE_STRNCPY(sid_, sid, sizeof(sid_));
}

void session::reset()
{
	std::map<string, VBUF*>::iterator it = attrs_.begin();
	for (; it != attrs_.end(); ++it)
		vbuf_free(it->second);
	attrs_.clear();
}

bool session::set(const char* name, const char* value)
{
	return set(name, value, strlen(value));
}

bool session::set(const char* name, const void* value, size_t len)
{
	string buf;

	// 调用纯虚接口，获得原来的 sid 数据
	if (get_data(buf) == false)
	{
		// 如果没有则创建新的 sid 数据
		serialize(name, value, len, buf);
	}

	// 反序列化
	else if (deserialize(buf) == false)
	{
		// XXX: 如果反序列化失败，则生成新的数据
		serialize(name, value, len, buf);
	}

	// 如果存在对应 sid 的数据，则将新数据添加在原来数据中
	else
	{
		// 如果该属性已存在，则需要先释放原来的属性值后再添加新值

		std::map<string, VBUF*>::iterator it = attrs_.find(name);
		if (it != attrs_.end())
			vbuf_free(it->second);
		attrs_[name] = vbuf_new(value, len);
		serialize(buf);  // 序列化数据
	}

	// 调用纯虚接口，向 memcached 或类似缓存中添加数据
	if (set_data(buf.c_str(), buf.length(), ttl_) == false)
	{
		logger_error("set cache error, sid(%s)", sid_);
		return false;
	}
	return true;
}

const char* session::get(const char* name)
{
	const VBUF* bf = get_vbuf(name);
	if (bf == NULL)
		return NULL;
	return bf->buf;
}

const VBUF* session::get_vbuf(const char* name)
{
	string buf;
	if (get_data(buf) == false)
		return NULL;
	if (deserialize(buf) == false)
		return NULL;

	std::map<string, VBUF*>::const_iterator cit = attrs_.find(name);
	if (cit == attrs_.end())
		return NULL;
	return cit->second;
}

time_t session::get_ttl() const
{
	return ttl_;
}

bool session::del(const char* name)
{
	string buf;
	if (get_data(buf) == false)
		return true;
	if (deserialize(buf) == false)
		return false;

	std::map<string, VBUF*>::iterator it = attrs_.find(name);
	if (it == attrs_.end())
		return false;

	// 先删除并释放对应的对象
	vbuf_free(it->second);
	attrs_.erase(it);

	// 如果 sid 中已经没有了数据，则应该将 sid 对象从 memcached 中删除
	if (attrs_.empty())
	{
		// 调用虚函数，删除该 sid 对应的缓存内容
		if (del_data() == false)
		{
			logger_error("del sid(%s) error", sid_);
			return false;
		}
		return true;
	}

	// 向 memcached 中重新添加剩余的数据

	serialize(buf);
	if (set_data(buf.c_str(), buf.length(), ttl_) == false)
	{
		logger_error("set cache error, sid(%s)", sid_);
		return false;
	}
	return true;
}

bool session::remove()
{
	// 调用虚函数，删除缓存对象
	if (del_data() == false)
	{
		logger_error("invalidate sid(%s) error", sid_);
		return false;
	}
	return true;
}

bool session::set_ttl(time_t ttl)
{
	if (set_timeout(ttl) == true)
	{
		ttl_ = ttl;
		return true;
	}
	else
		return false;
}

// 采用 handlersocket 的编码方式

void session::serialize(string& buf)
{
	buf.clear(); // 先清除缓冲区

	std::map<string, VBUF*>::const_iterator it = attrs_.begin();
	if (it == attrs_.end())
		return;

	// 添加第一个属性
	const char ch = 1;
	escape(it->first.c_str(), it->first.length(), buf);
	escape(&ch, 1, buf);
	escape(it->second->buf, it->second->len, buf);
	++it;

	// 添加后续的属性
	for (; it != attrs_.end(); ++it)
	{
		// 除第一个属性外后续的都需要添加分隔符
		buf << '\t';
		escape(it->first.c_str(), it->first.length(), buf);
		escape(&ch, 1, buf);
		escape(it->second->buf, it->second->len, buf);
	}
}

void session::serialize(const char* name, const void* value,
	size_t len, string& buf)
{
	escape(name, strlen(name), buf);
	const char ch = 1;
	escape(&ch, 1, buf);
	escape((const char*) value, len, buf);
}

// 采用 handlersocket 的解码方式

bool session::deserialize(string& buf)
{
	reset();  // 先重置 session 前一次查询状态

	ACL_ARGV* tokens = acl_argv_split(buf.c_str(), "\t");
	ACL_ITER  iter;
	acl_foreach(iter, tokens)
	{
		char* ptr = (char*) iter.data;

		// 重复使用原来的内存区，因为 tokens 中已经存储了中间结果数据
		buf.clear();
		if (unescape(ptr, strlen(ptr), buf) == false)
		{
			logger_error("unescape error");
			continue;
		}
		ptr = buf.c_str();
		// 因为 acl::string 肯定能保证缓冲区数据的尾部有 \0，所以在用
		// strchr 时不必须担心越界问题，但 std::string 并不保证这样
		char* p1 = strchr(ptr, 1);
		if (p1 == NULL || *(p1 + 1) == 0)
			continue;
		*p1++ = 0;
		std::map<string, VBUF*>::iterator it = attrs_.find(ptr);

		// xxx: 以防有重复的属性
		if (it != attrs_.end())
			vbuf_free(it->second);
		attrs_[ptr] = vbuf_new(p1, buf.length() - (p1 - buf.c_str()));
	}

	acl_argv_free(tokens);
	return true;
}

} // namespace acl
