#pragma once
#include <list>

namespace acl
{

class connect_client;

/**
 * �ṩ����������ӳص�ӳ���ϵ
 */
class connect_pool
{
public:
	connect_pool(const char* addr, int count, int retry_inter = 1);
	virtual ~connect_pool();

	/**
	 * �Ӹ÷����������ӳ�̽���Ƿ���ã����ڳ��������������
	 * @return {connect_client*} ���Ϊ�����ʾ�÷��������ӳض��󲻿���
	 */
	connect_client* peek();

	/**
	 * �ͷ�һ�����ӣ�ͬʱ�� peek �д����Ļ������ͷ�
	 * @param conn {redis_client*}
	 * @param keep {bool} �Ƿ���Ը����ӱ��ֳ�����
	 */
	void put(connect_client* conn, bool keep = true);

	/**
	 * �������ӳصĴ��״̬
	 * @param ok {bool} ���ø������Ƿ�����
	 */
	void set_alive(bool ok /* true | false */);

	/**
	 * ��ȡ���ӳصķ�������ַ
	 * @return {const char*} ���طǿյ�ַ
	 */
	const char* get_addr() const
	{
		return addr_;
	}

	/**
	 * ��ȡ���ӳ��������������
	 * @return {int}
	 */
	int get_count() const
	{
		return count_;
	}

	/**
	 * ����ͳ�Ƽ�����
	 * @param inter {int} ͳ�Ƶ�ʱ����
	 */
	void reset_statistics(int inter)
	{
		time_t now = time(NULL);
		lock_.lock();
		if (now - last_ >= inter)
		{
			last_ = now;
			current_used_ = 0;
		}
		lock_.unlock();
	}

	/**
	 * ��ȡ�����ӳ��ܹ���ʹ�õĴ���
	 */
	unsigned long long get_total_used() const
	{
		return total_used_;
	}

	/**
	 * ��ȡ�����ӳص�ǰ��ʹ�ô���
	 * @return {unsigned long long}
	 */
	unsigned long long get_current_used() const
	{
		return current_used_;
	}

protected:
	virtual connect_client* create_connect() = 0;
private:
	// �Ƿ�������
	bool  alive_;
	// ������ķ������Ŀ������Ե�ʱ����
	int   retry_inter_;
	time_t last_dead_;
	char  addr_[64];
	int   max_;	// ���������
	int   count_;	// ��ǰ��������

	locker lock_;
	unsigned long long total_used_;		// �����ӳص����з�����
	unsigned long long current_used_;	// ĳʱ����ڵķ�����
	time_t last_;				// �ϴμ�¼��ʱ���
	std::list<connect_client*> pool_;	// ���ӳؼ���
};

} // namespace acl
