#include "stdafx.h"

using namespace acl;

static void test(void)
{
	std::list<int*> tt;
	for (int i = 0; i < 5; i++)
	{
		int* n = (int*) malloc(sizeof(int));
		*n = i;
		tt.push_back(n);
	}
	std::list<int*>::iterator it = tt.end();
	do
	{
		--it;
		printf("i: %d\r\n", **it);
	} while (it != tt.begin());

	std::list<int*>::reverse_iterator rit = tt.rbegin();
	for (; rit != tt.rend(); ++rit)
	{
		it = rit.base();
		--it;
		printf("r-i: %d, %d\r\n", (**rit), **it);
		free(*it);
		tt.erase(it);
		rit--;
	}

	exit (0);
}

int main(void)
{
	if (0)
		test();

	db_pool *dp = new mysql_pool("127.0.0.1:3306", "test_db", "zsxxsz",
			"111111");

	std::list<db_handle*> dbs;

	// 设置空闲连接的生存周期
	dp->set_idle(1);
	for (int i = 0; i < 10; i++)
	{
		db_handle* dh = dp->peek();
		dbs.push_back(dh);
	}

	std::list<db_handle*>::iterator it = dbs.begin();
	for (; it != dbs.end(); ++it)
	{
		dp->put(*it);
		//sleep(1);
	}

	dbs.clear();

	printf("dbpool count: %d\r\n", dp->get_dbcount());
	sleep(2);
	db_handle* dh = dp->peek();
	dp->put(dh);
	printf("dbpool count: %d\r\n", dp->get_dbcount());
	delete dp;

	return 0;
}
