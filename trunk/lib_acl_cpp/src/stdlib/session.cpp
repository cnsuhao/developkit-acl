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

	// �������Լ��ٷ����ڴ�Ĵ���
	VBUF* buf = (VBUF*) acl_mymalloc(sizeof(VBUF) + len + 1);
	memcpy(buf->buf, str, len);
	// ���뱣֤β���� \0 �������������ַ�������ֵ
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

	// ���ô���ӿڣ����ԭ���� sid ����
	if (get_data(buf) == false)
	{
		// ���û���򴴽��µ� sid ����
		serialize(name, value, len, buf);
	}

	// �����л�
	else if (deserialize(buf) == false)
	{
		// XXX: ��������л�ʧ�ܣ��������µ�����
		serialize(name, value, len, buf);
	}

	// ������ڶ�Ӧ sid �����ݣ��������������ԭ��������
	else
	{
		// ����������Ѵ��ڣ�����Ҫ���ͷ�ԭ��������ֵ���������ֵ

		std::map<string, VBUF*>::iterator it = attrs_.find(name);
		if (it != attrs_.end())
			vbuf_free(it->second);
		attrs_[name] = vbuf_new(value, len);
		serialize(buf);  // ���л�����
	}

	// ���ô���ӿڣ��� memcached �����ƻ������������
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

	// ��ɾ�����ͷŶ�Ӧ�Ķ���
	vbuf_free(it->second);
	attrs_.erase(it);

	// ��� sid ���Ѿ�û�������ݣ���Ӧ�ý� sid ����� memcached ��ɾ��
	if (attrs_.empty())
	{
		// �����麯����ɾ���� sid ��Ӧ�Ļ�������
		if (del_data() == false)
		{
			logger_error("del sid(%s) error", sid_);
			return false;
		}
		return true;
	}

	// �� memcached ���������ʣ�������

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
	// �����麯����ɾ���������
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

// ���� handlersocket �ı��뷽ʽ

void session::serialize(string& buf)
{
	buf.clear(); // �����������

	std::map<string, VBUF*>::const_iterator it = attrs_.begin();
	if (it == attrs_.end())
		return;

	// ��ӵ�һ������
	const char ch = 1;
	escape(it->first.c_str(), it->first.length(), buf);
	escape(&ch, 1, buf);
	escape(it->second->buf, it->second->len, buf);
	++it;

	// ��Ӻ���������
	for (; it != attrs_.end(); ++it)
	{
		// ����һ������������Ķ���Ҫ��ӷָ���
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

// ���� handlersocket �Ľ��뷽ʽ

bool session::deserialize(string& buf)
{
	reset();  // ������ session ǰһ�β�ѯ״̬

	ACL_ARGV* tokens = acl_argv_split(buf.c_str(), "\t");
	ACL_ITER  iter;
	acl_foreach(iter, tokens)
	{
		char* ptr = (char*) iter.data;

		// �ظ�ʹ��ԭ�����ڴ�������Ϊ tokens ���Ѿ��洢���м�������
		buf.clear();
		if (unescape(ptr, strlen(ptr), buf) == false)
		{
			logger_error("unescape error");
			continue;
		}
		ptr = buf.c_str();
		// ��Ϊ acl::string �϶��ܱ�֤���������ݵ�β���� \0����������
		// strchr ʱ�����뵣��Խ�����⣬�� std::string ������֤����
		char* p1 = strchr(ptr, 1);
		if (p1 == NULL || *(p1 + 1) == 0)
			continue;
		*p1++ = 0;
		std::map<string, VBUF*>::iterator it = attrs_.find(ptr);

		// xxx: �Է����ظ�������
		if (it != attrs_.end())
			vbuf_free(it->second);
		attrs_[ptr] = vbuf_new(p1, buf.length() - (p1 - buf.c_str()));
	}

	acl_argv_free(tokens);
	return true;
}

} // namespace acl
