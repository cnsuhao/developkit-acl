#pragma once

namespace acl
{

class connect_client
{
public:
	connect_client() : when_(0) {}
	virtual ~connect_client() {}

	/**
	 * 获得该连接对象最近一次被使用的时间截
	 * @return {time_t}
	 */
	time_t get_when()
	{
		return when_;
	}

	/**
	 * 设置该连接对象当前被使用的时间截
	 */
	void set_when(time_t when)
	{
		when_ = when;
	}

	/**
	 * 纯虚函数，子类必须实现此函数用于连接服务器
	 * @param addr {const char*} 服务器地址，格式：ip:port(domain:port)
	 * @return {bool} 是否连接成功
	 */
	virtual bool open(const char* addr) = 0;
private:
	time_t when_;
};

} // namespace acl
