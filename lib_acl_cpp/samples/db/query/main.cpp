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
		" and age >= :age and male = 1 and nick = :name")
		.set_parameter("name", "'zsxxsz'\\")
		.set_parameter("age", 30);

	printf("sql: %s\r\n", query.to_string().c_str());

#ifdef WIN32
	printf("enter any key to exit\r\n");
	getchar();
#endif

	return 0;
}

