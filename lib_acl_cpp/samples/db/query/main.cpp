#include <stdio.h>
#include <stdlib.h>
#include "acl_cpp/db/query.hpp"

#ifdef WIN32
# ifndef snprintf
#  define snprintf _snprintf
# endif
#endif

int main(void)
{
	acl::query query;

	query.create_sql("select * from person where name = :name"
		" and age >= :age and male = 1 and nick = :name");
	query.set_parameter("name", "'zsxxsz'\\");
	query.set_parameter("age", 30);

	printf("sql: %s\r\n", query.to_string().c_str());

	query.reset();

	int age = 20;
	query.create_sql("update table set name=:name, age=%d"
		", home=:home where nick=:name", age)
		.set_parameter("name", "zsx1&xsz1")
		.set_parameter("home", "回龙观");
	printf("sql: %s\r\n", query.to_string().c_str());

	query.reset();
	query.create_sql("insert into table(name, age, home, nick)"
		" values(:name, %d, :home, :nick)", age)
		.set_parameter("name", "zsx'xsz")
		.set_parameter("home", "北京.昌平.回龙观")
		.set_parameter("nick", "'逍遥仙'");

	printf("sql: %s\r\n", query.to_string().c_str());

#ifdef WIN32
	printf("enter any key to exit\r\n");
	getchar();
#endif

	return 0;
}

