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
		 * 采用 mysql 数据库时的构造函数
		 * @param dbaddr {const char*} mysql 服务器地址，格式：IP:PORT，
		 *  在 UNIX 平台下可以为 UNIX 域套接口
		 * @param dbname {const char*} 数据库名
		 * @param dbuser {const char*} 数据库用户
		 * @param dbpass {const char*} 数据库用户密码
		 * @param dblimit {int} 数据库连接池的最大连接数限制
		 * @param dbflags {unsigned long} mysql 标记位
		 * @param auto_commit {bool} 是否自动提交
		 * @param conn_timeout {int} 连接数据库超时时间(秒)
		 * @param rw_timeout {int} 与数据库通信时的IO时间(秒)
		 */
		db_pool(const char* dbaddr, const char* dbname,
			const char* dbuser, const char* dbpass,
			int dblimit = 64, unsigned long dbflags = 0,
			bool auto_commit = true, int conn_timeout = 60,
			int rw_timeout = 60);

		/**
		 * 采用 sqlite 数据库时的构造函数
		 * @param dbfile {const char*} 数据库文件名
		 * @param dblimit {int} 数据库连接池的最大连接数限制
		 */
		db_pool(const char* dbfile, int dblimit = 64);
		~db_pool();

		/**
		 * 从数据库中连接池获得一个数据库连接，该函数返回的数据库
		 * 连接对象用完后必须调用 db_pool->put(db_handle*) 将连接
		 * 归还至数据库连接池，由该函数获得的连接句柄不能 delete，
		 * 否则会造成连接池的内部计数器出错
		 * @return {db_handle*} 返回空，则表示出错
		 */
		db_handle* peek();

		/**
		 * 将数据库连接放回至连接池中，当从数据库连接池中获得连接
		 * 句柄用完后应该通过该函数放回，不能直接 delete，因为那样
		 * 会导致连接池的内部记数发生错误
		 * @param conn {db_handle*} 数据库连接句柄，该连接句柄可以
		 *  是由 peek 创建的，也可以单独动态创建的
		 * @param keep {bool} 归还给连接池的数据库连接句柄是否继续
		 *  保持连接，如果否，则内部会自动删除该连接句柄
		 */
		void put(db_handle* conn, bool keep = true);
	private:
		std::list<db_handle*> pool_;
		int   dblimit_;  // 连接池的最大连接数限制
		int   dbcount_;  // 当前已经打开的连接数
		locker* locker_;
		char  id_[128];  // 本类实例的唯一 ID 标识

		// mysql 相关的参数项

		char* dbaddr_;  // 数据库监听地址
		char* dbname_;  // 数据库名
		char* dbuser_;  // 数据库账号
		char* dbpass_;  // 数据库账号密码
		unsigned long dbflags_;
		int   conn_timeout_;
		int   rw_timeout_;
		bool  auto_commit_;

		// sqlite 相关的参数项
		char* dbfile_;

		// 设置本实例的唯一 ID 标识
		void set_id();
	};

} // namespace acl
