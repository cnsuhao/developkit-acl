#include <string.h>
#include "string_util.hpp"

namespace acl_min
{

static char *__strtok(char **src, const char *sep)
{
	char   *start = *src;
	char   *end;

	/*
	 * Skip over leading delimiters.
	 */
	start += strspn(start, sep);
	if (*start == 0) {
		*src = start;
		return (0);
	}

	/*
	 * Separate off one token.
	 */
	end = start + strcspn(start, sep);
	if (*end != 0)
		*end++ = 0;
	*src = end;
	return (start);
}

std::vector<std::string>& split3(const char *str, const char *delim,
	std::vector<std::string>& tokens)
{
	char   *saved_string = strdup(str);
	char   *bp = saved_string;
	char   *arg;

	while ((arg = __strtok(&bp, delim)) != 0)
		tokens.push_back(arg);
	free(saved_string);
	return tokens;
}

char* lowercase(const char* src, char* buf, size_t size)
{
	if (src == NULL || buf == NULL || size == 0)
		return NULL;

	char *cp = buf;

	while (size > 1 && *src) {
		*cp++ = tolower(*src++);
		size--;
	}

	*cp = 0;
	return buf;
}

} // namespace acl_min
