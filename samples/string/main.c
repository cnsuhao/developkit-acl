#include "lib_acl.h"

int main(void)
{
	char *src = acl_mystrdup("hello \tworld! you're  welcome to China!");
	char *ptr, *src_saved;

	src_saved = src;
	printf("src: %s\r\n", src);
	while ((ptr = acl_mystrtok(&src, " \t!'")) != NULL)
	{
		printf("--------------------------------------\r\n");
		printf("ptr: |%s|\r\n", ptr);
		printf("src: |%s|\r\n", src);
		printf("src_saved: |%s|\r\n", src_saved);
	}

	acl_myfree(src_saved);
	return 0;
}
