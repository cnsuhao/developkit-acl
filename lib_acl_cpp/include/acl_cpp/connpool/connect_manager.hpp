#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/stdlib/string.hpp"
#include <vector>

namespace acl
{

class connect_pool;

/**
 * connect pool ������������л�ȡ���ӳصȹ���
 */
class ACL_CPP_API connect_manager
{
public:
	connect_manager();
	virtual ~connect_manager();

	/**
	 * ��ʼ�����з����������ӳأ��ú����ڲ����� set �������ÿ����������ӳ�
	 * @param default_addr {const char*} ȱʡ�ķ�������ַ������ǿգ�
	 *  ���ڲ�ѯʱ����ʹ�ô˷�����
	 * @param addr_list {const char*} ���з������б�����Ϊ��
	 *  ��ʽ��IP:PORT:COUNT|IP:PORT:COUNT ...
	 *  �磺127.0.0.1:7777:50|192.168.1.1:7777:10|127.0.0.1:7778
	 * @param default_count {int} �� addr_list �зָ���ĳ������û��
	 *  COUNT ��Ϣʱ���ô�ֵ
	 *  ע��default_addr �� addr_list ����ͬʱΪ��
	 */
	void init(const char* default_addr, const char* addr_list,
		int default_count);

	/**
	 * ��ӷ������Ŀͻ������ӳ�
	 * @param addr {const char*} ��������ַ(ip:port)
	 * @param count {int} ���ӳ���������
	 * @return {connect_pool&} ��������ӵ����ӳض���
	 */
	connect_pool& set(const char* addr, int count);

	/**
	 * ���ĳ�������������ӳ�
	 * @param addr {const char*} redis ��������ַ(ip:port)
	 * @return {connect_pool*} ���ؿձ�ʾû�д˷���
	 */
	connect_pool* get(const char* addr);

	/**
	 * �ڶ�����ʱ�������ӳؼ�Ⱥ�л��һ�����ӳ�
	 * @return {connect_pool*} ����һ�����ӳأ����� NULL ��ʾ���������ʽ
	 *  �޷���ȡ��Ч���ӳأ�Ӧ����Ҫ����������ʽ
	 */
	connect_pool* peek();

	/**
	 * ������еķ����������ӳأ������ӳ��а���ȱʡ�ķ������ӳ�
	 * @return {std::vector<connect_pool*>&}
	 */
	std::vector<connect_pool*>& get_pools()
	{
		return pools_;
	}

	/**
	 * ���ȱʡ�ķ��������ӳ�
	 * @return {connect_pool*}
	 */
	connect_pool* get_default_pool()
	{
		return default_pool_;
	}

	/**
	 * ��ʼ����ʱ�����ö�ʱ���ᶨ�ڴ�ӡ��ǰ���з�������Ⱥ�ķ���ͳ��
	 * @param inter {int} ��ʱ����������
	 */
	void statistics_settimer(int inter = 1);

	/**
	 * ��ӡ��ǰ���� redis ���ӳصķ�����
	 */
	void statistics();

protected:
	/**
	 * ���麯�����������ʵ�ִ˺��������������ӳض���
	 * @param addr {const char*} ������������ַ����ʽ��ip:port
	 * @param count {int} ���ӳصĴ�С����
	 */
	virtual connect_pool* create_pool(const char* addr, int count) = 0;
private:
	static void statistics_record(int, void* ctx);
	void statistics_timer();
private:
	string default_addr_;			// ȱʡ�ķ����ַ
	connect_pool* default_pool_;		// ȱʡ�ķ������ӳ�
	std::vector<connect_pool*> pools_;	// ���еķ������ӳ�
	size_t service_size_;			// ����ס��ǰ�ܵķ�������Ⱥ����
	size_t service_idx_;			// ��һ��Ҫ���ʵĵ��±�ֵ
	locker lock_;				// ���� pools_ ʱ�Ļ�����
	int  stat_inter_;			// ͳ�Ʒ������Ķ�ʱ�����

	// ���ó�ȱʡ����֮��ķ�������Ⱥ
	void set_service_list(const char* addr_list, int count);
};

} // namespace acl
