#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/stdlib/noncopyable.hpp"
#include <list>
#include <set>
//#include "string.hpp"

struct ACL_AIO;

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
	
	class aio_timer_task;
	class aio_handle;

	/**
	 * ��ʱ���Ļص���
	 */
	class ACL_CPP_API aio_timer_callback : public aio_delay_free
	{
	public:
		/**
		 * ���캯��
		 * @param keep {bool} �ö�ʱ���Ƿ������Զ�����
		 */
		aio_timer_callback(bool keep = false);
		virtual ~aio_timer_callback(void);

		/**
		 * ����ʱ�����������Ϊ��ʱ�Ļص�������
		 * ��������������ͷţ�һ���ú��������ã�
		 * ����ζ�Ÿö�ʱ�������е����ж�ʱ���񶼴�
		 * ��ʱ�������б�ɾ��
		 * �ú�����������������������
		 * 1) ��ʱ�����е�������Ϊ 0 ʱ(�磬
		 *    del_timer(aio_timer_callback*, unsigned int) ��
		 *    ������������Ϊ 0 ʱ)
		 * 2) �� aio_handle û�������ظ���ʱ���Ҹö�ʱ����
		 *    ��һ����ʱ���񱻴�����
		 * 3) �� del_timer(aio_timer_callback*) �����ú�
		 */
		virtual void destroy(void) {}

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
		friend class aio_handle;

		/**
		 * �������ʵ�ִ˻ص�������ע�����������߽�ֹ��
		 * timer_callback �ڲ����� aio_timer_callback ������
		 * ���������򽫻���ɴ��
		 * @param id {unsigned int} ��Ӧĳ������� ID ��
		 */
		virtual void timer_callback(unsigned int id) = 0;

		/************************************************************************/
		/*        ������Ե������º������һЩ�µĶ�ʱ������ ID ��              */
		/************************************************************************/
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

		/**
		 * ���õ�ǰ��ʱ����ʱ���
		 */
		void set_time(void);
	private:
		aio_handle* handle_;
		size_t length_;
		std::list<aio_timer_task*> tasks_;
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

	// �¼���������
	typedef enum
	{
		ENGINE_SELECT,  // select ģʽ(֧������ƽ̨)
		ENGINE_POLL,    // poll ģʽ(�� UNIX ƽ̨)
		ENGINE_KERNEL,  // kernel ģʽ(win32: iocp, Linux: epoll, FreeBsd: kqueue, Solaris: devpoll
		ENGINE_WINMSG   // win32 GUI ��Ϣģʽ
	} aio_handle_type;

	/**
	 * ������IO���¼������࣬�����װ��ϵͳ�� select/poll/epoll/kqueue/devpoll/iocp,
	 */

	class aio_timer_delay_free;

	class ACL_CPP_API aio_handle : private noncopyable
	{
	public:
		/**
		 * ���캯�������Զ�����IO�¼����棬���������������л��Զ��ͷ�
		 * @param engine_type {aio_handle_type} �����õ���������
		 *  ENGINE_SELECT: select ��ʽ��֧�� win32/unix ƽ̨
		 *  ENGINE_POLL: poll ��ʽ��֧�� unix ƽ̨
		 *  ENGINE_KERNEL: �Զ����ݸ���ϵͳƽ̨��֧�ֵĸ�Ч�ں������������
		 *  ENGINE_WINMSG: win32 ������Ϣ��ʽ��֧�� win32 ƽ̨
		 * @param nMsg {unsigned int} �� engine_type Ϊ ENGINE_WINMSG������ֵ
		 *  ���� 0 ʱ�����첽����������Ϣ�󶨣�������ȱʡ��Ϣ�󶨣�
		 *  �� engine_type Ϊ�� ENGINE_WINMSG ʱ����ֵ�������첽�����������
		 *  
		 */
		aio_handle(aio_handle_type engine_type = ENGINE_SELECT, unsigned int nMsg = 0);

		/**
		 * ���캯���������߽� ACL_AIO �������������������������в�����
		 * �Զ��ͷŸ� ACL_AIO ���
		 * @param handle {ACL_AIO*} ACL_AIO ���
		 */
		aio_handle(ACL_AIO* handle);

		virtual ~aio_handle();

		/**
		 * ����첽�����������Ƿ�������������������ᱻ���еĻ���
		 * ���첽���������첽�������̳У�һ�� aio_handle �������ȱʡ
		 * ���������������
		 * @param onoff {bool} �����Ƿ���������
		 */
		void keep_read(bool onoff);

		/**
		 * ����첽�������Ƿ������˳��������ݵĹ���
		 * @return {bool}
		 */
		bool keep_read() const;

		/**
		 * ���ö�ʱ��
		 * @param callback {aio_timer_callback*} ��ʱ���ص����������
		 * @param delay {int64} ��ʱ��ʱ����(΢��)
		 * @param id {unsigned int} ��ʱ��ĳ������� ID ��
		 * @return {int64} ��ʱ����Чʱ��(��1970.1.1������΢����)
		 */
#ifdef WIN32
		__int64 set_timer(aio_timer_callback* callback,
			__int64 delay, unsigned int id = 0);
#else
		long long int set_timer(aio_timer_callback* callback,
			long long int delay, unsigned int id = 0);
#endif

		/**
		 * ɾ����ʱ�������ж�ʱ�����¼�
		 * @param callback {aio_timer_callback*} ��ʱ���ص����������
		 * @return {time_t} ��ʱ����Чʱ��(��1970.1.1������΢����)
		 */
#ifdef WIN32
		__int64 del_timer(aio_timer_callback* callback);
#else
		long long int del_timer(aio_timer_callback* callback);
#endif

		/**
		 * ɾ����ʱ����ĳ��ָ�� ID �ŵĶ�ʱ����
		 * @param callback {aio_timer_callback*} ��ʱ���ص����������
		 * @param id {unsigned int} ��ʱ��ĳ������� ID ��
		 * @return {time_t} ��ʱ����Чʱ��(��1970.1.1������΢����)
		 */
#ifdef WIN32
		__int64 del_timer(aio_timer_callback* callback, unsigned int id);
#else
		long long del_timer(aio_timer_callback* callback, unsigned int id);
#endif

		/**
		 * ����ʱ����������״̬ʱ���û���Ϊ�޷��ͷŸö�ʱ��������ڴ�й¶��
		 * ͨ���˺��������Խ���������״̬�Ķ�ʱ��������δ����״̬ʱ���¼�
		 * ���������ͷ�(���� aio_delay_free::destroy())���Ӷ����Ա���
		 * �ڴ�й¶����
		 * @param callback {aio_delay_free*}
		 */
		void delay_free(aio_delay_free* callback);

		/**
		 * ��� ACL_AIO ���
		 * @return {ACL_AIO*}
		 */
		ACL_AIO* get_handle(void) const;

		/**
		 * ����첽���������
		 * @return {aio_handle_type}
		 */
		aio_handle_type get_engine_type(void) const;

		/**
		 * ��õ�ǰ���ڼ�ص��첽��������
		 * @return {int}
		 */
		int length() const;

		/**
		 * ��������첽����״̬��������׼�����첽���Ĵ������
		 * @return {bool} �Ƿ�Ӧ��ֹ�첽����
		 */
		bool check();

		/**
		 * ֪ͨ�첽��������ֹ
		 */
		void stop();

		/**
		 * �����첽������ڲ�״̬
		 */
		void reset();

	protected:
		friend class aio_stream;

		/**
		 * �첽�������� 1
		 */
		void increase();

		/**
		 * ���첽�������� 1 ʱ�Ļص��麯��
		 */
		virtual void on_increase() {}

		/**
		 * �첽�������� 1
		 */
		void decrease();

		/**
		 * ���첽�������� 1 ʱ�Ļص��麯��
		 */
		virtual void on_decrease() {}

	private:
		ACL_AIO* aio_;
		bool inner_alloc_;
		bool stop_;
		int  nstream_;
		aio_handle_type engine_type_;
		aio_timer_delay_free* delay_free_timer_;

		void destroy_timer(aio_timer_callback* callback);
		static void on_timer_callback(int event_type,
			aio_timer_callback *callback);
	};

}
