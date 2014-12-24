#pragma once
#include <vector>

class server_status
{
public:
	server_status(const char* key, const char* data)
		: key_(key)
		, data_(data)
	{
	}
	~server_status() {}

	const char* get_data() const
	{
		return data_.c_str();
	}

private:
	time_t when_;
	acl::string key_;
	acl::string data_;
};

class status_manager : public acl::singleton<status_manager>
{
public:
	status_manager();
	~status_manager();

	/**
	 * 获得当前服务器集群的所有结点的运行状态
	 * @param out {acl::string&} 存储结果数据，内部采用数据添加方式，即
	 *  并不会清空 out 原始的数据
	 * @return {acl::string&}
	 */
	acl::string& get_status(acl::string& out);

	/**
	 * 设置某个服务器结点的当前状态数据
	 * @param key {const char*} 标识某个服务结点
	 * @param data {const char*} 该服务结点的状态数据
	 */
	void set_status(const char* key, const char* data);

	/**
	 * 将过期的数据清除
	 * @return {int} 被清除的数据数量
	 */
	int check_timeout();

private:
	acl::locker lock_;
	std::map<acl::string, server_status*> servers_status_;
};
