#pragma once

struct acl_pthread_pool_t;
struct acl_pthread_pool_attr_t;

namespace acl
{

class thread;

class thread_pool
{
public:
	thread_pool();
	virtual ~thread_pool();

	/**
	 * �����̳߳�
	 */
	void start();

	/**
	 * ֹͣ�̳߳أ��ڲ�ֻ�ǵȴ��������߳��˳����û����ô�
	 * �����ȴ��������߳��˳�����Ȼ���������̳߳ع��̣�
	 * ������ʽ�ص��� start ���̣�����ֱ�ӵ��� execute
	 */
	void stop();

	/**
	 * ǿ�������̳߳أ����ͷ��̳߳���Դ�����ô˺�������ʹ�������߳��˳���
	 * �������ͷű�ʵ�����������ʵ���Ƕ�̬��������û�Ӧ�����ͷ���ʵ����
	 * �ڵ��ñ�����������������̳߳ع��̣���������µ��� start ����
	 */
	void destroy();
	
	/**
	 * �ڵ��� start ǰ���ô˺������������������̵߳Ķ�ջ��С
	 * @param size {size_t} �̶߳�ջ��С������ֵΪ 0 ��δ
	 *  ���ô˺����������������̶߳�ջ��СΪϵͳ��Ĭ��ֵ
	 * @return {thread&}
	 */
	thread_pool& set_stacksize(size_t size);

	/**
	 * �����̳߳�����̸߳�������
	 * @param max {size_t} ����߳�������������ô˺��������ڲ�ȱʡֵΪ 100
	 * @return {thread_pool&}
	 */
	thread_pool& set_limit(size_t max);

	/**
	 * �����̳߳��п����̵߳ĳ�ʱ�˳�ʱ��
	 * @param ttl {int} ���г�ʱʱ��(��)����������ô˺��������ڲ�ȱʡΪ 0
	 * @return {thread_pool&}
	 */
	thread_pool& set_idle(int ttl);

	/**
	 * ��һ�����񽻸��̳߳��е�һ���߳�ȥִ�У��̳߳��е�
	 * �̻߳�ִ�и������е� run ����
	 * @param thr {thread*} �߳�����
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool execute(thread* thr);

	/**
	 * ��õ�ǰ�̳߳������̵߳�����
	 * @return {int} �����̳߳������̵߳����������δͨ������ start
	 *  �����̳߳ع��̣���ú������� -1
	 */
	int curr_threads() const;

protected:
	/**
	 * ���̳߳��е����̵߳�һ�α�����ʱ�����麯���������ã�
	 * �û��������Լ���ʵ������һЩ��ʼ������
	 * @return {bool} ��ʼ���Ƿ�ɹ�
	 */
	virtual bool thread_on_init() { return true; }

	/**
	 * ���̳߳��е����߳��˳�ʱ�����麯���������ã��û�����
	 * ���Լ���ʵ�� ����һЩ��Դ�ͷŹ���
	 */
	virtual void thread_on_exit() {}

private:
	size_t stack_size_;
	size_t threads_limit_;
	int    thread_idle_;

	acl_pthread_pool_t* thr_pool_;
	acl_pthread_pool_attr_t* thr_attr_;

	static void thread_run(void* arg);
	static int  thread_init(void* arg);
	static void thread_exit(void* arg);
};

} // namespace acl
