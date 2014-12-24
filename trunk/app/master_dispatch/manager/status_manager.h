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
	 * ��õ�ǰ��������Ⱥ�����н�������״̬
	 * @param out {acl::string&} �洢������ݣ��ڲ�����������ӷ�ʽ����
	 *  ��������� out ԭʼ������
	 * @return {acl::string&}
	 */
	acl::string& get_status(acl::string& out);

	/**
	 * ����ĳ�����������ĵ�ǰ״̬����
	 * @param key {const char*} ��ʶĳ��������
	 * @param data {const char*} �÷������״̬����
	 */
	void set_status(const char* key, const char* data);

	/**
	 * �����ڵ��������
	 * @return {int} ���������������
	 */
	int check_timeout();

private:
	acl::locker lock_;
	std::map<acl::string, server_status*> servers_status_;
};
