#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <list>

namespace acl
{

	class db_handle;
	class locker;

	class db_pool
	{
	public:
		/**
		 * ���� mysql ���ݿ�ʱ�Ĺ��캯��
		 * @param dbaddr {const char*} mysql ��������ַ����ʽ��IP:PORT��
		 *  �� UNIX ƽ̨�¿���Ϊ UNIX ���׽ӿ�
		 * @param dbname {const char*} ���ݿ���
		 * @param dbuser {const char*} ���ݿ��û�
		 * @param dbpass {const char*} ���ݿ��û�����
		 * @param dblimit {int} ���ݿ����ӳص��������������
		 * @param dbflags {unsigned long} mysql ���λ
		 * @param auto_commit {bool} �Ƿ��Զ��ύ
		 * @param conn_timeout {int} �������ݿⳬʱʱ��(��)
		 * @param rw_timeout {int} �����ݿ�ͨ��ʱ��IOʱ��(��)
		 */
		db_pool(const char* dbaddr, const char* dbname,
			const char* dbuser, const char* dbpass,
			int dblimit = 64, unsigned long dbflags = 0,
			bool auto_commit = true, int conn_timeout = 60,
			int rw_timeout = 60);

		/**
		 * ���� sqlite ���ݿ�ʱ�Ĺ��캯��
		 * @param dbfile {const char*} ���ݿ��ļ���
		 * @param dblimit {int} ���ݿ����ӳص��������������
		 */
		db_pool(const char* dbfile, int dblimit = 64);
		~db_pool();

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
	private:
		std::list<db_handle*> pool_;
		int   dblimit_;  // ���ӳص��������������
		int   dbcount_;  // ��ǰ�Ѿ��򿪵�������
		locker* locker_;
		char  id_[128];  // ����ʵ����Ψһ ID ��ʶ

		// mysql ��صĲ�����

		char* dbaddr_;  // ���ݿ������ַ
		char* dbname_;  // ���ݿ���
		char* dbuser_;  // ���ݿ��˺�
		char* dbpass_;  // ���ݿ��˺�����
		unsigned long dbflags_;
		int   conn_timeout_;
		int   rw_timeout_;
		bool  auto_commit_;

		// sqlite ��صĲ�����
		char* dbfile_;

		// ���ñ�ʵ����Ψһ ID ��ʶ
		void set_id();
	};

} // namespace acl
