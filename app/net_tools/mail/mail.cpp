#include "StdAfx.h"
#include "upload/mime_builder.hpp"
#include "global/global.h"
#include "global/util.h"
#include "rpc/rpc_manager.h"
#include "mail_store.h"
#include "mail.h"

mail::mail()
{
	memset(&meter_, 0, sizeof(meter_));
	meter_.smtp_nslookup_elapsed = 0.00;
	meter_.smtp_connect_elapsed = 0.00;
	meter_.smtp_envelope_eplased = 0.00;
	meter_.smtp_auth_elapsed = 0.00;
	meter_.smtp_data_elapsed = 0.00;
	meter_.smtp_total_elapsed = 0.00;
	meter_.pop3_nslookup_elapsed = 0.00;
	meter_.pop3_connect_elapsed = 0.00;
	meter_.smtp_auth_elapsed = 0.00;
	meter_.pop3_list_elapsed = 0.00;
	meter_.pop3_total_elapsed = 0.00;
}

mail::~mail()
{

}

mail& mail::set_callback(mail_callback* c)
{
	callback_ = c;
	return *this;
}

mail& mail::set_conn_timeout(int n)
{
	connect_timeout_ = n;
	return *this;
}

mail& mail::set_rw_timeout(int n)
{
	rw_timeout_ = n;
	return *this;
}

mail& mail::set_account(const char* s)
{
	auth_account_ = s;
	return *this;
}

mail& mail::set_passwd(const char* s)
{
	auth_passwd_ = s;
	return *this;
}

mail& mail::set_smtp(const char* addr, int port)
{
	smtp_addr_ = addr;
	smtp_port_ = port;
	smtp_ip_ = addr;
	return *this;
}

mail& mail::set_from(const char* s)
{
	mail_from_ = s;
	return *this;
}

mail& mail::add_to(const char* s)
{
	recipients_.push_back(s);
	return *this;
}

mail& mail::set_subject(const char* s)
{
	acl::string buf;
	//acl::charset_conv conv;
	//if (conv.convert("gbk", "utf-8", s, strlen(s), &buf) == false)
	//	logger_error("convert from gbk to utf-8 failed");
	//else
	//{
	//	acl::string buf2;
	//	conv.convert("utf-8", "gbk", buf.c_str(),
	//		(int) buf.length(), &buf2);
	//	printf(">>>buf: %s, buf2: %s\r\n", buf.c_str(), buf2.c_str());
	//}
	buf = s;
	acl::rfc2047::encode(s, (int) buf.length(), &subject_, "gbk", 'B', false);
	return *this;
}

mail& mail::add_file(const char* p)
{
	files_.push_back(p);
	return *this;
}

mail& mail::set_pop3(const char* addr, int port)
{
	pop3_addr_ = addr;
	pop3_port_ = port;
	pop3_ip_ = addr;
	return *this;
}

//////////////////////////////////////////////////////////////////////////

struct UP_CTX
{
	acl::string msg;
	size_t total;
	size_t curr;
};

//////////////////////////////////////////////////////////////////////////
// 主线程中运行

void mail::rpc_onover()
{
	mail_store* s = new mail_store(auth_account_.c_str(), smtp_ip_.c_str(),
		pop3_ip_.c_str(), meter_, *callback_);
	rpc_manager::get_instance().fork(s);
	delete this;
}

void mail::rpc_wakeup(void* ctx)
{
	UP_CTX* up = (UP_CTX*) ctx;

	callback_->mail_report(up->msg.c_str(),
		up->total, up->curr, meter_);
	delete up;
}

//////////////////////////////////////////////////////////////////////////
// 子线程中运行

void mail::rpc_run()
{
	test_smtp();
	test_pop3();
}

