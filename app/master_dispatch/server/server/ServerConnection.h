#pragma once
#include "IConnection.h"

/**
 * ��������Ӷ���
 */
class ServerConnection : public IConnection
{
public:
	ServerConnection(acl::aio_socket_stream* conn);
	~ServerConnection() {}

	/**
	 * ���õ�ǰ��������ӵĸ���
	 * @param conns {unsigned int}
	 * @return {ServerConnection&}
	 */
	ServerConnection& set_conns(unsigned int nconns);

	/**
	 * ����ǰ�������Ŀͻ�����������1
	 * @return {ServerConnection&}
	 */
	ServerConnection& inc_conns();

	/**
	 * ��õ�ǰ��������ӵĸ���
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
	 * �رշ�������ӣ������ӹر�ʱ�ᴥ�� ServiceIOCallback �е�
	 * close_callback ���̣�ͬʱ�� ServiceIOCallback �������������
	 * �л�ɾ������˱���������Ӷ���
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
