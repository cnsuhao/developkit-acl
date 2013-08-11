#include "acl_stdafx.hpp"
#include "acl_cpp/stdlib/locker.hpp"
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/db/db_handle.hpp"
#include "acl_cpp/db/db_sqlite.hpp"
#include "acl_cpp/db/db_pool.hpp"

namespace acl
{

db_pool::db_pool(int dblimit /* = 64 */, int idle /* = 0 */)
{
	if (dblimit > 0)
		dblimit_ = dblimit;
	else
		dblimit_ = 64;

	dbcount_ = 0;
	locker_ = NEW locker(true);
}

db_pool::~db_pool()
{
	std::list<db_handle*>::iterator it = pool_.begin();
	for (; it != pool_.end(); ++it)
		delete (*it);
	pool_.clear();
	delete locker_;
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
		conn->set_when(time(NULL));
		return conn;
	}
	else if (dbcount_ >= dblimit_)
	{
		logger_error("dbcount(%d) > dblimit(%d)",
			dbcount_, dblimit_);
		locker_->unlock();
		return NULL;
	}

	// �������� 1
	dbcount_++;
	locker_->unlock(); // ����������������ʱ��

	// ���� mysql �� sqlite ���Ӿ��
	conn = create();

	conn->set_id(id_);
	conn->set_when(time(NULL));
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
		conn->set_when(time(NULL));
		// ���黹�����ӷ��������ײ��������ڵ����ͷŹ�������
		// ʱ�ȽϷ��㣬�����ھ��콫��æ�����ݿ����ӹر�
		pool_.push_front(conn);
		// ��������Ӿ�������ɱ����ӳز����ģ�����Ҫ
		// �����������Ӿ���� ID ��ʶ
		if (!eq)
			conn->set_id(id_);
	}
	else
	{
		// ֻ�е������Ӿ���ɱ����ӳز���ʱ������Ҫ
		// ���ټ�����
		if (eq)
			dbcount_--;
		delete conn;
	}

	(void) dbidle_erase(ttl_, false);
	locker_->unlock();
}

int db_pool::dbidle_erase(time_t ttl, bool exclusive /* = true */)
{
	if (ttl < 0)
		return 0;
	if (exclusive)
		locker_->lock();
	if (ttl == 0)
	{
		int   n = 0;
		std::list<db_handle*>::iterator it = pool_.begin();
		for (; it != pool_.end(); ++it)
		{
			delete *it;
			n++;
		};
		pool_.clear();
		dbcount_ = 0;
		locker_->unlock();
		return n;
	}

	int n = 0;
	time_t now = time(NULL), when;
	std::list<db_handle*>::reverse_iterator next, it = pool_.rbegin();
	for (next = it; it != pool_.rend(); it = next)
	{
		++next;
		when = (*it)->get_when();
		if (when <= 0)
			continue;
		if (now - when < ttl)
			break;
		delete *it;
		pool_.erase(it.base());
		n++;
		dbcount_--;
	}

	locker_->unlock();
	return n;
}

} // namespace acl
