#include "StdAfx.h"
#include "global/global.h"
#include "mime_builder.hpp"
#include "upload.h"

upload& upload::set_callback(upload_callback* c)
{
	callback_ = c;
	return *this;
}

upload& upload::set_dbpath(const char* p)
{
	dbpath_ = p;
	return *this;
}

upload::~upload()
{
	//if (!mailpath_.empty())
	//	_unlink(mailpath_.c_str());
}

upload& upload::set_server(const char* addr)
{
	smtp_addr_ = addr;
	return *this;
}

upload& upload::set_conn_timeout(int n)
{
	connect_timeout_ = n;
	return *this;
}

upload& upload::set_rw_timeout(int n)
{
	rw_timeout_ = n;
	return *this;
}

upload& upload::set_account(const char* s)
{
	auth_account_ = s;
	return *this;
}

upload& upload::set_passwd(const char* s)
{
	auth_passwd_ = s;
	return *this;
}

upload& upload::set_from(const char* s)
{
	mail_from_ = s;
	return *this;
}

upload& upload::add_to(const char* s)
{
	recipients_.push_back(s);
	return *this;
}

upload& upload::set_subject(const char* s)
{
	acl::charset_conv conv;
	acl::string buf;
	if (conv.convert("gbk", "utf-8", s, strlen(s), &buf) == false)
		logger_error("convert from gbk to utf-8 failed");
	else
	{
		acl::string buf2;
		conv.convert("utf-8", "gbk", buf.c_str(),
			(int) buf.length(), &buf2);
		printf(">>>buf: %s, buf2: %s\r\n", buf.c_str(), buf2.c_str());
	}
	//buf = s;
	acl::rfc2047::encode(s, (int) buf.length(), &subject_,
		"gbk", 'B', false);
	return *this;
}

//////////////////////////////////////////////////////////////////////////
// 主线程中运行

void upload::rpc_onover()
{
	delete this;
}

void upload::rpc_wakeup(void* ctx)
{

}

//////////////////////////////////////////////////////////////////////////
// 子线程中运行

void upload::rpc_run()
{
	// 创建邮件内容

	mime_builder builer;
	builer.primary_header()
		.set_from(mail_from_.c_str())
		.set_sender(mail_from_.c_str());

	std::list<acl::string>::const_iterator cit1 = recipients_.begin();
	for (; cit1 != recipients_.end(); ++cit1)
		builer.primary_header().add_to((*cit1).c_str());
	builer.primary_header().set_subject(subject_.c_str());

	acl::string body_text("error info");
	builer.set_body_text(body_text.c_str(), body_text.length());
	builer.add_file(dbpath_.c_str());

	mailpath_.format("%s/%ld.eml", global::get_instance().get_path(),
		time(NULL));
	if (builer.save_as(mailpath_.c_str()) == false)
	{
		logger_error("build email(%s) error(%s)",
			mailpath_.c_str(), acl::last_serror());
		mailpath_.clear();
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	// 远程连接 SMTP 服务器，将本地创建的邮件发送出去
	SMTP_CLIENT* conn = smtp_open(smtp_addr_.c_str(), connect_timeout_,
		rw_timeout_, 1024);
	if (conn == NULL)
	{
		logger_error("connect smtp server(%s) error", smtp_addr_);
		return;
	}
	else if (smtp_get_banner(conn) != 0)
	{
		logger_error("get smtpd banner error");
		smtp_close(conn);
		return;
	}
	else if (smtp_greet(conn, "localhost", 1) != 0)
	{
		logger_error("send EHLO error(%d:%s) to server %s",
			conn->smtp_code, conn->buf, smtp_addr_.c_str());
		smtp_close(conn);
		return;
	}
	else if (smtp_auth(conn, auth_account_.c_str(),
		auth_passwd_.c_str()) != 0)
	{
		logger_error("smtp auth error(%d:%s) from server(%s), "
			"account: %s, passwd: %s", conn->smtp_code, conn->buf,
			smtp_addr_.c_str(), auth_account_.c_str(),
			auth_passwd_.c_str());
		smtp_close(conn);
		return;
	}
	else if (smtp_mail(conn, mail_from_.c_str()) != 0)
	{
		logger_error("smtp MAIL FROM error(%d:%s), from: %s, server: %s",
			mail_from_.c_str(), conn->smtp_code,
			conn->buf, smtp_addr_.c_str());
		smtp_close(conn);
		return;
	}

	std::list<acl::string>::const_iterator cit = recipients_.begin();
	for (; cit != recipients_.end(); ++cit)
	{
		if (smtp_rcpt(conn, (*cit).c_str()) != 0)
		{
			logger_error("smtp RCPT TO error(%d:%s), to: %s, server: %s",
				conn->smtp_code, conn->buf, (*cit).c_str(),
				smtp_addr_.c_str());
			smtp_close(conn);
			return;
		}
	}

	if (smtp_data(conn) != 0)
	{
		logger_error("smtp DATA to server %s error(%d:%s)",
			smtp_addr_.c_str(), conn->smtp_code, conn->buf);
		smtp_close(conn);
		return;
	}

	// 发送邮件内容

	if (smtp_send_file(conn, mailpath_.c_str()) != 0)
	{
		logger_error("send email(%s) error(%d:%s)",
			mailpath_.c_str(), conn->smtp_code, conn->buf);
		smtp_close(conn);
		return;
	}

	/* 发送 \r\n.\r\n 表示邮件数据发送完毕 */
	if (smtp_data_end(conn) != 0)
	{
		logger_error("send . error: %s, code: %d\r\n",
			conn->buf, conn->smtp_code);
		smtp_close(conn);
	}
}