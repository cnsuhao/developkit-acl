#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <list>

namespace acl {

class db_handle;
class locker;

class db_pool
{
public:
	/**
	 * ���ݿ⹹�캯��
	 * @param dblimit {int} ���ݿ����ӳص��������������
	 * @param idle {int} ���ӳ��п������ӵ����Ӵ��ʱ�䣬����ֵ
	 *  Ϊ -1 ʱ����ʾ������������ӣ�Ϊ 0 ʱ��ʾ�ڲ��������κγ�����
	 */
	db_pool(int dblimit = 64, int idle = -1);
	virtual ~db_pool();

	/**
	 * �����ݿ������ӳػ��һ�����ݿ����ӣ��ú������ص����ݿ�
	 * ���Ӷ�������������� db_pool->put(db_handle*) ������
	 * �黹�����ݿ����ӳأ��ɸú�����õ����Ӿ������ delete��
	 * �����������ӳص��ڲ�����������
	 * @return {db_handle*} ���ؿգ����ʾ����
	 */
	db_handle* peek();

	/**
	 * �����ݿ����ӷŻ������ӳ��У��������ݿ����ӳ��л������
	 * ��������Ӧ��ͨ���ú����Żأ�����ֱ�� delete����Ϊ����
	 * �ᵼ�����ӳص��ڲ�������������
	 * @param conn {db_handle*} ���ݿ����Ӿ���������Ӿ������
	 *  ���� peek �����ģ�Ҳ���Ե�����̬������
	 * @param keep {bool} �黹�����ӳص����ݿ����Ӿ���Ƿ����
	 *  �������ӣ���������ڲ����Զ�ɾ�������Ӿ��
	 */
	void put(db_handle* conn, bool keep = true);

	/**
	 * �����ݿ����ӳ��еĹ��������ͷŵ����Լ��ٶԺ�����ݿ��������
	 * @param ttl {time_t} �����ݿ����ӵĿ���ʱ����ڵ��ڴ�ֵʱ
	 *  �����ӽ����ͷţ��� idle Ϊ 0 ʱ����Ҫ�ͷ����е����ݿ�����;
	 *  ��Ϊ -1 ʱ�򲻽����ͷ����Ӳ���
	 * @param exclusive {bool} �ڲ��Ƿ���Ҫ�ӻ�����
	 * @return {int} ���ͷŵ����ݿ����ӵĸ���(>= 0)
	 */
	int dbidle_erase(time_t ttl, bool exclusive = true);

	/**
	 * ��õ�ǰ���ݿ����ӳص��������������
	 * @return {int}
	 */
	int get_dblimit() const
	{
		return dblimit_;
	}

	/**
	 * ��õ�ǰ���ݿ����ӳص�ǰ��������
	 * @return {int}
	 */
	int get_dbcount() const
	{
		return dbcount_;
	}
protected:
	/**
	 * ���麯�������� DB �ķ���
	 * @return {db_handle*}
	 */
	virtual db_handle* create() = 0;
private:
	std::list<db_handle*> pool_;
	int   dblimit_;  // ���ӳص��������������
	int   dbcount_;  // ��ǰ�Ѿ��򿪵�������
	locker* locker_;
	char  id_[128];  // ����ʵ����Ψһ ID ��ʶ
	time_t ttl_;     // ���ӳ��п������ӱ��ͷŵĳ�ʱֵ

	// ���ñ�ʵ����Ψһ ID ��ʶ
	void set_id();
};

} // namespace acl
