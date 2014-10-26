#include "acl_stdafx.hpp"
#include <assert.h>
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/db/query.hpp"

namespace acl
{

query::query()
: sql_buf_(NULL)
{
}

query::~query()
{
	if (sql_buf_)
		delete sql_buf_;
	reset();
}

query& query::create_sql(const char* sql_fmt, ...)
{
	va_list ap;
	va_start(ap, sql_fmt);
	sql_.vformat(sql_fmt, ap);
	va_end(ap);

	return *this;
}

bool query::append_key(string& buf, char* key)
{
	acl_lowercase(key);
	std::map<string, query_param*>::iterator it = params_.find(key);
	if (it == params_.end())
	{
		logger_warn("unknown key: %s", key);
		buf.append(key);
		return false;
	}

	query_param* param = it->second;
	switch (param->type)
	{
	case DB_PARAM_CHAR:
		buf.format_append("'%c'", param->v.c);
		break;
	case DB_PARAM_SHORT:
		buf.format_append("%d", param->v.s);
		break;
	case DB_PARAM_INT32:
		buf.format_append("%d", param->v.n);
		break;
	case DB_PARAM_INT64:
		buf.format_append("%lld", param->v.l);
		break;
	case DB_PARAM_STR:
		buf.format_append("'%s'",
			escape(param->v.S, param->dlen).c_str());
		break;
	default:
		logger_error("unknown type: %d", param->type);
		break;
	}

	return true;
}

const string& query::to_string()
{
	if (params_.empty())
		return sql_;
	if (sql_buf_ == NULL)
		sql_buf_ = NEW string(sql_.length() + 32);
	else
		sql_buf_->clear();

#define SKIP_WHILE(cond, ptr) { while(*ptr && (cond)) ptr++; }

	char last_ch;
	char* src = sql_.c_str(), *ptr, *key;
	while (*src != 0)
	{
		ptr = strchr(src, ':');
		if (ptr == NULL)
		{
			sql_buf_->append(src);
			break;
		}
		else if (*++ptr == 0)
		{
			sql_buf_->append(src);
			logger_warn("the last char is ':'");
			break;
		}

		sql_buf_->append(src, ptr - src - 1);
		key = ptr;

		SKIP_WHILE(*ptr != ',' && *ptr != ';'
			&& *ptr != ' ' && *ptr != '\t'
			&& *ptr != '(' && *ptr != ')'
			&& *ptr != '\r' && *ptr != '\n', ptr);
		if (ptr - key == 1)
		{
			logger_warn("only found: ':%c'", *ptr);
			sql_buf_->append(key, ptr - key + 1);
			src = ptr + 2;
			continue;
		}

		last_ch = *ptr;
		*ptr = 0;
		(void) append_key(*sql_buf_, key);
		*ptr = last_ch;

		if (last_ch == '\0')
			break;
		src = ptr;
	}

	return *sql_buf_;
}

const string& query::escape(const char* in, size_t len)
{
	buf_.clear();

	for (size_t i = 0; i < len; i++, in++)
	{
		switch (*in) {
		case 0:			/* Must be escaped for 'mysql' */
			buf_ += '\\';
			buf_ += '0';
			break;
		case '\n':		/* Must be escaped for logs */
			buf_ += '\\';
			buf_ += 'n';
			break;
		case '\r':
			buf_ += '\\';
			buf_ += 'r';
			break;
		case '\\':
			buf_ += '\\';
			buf_ += '\\';
			break;
		case '\'':
			buf_ += '\\';
			buf_ += '\'';
			break;
		case '"':		/* Better safe than sorry */
			buf_ += '\\';
			buf_ += '"';
			break;
		case '\032':		/* This gives problems on Win32 */
			buf_ += '\\';
			buf_ += 'Z';
			break;
		default:
			buf_ += *in;
			break;
		}
	}

	return buf_;
}

void query::del_param(const string& key)
{
	std::map<string, query_param*>::iterator it = params_.find(key);
	if (it != params_.end())
	{
		acl_myfree(it->second);
		params_.erase(it);
	}
}

query& query::set_parameter(const char* name, const char *value)
{
	string key(name);
	key.lower();
	del_param(key);

	size_t len = strlen(value);
	query_param* param = (query_param*)
		acl_mymalloc(sizeof(query_param) + len + 1);
	param->type = DB_PARAM_STR;
	memcpy(param->v.S, value, len);
	param->v.S[len] = 0;
	param->dlen = len;

	params_[key] = param;
	return *this;
}

query& query::set_parameter(const char* name, char value)
{
	string key(name);
	key.lower();
	del_param(key);

	size_t len = sizeof(query_param);
	query_param* param = (query_param*) acl_mymalloc(len);
	param->type = DB_PARAM_CHAR;
	param->v.c = value;
	param->dlen = sizeof(char);

	params_[key] = param;
	return *this;
}

query& query::set_parameter(const char* name, short value)
{
	string key(name);
	key.lower();
	del_param(key);

	size_t len = sizeof(query_param);
	query_param* param = (query_param*) acl_mymalloc(len);
	param->type = DB_PARAM_SHORT;
	param->v.s = value;
	param->dlen = sizeof(short);

	params_[key] = param;
	return *this;
}

query& query::set_parameter(const char* name, int value)
{
	string key(name);
	key.lower();
	del_param(key);

	size_t len = sizeof(query_param);
	query_param* param = (query_param*) acl_mymalloc(len);
	param->type = DB_PARAM_INT32;
	param->v.n = value;
	param->dlen = sizeof(int);

	params_[key] = param;
	return *this;
}

query& query::set_parameter(const char* name, acl_int64 value)
{
	string key(name);
	key.lower();
	del_param(key);

	size_t len = sizeof(query_param);
	query_param* param = (query_param*) acl_mymalloc(len);
	param->type = DB_PARAM_INT64;
	param->v.l = value;
	param->dlen = sizeof(long long int);

	params_[key] = param;
	return *this;
}

void query::reset()
{
	std::map<string, query_param*>::iterator it = params_.begin();
	for (; it != params_.end(); ++it)
		acl_myfree(it->second);
	params_.clear();
}

} // namespace acl
