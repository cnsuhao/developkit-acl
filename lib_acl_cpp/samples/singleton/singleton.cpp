// singleton.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "singleton.hpp"

// 跟踪调用过程的计数器
static int __nstep = 0;

class singleton_test : public acl::singleton <singleton_test>
{
public:
	singleton_test()
	{
		// 如果该句话打印先于 main 函数中的打印结果，则
		// 说明该单例是在 main 函数执行先被初始化的
		printf("step %d: construct called\r\n", ++__nstep);
		fflush(stdout);
	}

	virtual ~singleton_test()
	{
		printf("step %d: destruct called\r\n", ++__nstep);
		fflush(stdout);
		getchar();
	}

	const singleton_test& init() const
	{
		printf("step %d: init called\r\n", ++__nstep);
		fflush(stdout);
		return *this;
	}

	const singleton_test& set(const char* name) const
	{
		printf("step %d: set(%s) called\r\n", ++__nstep, name);
		fflush(stdout);
		return *this;
	}
};

//singleton_test::get_const_instance();
int main(int argc, char* argv[])
{
	printf("step %d: first line in main\r\n", ++__nstep);
	fflush(stdout);
	const singleton_test& test1 = singleton_test::get_mutable_instance().init();
	const singleton_test& test2 = singleton_test::get_const_instance();
	test1.set("test1");
	test2.set("test2");
	printf("step %d: enter any key to exit\r\n", ++__nstep);
	fflush(stdout);
	getchar();
	return 0;
}