void mail::test_smtp()
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

	acl::string body_text("test");
	builer.set_body_text(body_text.c_str(), body_text.length());
	std::vector<acl::string>::const_iterator cit = files_.begin();
	for (; cit != files_.end(); ++cit)
		builer.add_file((*cit).c_str());

	mailpath_.format("%s/%ld.eml", global::get_instance().get_path(),
		time(NULL));
	if (builer.save_as(mailpath_.c_str()) == false)
	{
		logger_error("build email(%s) error(%s)",
			mailpath_.c_str(), acl::last_serror());
		mailpath_.clear();
		return;
	}

	acl::ifstream in;
	if (in.open_read(mailpath_.c_str()) == false)
	{
		logger_error("open %s error %s", mailpath_.c_str(),
			acl::last_serror());
		mailpath_.clear();
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	// 远程连接 SMTP 服务器，将本地创建的邮件发送出去
	UP_CTX* up;

	struct timeval begin, last, now;
	gettimeofday(&begin, NULL);
	gettimeofday(&last, NULL);

	ACL_DNS_DB* dns_db = acl_gethostbyname(smtp_addr_.c_str(), NULL);
	if (dns_db == NULL)
	{
		logger_error("gethostbyname(%s) failed", smtp_addr_.c_str());
		up = new UP_CTX;
		up->curr = 0;
		up->total = (size_t) in.fsize();
		up->msg.format("解析 smtp 域名：%s 失败！", smtp_addr_.c_str());
		rpc_signal(up);
		return;
	}
	const char* first_ip = acl_netdb_index_ip(dns_db, 0);
	if (first_ip == NULL || *first_ip == 0)
	{
		up = new UP_CTX;
		up->curr = 0;
		up->total = (size_t) in.fsize();
		up->msg.format("解析 smtp 域名2：%s 失败！", smtp_addr_.c_str());
		rpc_signal(up);
		logger_error("no ip for domain: %s", smtp_addr_.c_str());
		acl_netdb_free(dns_db);
		return;
	}
	smtp_ip_ = first_ip;

	gettimeofday(&now, NULL);
	meter_.smtp_nslookup_elapsed = util::stamp_sub(&now, &last);

	acl::string smtp_addr;
	smtp_addr.format("%s:%d", first_ip, smtp_port_);
	acl_netdb_free(dns_db);

	up = new UP_CTX;
	up->curr = 0;
	up->total = (size_t) in.fsize();
	up->msg.format("连接 SMTP 服务器 ...");
	rpc_signal(up);

	SMTP_CLIENT* conn = smtp_open(smtp_addr.c_str(), connect_timeout_,
		rw_timeout_, 1024);
	if (conn == NULL)
	{
		logger_error("connect smtp server(%s) error", smtp_addr);
		up = new UP_CTX;
		up->curr = 0;
		up->total = (size_t) in.fsize();
		up->msg.format("连接 smtp 服务器：%s 失败！", smtp_addr.c_str());
		rpc_signal(up);
		return;
	}

	gettimeofday(&now, NULL);
	meter_.smtp_connect_elapsed = util::stamp_sub(&now, &last);

	up = new UP_CTX;
	up->curr = 0;
	up->total = (size_t) in.fsize();
	up->msg.format("接收 SMTP 服务器欢迎信息(连接耗时 %.2f 毫秒) ...",
		meter_.smtp_connect_elapsed);
	rpc_signal(up);

	if (smtp_get_banner(conn) != 0)
	{
		logger_error("get smtpd banner error");
		smtp_close(conn);
		return;
	}
	else if (smtp_greet(conn, "localhost", 1) != 0)
	{
		logger_error("send EHLO error(%d:%s) to server %s",
			conn->smtp_code, conn->buf, smtp_addr.c_str());
		smtp_close(conn);
		return;
	}

	up = new UP_CTX;
	up->curr = 0;
	up->total = (size_t) in.fsize();
	up->msg.format("认证用户身份 ...");
	rpc_signal(up);

	gettimeofday(&last, NULL);

	if (smtp_auth(conn, auth_account_.c_str(),
		auth_passwd_.c_str()) != 0)
	{
		logger_error("smtp auth error(%d:%s) from server(%s), "
			"account: %s, passwd: %s", conn->smtp_code, conn->buf,
			smtp_addr.c_str(), auth_account_.c_str(),
			auth_passwd_.c_str());
		smtp_close(conn);
		return;
	}

	gettimeofday(&now, NULL);
	meter_.smtp_auth_elapsed = util::stamp_sub(&now, &last);

	up = new UP_CTX;
	up->curr = 0;
	up->total = (size_t) in.fsize();
	up->msg.format("发送邮件信封(认证耗时 %.2f 毫秒) ...", meter_.smtp_auth_elapsed);
	rpc_signal(up);

	if (smtp_mail(conn, mail_from_.c_str()) != 0)
	{
		logger_error("smtp MAIL FROM error(%d:%s), from: %s, server: %s",
			mail_from_.c_str(), conn->smtp_code,
			conn->buf, smtp_addr.c_str());
		smtp_close(conn);
		return;
	}

	std::list<acl::string>::const_iterator cit2 = recipients_.begin();
	for (; cit2 != recipients_.end(); ++cit2)
	{
		if (smtp_rcpt(conn, (*cit2).c_str()) != 0)
		{
			logger_error("smtp RCPT TO error(%d:%s), to: %s, server: %s",
				conn->smtp_code, conn->buf, (*cit2).c_str(),
				smtp_addr.c_str());
			smtp_close(conn);
			return;
		}
	}

	gettimeofday(&last, NULL);

	if (smtp_data(conn) != 0)
	{
		logger_error("smtp DATA to server %s error(%d:%s)",
			smtp_addr.c_str(), conn->smtp_code, conn->buf);
		smtp_close(conn);
		return;
	}
	gettimeofday(&now, NULL);
	meter_.smtp_envelope_eplased = util::stamp_sub(&now, &last);

	// 发送邮件内容

	char buf[8192];
	int  ret;
	int  n = 0;

	while (!in.eof())
	{
		ret = in.read(buf, sizeof(buf), false);
		if (ret == -1)
			break;
		if (acl_vstream_writen(conn->conn, buf, ret) == ACL_VSTREAM_EOF)
		{
			logger_error("smtp send data to server %s error(%d:%s)",
				smtp_addr.c_str(), conn->smtp_code, conn->buf);
			smtp_close(conn);
			return;
		}
		n += ret;
		up = new UP_CTX;
		up->curr = n;
		up->total = (size_t) in.fsize();
		up->msg.format("发送邮件中(%d/%d 字节) ...", up->curr, up->total);
		rpc_signal(up);
	}

	/* 发送 \r\n.\r\n 表示邮件数据发送完毕 */
	if (smtp_data_end(conn) != 0)
	{
		logger_error("send . error: %s, code: %d\r\n",
			conn->buf, conn->smtp_code);
		smtp_close(conn);
	}

	gettimeofday(&now, NULL);
	meter_.smtp_data_elapsed = util::stamp_sub(&now, &last);
	meter_.smtp_total_elapsed = util::stamp_sub(&now, &begin);

	up = new UP_CTX;
	up->curr = (size_t) in.fsize();
	up->total = (size_t) in.fsize();
	up->msg.format("发送邮件成功！(%d/%d 字节, 耗时 %.2f 毫秒)",
		up->curr, up->total, meter_.smtp_total_elapsed);
	rpc_signal(up);
}

void mail::test_pop3()
{

}