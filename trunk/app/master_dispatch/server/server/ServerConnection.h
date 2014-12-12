#pragma once
#include "IConnection.h"

/**
 * 服务端连接对象
 */
class ServerConnection : public IConnection
{
public:
	ServerConnection(acl::aio_socket_stream* conn);
	~ServerConnection() {}

	/**
	 * 设置当前服务端连接的个数
	 * @param conns {unsigned int}
	 * @return {ServerConnection&}
	 */
	ServerConnection& set_conns(unsigned int nconns);

	/**
	 * 将当前服务器的客户端连接数加1
	 * @return {ServerConnection&}
	 */
	ServerConnection& inc_conns();

	/**
	 * 获得当前服务端连接的个数
	 * @return {unsigned int}
	 */
	unsigned int get_conns() const
	{
		return conns_;
	}

	ServerConnection& set_used(unsigned int n)
	{
		used_ = n;
		return *this;
	}

	unsigned int get_used() const
	{
		return used_;
	}

	ServerConnection& set_pid(pid_t pid)
	{
		pid_ = pid;
		return *this;
	}

	unsigned int get_pid() const
	{
		return pid_;
	}

	ServerConnection& set_max_threads(unsigned int n)
	{
		max_threads_ = n;
		return *this;
	}

	unsigned int get_max_threads() const
	{
		return max_threads_;
	}

	ServerConnection& set_curr_threads(unsigned int n)
	{
		curr_threads_ = n;
		return *this;
	}

	unsigned int get_curr_threads() const
	{
		return curr_threads_;
	}

	ServerConnection& set_busy_threads(unsigned int n)
	{
		busy_threads_ = n;
		return *this;
	}

	unsigned int get_busy_threads() const
	{
		return busy_threads_;
	}

	ServerConnection& set_qlen(unsigned int n)
	{
		qlen_ = n;
		return *this;
	}

	unsigned int get_qlen() const
	{
		return qlen_;
	}

	ServerConnection& set_type(const char* type)
	{
		type_ = type;
		return *this;
	}

	const acl::string& get_type() const
	{
		return type_;
	}

	/**
	 * 关闭服务端连接，当连接关闭时会触发 ServiceIOCallback 中的
	 * close_callback 过程，同时在 ServiceIOCallback 对象的析构过程
	 * 中会删除服务端本服务端连接对象
	 */
	void close();

protected:
	void run();

private:
	unsigned int conns_;
	unsigned int used_;
	pid_t pid_;
	acl::string type_;
	unsigned int max_threads_;
	unsigned int curr_threads_;
	unsigned int busy_threads_;
	unsigned int qlen_;
};
