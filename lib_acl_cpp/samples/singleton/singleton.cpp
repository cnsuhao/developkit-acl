// singleton.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "singleton.hpp"

// ���ٵ��ù��̵ļ�����
static int __nstep = 0;

class singleton_test : public acl::singleton <singleton_test>
{
public:
	singleton_test()
	{
		// ����þ仰��ӡ���� main �����еĴ�ӡ�������
		// ˵���õ������� main ����ִ���ȱ���ʼ����
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
