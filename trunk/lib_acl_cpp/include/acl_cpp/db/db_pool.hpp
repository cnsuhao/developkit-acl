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
	 */
	db_pool(int dblimit = 64);
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

	// ���ñ�ʵ����Ψһ ID ��ʶ
	void set_id();
};

} // namespace acl
