#pragma once
#include "acl_cpp/acl_cpp_define.hpp"

namespace acl
{

/**
 * ��Ҫ���ӳ��ͷŵ���̳д���󣬿��Ե��� aio_handle:delay_free ������
 * �ӳ����ٵ�Ŀ�ģ��������ڵݹ�����б������ͷ�ʱ�Ķ�����ǰ�ͷŵ�����
 */
class ACL_CPP_API aio_delay_free
{
public:
	aio_delay_free(void);
	virtual ~aio_delay_free(void);

	/**
	 * �ж���ʱ���Ƿ�����������״̬����������״̬�Ķ�ʱ����
	 * ���ܱ�ɾ���ģ����������ڴ����ش���
	 * @return {bool} �Ƿ�������״̬����������״̬�Ķ�����
	 *  ���������ٵ�
	 */
	bool locked(void) const;

	/**
	 * ���������������������������������ڶ�ʱ����������оͲ���
	 * �Զ����������������ٹ���
	 */
	void set_locked(void);

	/**
	 * ��������ȡ������������״̬
	 */
	void unset_locked(void);

	/**
	 * ���ٺ��������ڲ��� aio_timer_delay_free �����ж���Ҫ���ӳ��ͷ�
	 * �����������
	 */
	virtual void destroy(void) {}
private:
	bool locked_;
	bool locked_fixed_;
};

class master_timer_task;

class ACL_CPP_API master_timer
{
public:
	/**
	 * ���캯��
	 * @param keep {bool} �ö�ʱ���Ƿ������Զ�����
	 */
	master_timer(bool keep = false);
	virtual ~master_timer();

	/**
	 * ��ʱ����������Ƿ�Ϊ��
	 * @return {bool}
	 */
	bool empty(void) const;

	/**
	 * ��ʱ������������
	 * @return {size_t}
	 */
	size_t length(void) const;

	/**
	 * �ö�ʱ���Ƿ����Զ�������
	 * @param on {bool}
	 */
	void keep_timer(bool on);

	/**
	 * �жϸö�ʱ���Ƿ����Զ�������
	 * @return {bool}
	 */
	bool keep_timer(void) const;

	/**
	 * ��ն�ʱ����Ķ�ʱ����
	 * @return {int} ������Ķ�ʱ����ĸ���
	 */
	int clear(void);

protected:
	/**
	 * �������ʵ�ִ˻ص�������ע�����������߽�ֹ��
	 * timer_callback �ڲ����� master_timer ������
	 * ���������򽫻���ɴ��
	 * @param id {unsigned int} ��Ӧĳ������� ID ��
	 */
	virtual void timer_callback(unsigned int id) = 0;

	/****************************************************************/
	/*        ������Ե������º������һЩ�µĶ�ʱ������ ID ��              */
	/****************************************************************/
#ifdef WIN32
	__int64 present_;

	/**
	 * ��Ա���ʱ�������µ�����ID�ţ����������ͨ��һ����ʱ������
	 * �����ʱ����
	 * @param id {unsigned int} ��ʱ����ʱ����ID��
	 * @param delay {__int64} ÿ������Զ������ö�ʱ����ͬʱ����Ӧ�Ķ�ʱ����ʱ
	 *  ����ID�Ŵ���(΢�뼶)
	 * @return {__int64} ���뱾��ʱ���ĵ�һ�����ᴥ���Ķ�ʱ����ID�����(΢�뼶)
	 */
	__int64 set_task(unsigned int id, __int64 delay);

	/**
	 * ɾ����ʱ����ĳ����ϢID��Ӧ�Ķ�ʱ����
	 * @param {unsigned int} ��ʱ����ID
	 * @return {__int64} ���뱾��ʱ���ĵ�һ�����ᴥ���Ķ�ʱ����ID�����(΢�뼶)
	 */
	__int64 del_task(unsigned int id);
#else
	long long int present_;
	long long int set_task(unsigned int id, long long int delay);
	long long int del_task(unsigned int id);
#endif

private:
	size_t length_;
	std::list<master_timer_task*> tasks_;
	bool keep_;  // �ö�ʱ���Ƿ������Զ�����
	bool destroy_on_unlock_;  // �������Ƿ� destroy
#ifdef WIN32
	__int64 set_task(aio_timer_task* task);
	__int64 trigger(void);
#else
	long long int set_task(aio_timer_task* task);
	long long int trigger(void);
#endif
};

}  // namespace acl
