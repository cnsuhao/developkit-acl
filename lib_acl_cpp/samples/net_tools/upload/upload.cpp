#include "StdAfx.h"
#include "mime_builder.hpp"
#include "upload.h"

upload::upload(upload_callback* callback, const char* dbpath,
	const char* smtpAddr, int connectTimeout,
	int rwTimeout)
	: callback_(callback)
	, dbpath_(dbpath)
	, smtp_addr_(smtpAddr)
	, connect_timeout_(connectTimeout)
	, rw_timeout_(rwTimeout)
{

}

upload::~upload()
{
	if (!mailpath_.empty())
		_unlink(mailpath_.c_str());
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
	mime_builder builer;
	builer.primary_header()
		.set_from(mail_from_)
		.set_sender(mail_from_);

	std::list<acl::string>::const_iterator cit1 = recipients_.begin();
	for (; cit1 != recipients_.end(); ++cit1)
			builer.primary_header().add_to((*cit1).c_str());
	builer.primary_header().set_subject(subject_);

	acl::string body_text("error info");
	builer.set_body_text(body_text.c_str(), body_text.length());
	builer.add_file(dbpath_.c_str());

	mailpath_.format("%s/%ld.eml", acl_process_path(), time(NULL));
	if (builer.save_as(mailpath_.c_str()) == false)
	{
		logger_error("build email(%s) error(%s)",
			mailpath_.c_str(), acl::last_serror());
		mailpath_.clear();
		return;
	}
	//////////////////////////////////////////////////////////////////////////

	SMTP_CLIENT* conn = smtp_open(smtp_addr_, connect_timeout_, 1024);
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
		logger_error("send EHLO error to server %s", smtp_addr_);
		smtp_close(conn);
		return;
	}
	else if (smtp_auth(conn, auth_account_, auth_passwd_) != 0)
	{
		logger_error("smtp auth failed from server(%s), account: %s, passwd: %s",
			smtp_addr_, auth_account_, auth_passwd_);
		smtp_close(conn);
		return;
	}
	else if (smtp_mail(conn, mail_from_) != 0)
	{
		logger_error("smtp MAIL FROM error, from: %s, server: %s",
			mail_from_, smtp_addr_);
		smtp_close(conn);
		return;
	}

	std::list<acl::string>::const_iterator cit = recipients_.begin();
	for (; cit != recipients_.end(); ++cit)
	{
		if (smtp_rcpt(conn, *cit) != 0)
		{
			logger_error("smtp RCPT TO error, to: %s, server: %s",
				*cit, smtp_addr_);
			smtp_close(conn);
			return;
		}
	}

	if (smtp_data(conn) != 0)
	{
		logger_error("smtp DATA to server %s error", smtp_addr_);
		smtp_close(conn);
		return;
	}

	// 发送邮件内容
	acl::ifstream in;
	if (in.open_read(mailpath_.c_str()) == false)
	{
		logger_error("open email file(%s) error(%s)",
			mailpath_.c_str(), acl::last_serror());
		smtp_close(conn);
		return;
	}

	if (smtp_send_file(conn, mailpath_.c_str()) <= 0)
	{
		logger_error("send email(%s) error(%s)",
			mailpath_.c_str(), acl::last_serror());
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