#pragma once

namespace acl
{

class thread_pool;

/**
 * �̴߳����࣬����Ľӿڶ��������� Java �Ľӿڶ��壬������Ҫʵ��
 * ����Ĵ��麯����ʹ����ͨ������ thread::start() �����̹߳���
 */
class thread
{
public:
	thread();
	virtual ~thread();

	/**
	 * ��ʼ�����̹߳���
	 * @return {bool} �Ƿ�ɹ������߳�
	 */
	bool start();

	/**
	 * �������߳�ʱΪ�� detachable ״̬������Ե��ô˺���
	 * �ȴ��߳̽����������������߳�ʱΪ detachable ״̬
	 * �ڵ��ñ�����ʱ���ᱨ��
	 * @param out {void**} ���ò����ǿ�ָ��ʱ���ò����������
	 *  �߳��˳�ǰ���صĲ���
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool wait(void** out);

	/**
	 * �ڵ��� start ǰ���ô˺������������������߳��Ƿ�Ϊ
	 * ���� (detachable) ״̬�����δ���ô˺�������������
	 * ���߳�Ĭ��Ϊ����״̬
	 * @param yes {bool} �Ƿ�Ϊ����״̬
	 * @return {thread&}
	 */
	thread& set_detachable(bool yes);

	/**
	 * �ڵ��� start ǰ���ô˺������������������̵߳Ķ�ջ��С
	 * @param size {size_t} �̶߳�ջ��С������ֵΪ 0 ��δ
	 *  ���ô˺����������������̶߳�ջ��СΪϵͳ��Ĭ��ֵ
	 * @return {thread&}
	 */
	thread& set_stacksize(size_t size);

	/**
	 * �ڵ��� start ����ô˺������Ի���������̵߳� id ��
	 * @return {unsigned long}
	 */
	unsigned long thread_id() const;

	/**
	 * ��ǰ�����������̵߳��߳� id ��
	 * @return {unsigned long}
	 */
	static unsigned long thread_self();

protected:
	friend class thread_pool;

	/**
	 * ���麯�����������ʵ�ִ˺��������û����� start ������
	 * ���麯�������������ã���������һ�����߳�
	 * @return {void*} �߳��˳�ǰ���صĲ���
	 */
	virtual void* run() = 0;

private:
	bool detachable_;
	size_t stack_size_;
	unsigned long thread_id_;
#ifdef WIN32
	void* thread_;
#endif
	void* return_arg_;
	static void* thread_run(void* arg);
};

} // namespace acl
