#include "acl_stdafx.hpp"
#include "lib_acl.h"
#include "locker.hpp"
#include "log.hpp"
#include "db_handle.hpp"
#include "db_mysql.hpp"
#include "db_sqlite.hpp"
#include "db_pool.hpp"

namespace acl
{

	db_pool::db_pool(const char* dbaddr, const char* dbname,
		const char* dbuser, const char* dbpass, int dblimit /* = 64 */,
		unsigned long dbflags /* = 0 */, bool auto_commit /* = true */,
		int conn_timeout /* = 60 */, int rw_timeout /* = 60 */)
	: dbfile_(NULL)
	{
		acl_assert(dbaddr && *dbaddr);
		acl_assert(dbname && *dbname);
		dbaddr_ = acl_mystrdup(dbaddr);
		dbname_ = acl_mystrdup(dbname);
		if (dbuser)
			dbuser_ = acl_mystrdup(dbuser);
		else
			dbuser_ = NULL;

		if (dbpass)
			dbpass_ = acl_mystrdup(dbpass);
		else
			dbpass_ = NULL;

		if (dblimit > 0)
			dblimit_ = dblimit;
		else
			dblimit_ = 64;

		dbcount_ = 0;
		dbflags_ = dbflags;
		auto_commit_ = auto_commit;
		conn_timeout_ = conn_timeout;
		rw_timeout_ = rw_timeout;

		locker_ = NEW locker(true);
	}

	db_pool::db_pool(const char* dbfile, int dblimit /* = 64 */)
	: dbaddr_(NULL)
	, dbname_(NULL)
	, dbuser_(NULL)
	, dbpass_(NULL)
	{
		acl_assert(dbfile && *dbfile);
		dbfile_ = acl_mystrdup(dbfile);

		if (dblimit > 0)
			dblimit_ = dblimit;
		else
			dblimit_ = 64;
		dbcount_ = 0;
		set_id();
	}

	db_pool::~db_pool()
	{
		std::list<db_handle*>::iterator it = pool_.begin();
		for (; it != pool_.end(); ++it)
			delete (*it);
		pool_.clear();
		delete locker_;

		if (dbaddr_)
			acl_myfree(dbaddr_);
		if (dbname_)
			acl_myfree(dbname_);
		if (dbuser_)
			acl_myfree(dbuser_);
		if (dbpass_)
			acl_myfree(dbpass_);
		if (dbfile_)
			acl_myfree(dbfile_);
		set_id();
	}

	void db_pool::set_id()
	{
		struct timeval tv;

		(void) gettimeofday(&tv, NULL);
		snprintf(id_, sizeof(id_), "%d.%d.%d",
			(int) tv.tv_sec, (int) tv.tv_usec, rand());
	}

	db_handle* db_pool::peek()
	{
		db_handle* conn;

		locker_->lock();

		std::list<db_handle*>::iterator it = pool_.begin();
		if (it != pool_.end())
		{
			conn = *it;
			pool_.erase(it);
			locker_->unlock();
			return conn;
		}
		else if (dbcount_ >= dblimit_)
		{
			logger_error("dbcount(%d) > dblimit(%d)",
				dbcount_, dblimit_);
			locker_->unlock();
			return NULL;
		}

		// 记数器加 1
		dbcount_++;
		locker_->unlock(); // 尽量减少锁的锁定时间

		// 创建 mysql 或 sqlite 连接句柄
		if (dbfile_)
			conn = NEW db_sqlite(dbfile_);
		else
			conn = NEW db_mysql(dbaddr_, dbname_, dbuser_,
				dbpass_, dbflags_, auto_commit_,
				conn_timeout_, rw_timeout_);
		conn->set_id(id_);
		return conn;
	}

	void db_pool::put(db_handle* conn, bool keep /* = true */)
	{
		acl_assert(conn);

		const char* id = conn->get_id();
		bool eq;
		if (id && strcmp(id, id_) == 0)
			eq = true;
		else
			eq = false;

		locker_->lock();

		if (keep)
		{
			pool_.push_back(conn);
			// 如果该连接句柄不是由本连接池产生的，则需要
			// 重新设置连接句柄的 ID 标识
			if (!eq)
				conn->set_id(id_);
		}
		else
		{
			// 只有当该连接句柄由本连接池产生时，才需要
			// 减少记数器
			if (eq)
				dbcount_--;
			delete conn;
		}
		locker_->unlock();
	}

} // namespace acl
