#pragma once

//////////////////////////////////////////////////////////////////////////

class test_callback
{
public:
	test_callback() {}
	virtual ~test_callback() {}

	virtual void test_report(const char* msg, unsigned nstep) = 0;
	virtual void test_store(const char* dbpath) = 0;
	virtual void test_finish() = 0;
};

//////////////////////////////////////////////////////////////////////////

class test_all;

class ping_result : public ping_callback
{
public:
	ping_result(test_all& test) : test_(test) {}
	~ping_result() {}
protected:
	virtual void ping_report(size_t total, size_t curr, size_t nerr);
	virtual void ping_finish(const char* dbpath);
private:
	test_all& test_;
};

class nslookup_result : public nslookup_callback
{
public:
	nslookup_result(test_all& test) : test_(test) {}
	~nslookup_result() {}
protected:
	virtual void nslookup_report(size_t total, size_t curr);
	virtual void nslookup_finish(const char* dbpath);
private:
	test_all& test_;
};

class mail_result : public mail_callback
{
public:
	mail_result(test_all& test) : test_(test) {}
	~mail_result() {}
protected:
	virtual void mail_finish(const char* dbpath);
	virtual void mail_report(const char* msg, size_t total,
		size_t curr, const MAIL_METER& meter);
private:
	test_all& test_;
};
//////////////////////////////////////////////////////////////////////////

class test_all
{
public:
	test_all(test_callback*);
	void start();
protected:
	~test_all();
public:
	void ping_report(size_t total, size_t curr, size_t nerr);
	void ping_finish(const char* dbpath);

	void nslookup_report(size_t total, size_t curr);
	void nslookup_finish(const char* dbpath);

	void mail_finish(const char* dbpath);
	void mail_report(const char* msg, size_t total,
		size_t curr, const MAIL_METER& meter);
private:
	test_callback* callback_;
	void check_finish();
private:
	acl::string ip_file_;
	int ping_npkt_;
	int ping_delay_;
	int ping_timeout_;
	int ping_size_;
	bool ping_ok_;
	ping_result ping_result_;
private:
	acl::string domain_file_;
	acl::string dns_ip_;
	int dns_port_;
	int dns_timeout_;
	bool dns_ok_;
	nslookup_result ns_result_;
private:
	acl::string attach_;
	acl::string smtp_addr_;
	int smtp_port_;
	int conn_timeout_;
	int rw_timeout_;
	acl::string mail_user_;
	acl::string mail_pass_;
	acl::string recipients_;
	bool mail_ok_;
	mail_result mail_result_;

public:
	test_all& set_ip_file(const char* filename)
	{
		ip_file_ = filename;
		return *this;
	}
	test_all& set_ping_npkt(int ping_npkt)
	{
		ping_npkt_ = ping_npkt;
		return *this;
	}
	test_all& set_ping_delay(int ping_delay)
	{
		ping_delay_ = ping_delay;
		return *this;
	}
	test_all& set_ping_timeout(int ping_timeout)
	{
		ping_timeout_ = ping_timeout;
		return *this;
	}
	test_all& set_ping_size(int ping_size)
	{
		ping_size_ = ping_size;
		return *this;
	}
public:
	test_all& set_domain_file(const char* filename)
	{
		domain_file_ = filename;
		return *this;
	}
	test_all& set_dns_ip(const char* ip)
	{
		dns_ip_ = ip;
		return *this;
	}
	test_all& set_dns_port(int port)
	{
		dns_port_ = port;
		return *this;
	}
	test_all& set_dns_timeout(int timeout)
	{
		dns_timeout_ = timeout;
		return *this;
	}
public:
	test_all& set_attach(const char* attach)
	{
		attach_ = attach;
		return *this;
	}
	test_all& set_smtp_addr(const char* smtp_addr)
	{
		smtp_addr_ = smtp_addr;
		return *this;
	}
	test_all& set_smtp_port(int smtp_port)
	{
		smtp_port_ = smtp_port;
		return *this;
	}
	test_all& set_conn_timeout(int conn_timeout)
	{
		conn_timeout_ = conn_timeout;
		return *this;
	}
	test_all& set_rw_timeout(int rw_timeout)
	{
		rw_timeout_ = rw_timeout;
		return *this;
	}
	test_all& set_mail_user(const char* mail_user)
	{
		mail_user_ = mail_user;
		return *this;
	}
	test_all& set_mail_pass(const char* mail_pass)
	{
		mail_pass_ = mail_pass;
		return *this;
	}
	test_all& set_recipients(const char* recipients)
	{
		recipients_ = recipients;
		return *this;
	}
};