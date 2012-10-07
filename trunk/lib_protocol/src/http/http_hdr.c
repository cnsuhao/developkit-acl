#include "StdAfx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "http/lib_http.h"

static int __http_hdr_def_entry = 50;
static int __http_hdr_max_lines = 1024;

/*-------------------------- for general http header -------------------------*/
/* 生成一个新的 HTTP_HDR 数据结构 */
static void __hdr_init(HTTP_HDR *hh)
{
	const char *myname = "__hdr_init";

	hh->entry_lnk = acl_array_create(__http_hdr_def_entry);
	if (hh->entry_lnk == NULL) {
		char ebuf[256];
		acl_msg_fatal("%s, %s(%d): array create error=%s",
				__FILE__, myname, __LINE__,  acl_last_strerror(ebuf, sizeof(ebuf)));
	}

	hh->max_lines      = __http_hdr_max_lines;
	hh->cur_lines      = 0;
	hh->valid_lines    = 0;
	hh->content_length = -1;
	hh->chunked        = 0;
	hh->keep_alive     = 0;
}

/* 分配一个HTTP协议头的基础结构 */

HTTP_HDR *http_hdr_new(size_t size)
{
	const char *myname = "http_hdr_new";
	HTTP_HDR *hh;

	if (size != sizeof(HTTP_HDR_REQ) && size != sizeof(HTTP_HDR_RES))
		acl_msg_fatal("%s, %s(%d): size(%d) invalid",
			__FILE__, myname, __LINE__, (int) size);

	hh = (HTTP_HDR*) acl_mycalloc(1, (int) size);
	if (hh == NULL) {
		acl_msg_fatal("%s, %s(%d): can't calloc, error(%s)",
			__FILE__, myname, __LINE__,  acl_last_serror());
	}

	__hdr_init(hh);

	return (hh);
}

/* 克隆一个HTTP协议头的基础结构 */

void http_hdr_clone(const HTTP_HDR *src, HTTP_HDR *dst)
{
	ACL_ARRAY  *entry_lnk_saved = dst->entry_lnk;  /* 先保留原指针 */
	HTTP_HDR_ENTRY *entry, *entry_from;
	int   i, n;

	memcpy(dst, src, sizeof(HTTP_HDR));
	dst->entry_lnk = entry_lnk_saved;  /* 恢复原始指针 */
	dst->chat_ctx = NULL;  /* bugfix, 2008.10.7 , zsx */
	dst->chat_free_ctx_fn = NULL;  /* bugfix, 2008.10.7 , zsx */

	n = acl_array_size(src->entry_lnk);
	for (i = 0; i < n; i++) {
		entry_from = (HTTP_HDR_ENTRY*) acl_array_index(src->entry_lnk, i);
		entry = (HTTP_HDR_ENTRY*) acl_mymalloc(sizeof(HTTP_HDR_ENTRY));
		entry->off = 0;
		entry->name = acl_mystrdup(entry_from->name);
		entry->value = acl_mystrdup(entry_from->value);
		http_hdr_append_entry(dst, entry);
	}
}

/* 释放一条HTTP_HDR_ENTRY */
static void __hdr_entry_free_fn(void *arg)
{
	HTTP_HDR_ENTRY *entry = (HTTP_HDR_ENTRY *) arg;

	if (entry == NULL)
		return;
	if (entry->name != NULL)
		acl_myfree(entry->name);
	if (entry->value != NULL)
		acl_myfree(entry->value);
	acl_myfree(entry);
}

/* 释放 HTTP_HDR */
static void __hdr_free_member(HTTP_HDR *hh)
{
	if (hh->entry_lnk)
		acl_array_destroy(hh->entry_lnk, __hdr_entry_free_fn);
	hh->entry_lnk = NULL;
}

void http_hdr_free(HTTP_HDR *hh)
{
	if (hh == NULL)
		return;

	__hdr_free_member(hh);
	if (hh->chat_free_ctx_fn && hh->chat_ctx)
		hh->chat_free_ctx_fn(hh->chat_ctx);
	acl_myfree(hh);
}

void http_hdr_reset(HTTP_HDR *hh)
{
	if (hh == NULL)
		return;

	__hdr_free_member(hh);
	__hdr_init(hh);
}

/*----------------------------------------------------------------------------*/

HTTP_HDR_ENTRY *http_hdr_entry_build(const char *name, const char *value)
{
	HTTP_HDR_ENTRY *entry;

	entry = (HTTP_HDR_ENTRY*) acl_mymalloc(sizeof(HTTP_HDR_ENTRY));
	entry->off = 0;
	entry->name = acl_mystrdup(name);
	entry->value = acl_mystrdup(value);
	return (entry);
}

/* 根据传入的一行数据进行分析, 生成一个 HTTP_HDR_ENTRY */

HTTP_HDR_ENTRY *http_hdr_entry_new(const char *data)
{
/* data format: Content-Length: 245 */
	const char *myname = "http_hdr_entry_new";
	char *pbuf = NULL, *ptr;
	char *pname, *pvalue;
	HTTP_HDR_ENTRY *entry;

#undef	RETURN
#define	RETURN(_x_) do { \
	if (pbuf != NULL) \
		acl_myfree(pbuf); \
	return (_x_); \
} while (0)

	pbuf = acl_mystrdup(data);
	ptr = pbuf;
	pname = acl_mystrtok(&ptr, ":\t ");
	if (pname == NULL || *pname == 0 || ptr == NULL) {
		acl_msg_error("%s, %s(%d): invalid data=%s",
			__FILE__, myname, __LINE__, data);
		RETURN (NULL);
	}

	pvalue = ptr;

	while (*pvalue == ' ' || *pvalue == '\t' || *pvalue == ':')
		pvalue++;

	if (*pvalue == 0) {
		RETURN (NULL);
	}

	entry = http_hdr_entry_build(pname, pvalue);
	RETURN (entry);
#ifdef	ACL_BCB_COMPILER
	return (NULL);
#endif
}

HTTP_HDR_ENTRY *http_hdr_entry_head(char *data)
{
/* data format: GET / HTTP/1.1 or 200 OK */
	const char *myname = "http_hdr_entry_head";
	char *ptr, *pname, *psep = NULL;
	HTTP_HDR_ENTRY *entry;

	pname = ptr = data;
	while (*ptr) {
		if (*ptr == ' ') {
			psep = ptr;
			*ptr++ = 0;
			break;
		}
		ptr++;
	}
	if (*ptr == 0) {
		acl_msg_error("%s, %s(%d): invalid data=%s",
			__FILE__, myname, __LINE__, data);
		return (NULL);
	}
	while (*ptr == ' ' || *ptr == '\t')
		ptr++;
	if (*ptr == 0) {
		if (psep)
			*psep = ' ';
		acl_msg_error("%s, %s(%d): invalid data=%s",
			__FILE__, myname, __LINE__, data);
		return (NULL);
	}

#if 1
	entry = (HTTP_HDR_ENTRY*) acl_mymalloc(sizeof(HTTP_HDR_ENTRY));
	entry->off = 0;
	entry->name = acl_mystrdup(pname);
	entry->value = acl_mystrdup(ptr);
#else
	entry = http_hdr_entry_build(pname, ptr);
#endif
	return (entry);
}

HTTP_HDR_ENTRY *http_hdr_entry_new2(char *data)
{
/* data format: Content-Length: 245 */
	const char *myname = "http_hdr_entry_new2";
	char *ptr, *pname, *psep = NULL;
	HTTP_HDR_ENTRY *entry;

	pname = ptr = data;
	while (*ptr) {
		if (*ptr == ':') {
			psep = ptr;
			*ptr++ = 0;
			break;
		}
		ptr++;
	}
	if (*ptr == 0) {
		acl_msg_error("%s, %s(%d): invalid data=%s",
			__FILE__, myname, __LINE__, data);
		return (NULL);
	}
	while (*ptr == ':' || *ptr == ' ' || *ptr == '\t')
		ptr++;
	if (*ptr == 0) {
		if (psep)
			*psep = ':';
		acl_msg_error("%s, %s(%d): invalid data=%s",
			__FILE__, myname, __LINE__, data);
		return (NULL);
	}

#if 1
	entry = (HTTP_HDR_ENTRY*) acl_mymalloc(sizeof(HTTP_HDR_ENTRY));
	entry->off = 0;
	entry->name = acl_mystrdup(pname);
	entry->value = acl_mystrdup(ptr);
#else
	entry = http_hdr_entry_build(pname, ptr);
#endif
	return (entry);
}

/* 将 HTTP_HDR_ENTRY 放入 HTTP_HDR 中 */

void http_hdr_append_entry(HTTP_HDR *hh, HTTP_HDR_ENTRY *entry)
{
	const char *myname = "http_hdr_append_entry";

	if (entry == NULL)
		acl_msg_fatal("%s, %s(%d): entry null", __FILE__,
				myname, __LINE__);

	if (hh == NULL)
		acl_msg_fatal("%s, %s(%d): hh null",
				__FILE__, myname, __LINE__);

	if (hh->entry_lnk == NULL) {
		acl_msg_fatal("%s, %s(%d): entry_lnk null",
				__FILE__, myname, __LINE__);
	}

	if (acl_array_append(hh->entry_lnk, entry) < 0) {
		acl_msg_fatal("%s, %s(%d): acl_array_append error(%s)",
			__FILE__, myname, __LINE__, acl_last_serror());
	}
}

int http_hdr_parse_version(HTTP_HDR *hh, const char *data)
{
	const char *myname = "http_hdr_parse_version";

	if (hh == NULL)
		acl_msg_fatal("%s, %s(%d): hh null",
				__FILE__, myname, __LINE__);

	if (data == NULL || *data == 0)
		return (-1);

	if (strncasecmp(data, "HTTP/", 5) != 0)
		return (-1);
	if (sscanf(data + 5, "%u.%u", &hh->version.major, &hh->version.minor) != 2)
		return (-1);

	ACL_SAFE_STRNCPY(hh->proto, "HTTP", sizeof(hh->proto));

	return (0);
}

/*----------------------------------------------------------------------------*/

/* 根据变量名取得后面的整个数据项 */

static HTTP_HDR_ENTRY *__get_hdr_entry(const HTTP_HDR *hh, const char *name)
{
	const char *myname = "__get_hdr_entry";
	HTTP_HDR_ENTRY *entry;
	int  i, n;

	if (hh == NULL || name == NULL || *name == 0) {
		acl_msg_error("%s, %s(%d): input invalid",
				__FILE__, myname, __LINE__);
		return (NULL);
	}

	if (hh->entry_lnk == NULL)
		acl_msg_fatal("%s, %s(%d): entry_lnk null",
			__FILE__, myname, __LINE__);

	n = acl_array_size(hh->entry_lnk);
	if (n <= 0) {
		acl_msg_error("%s, %s(%d): entry_lnk size %d, name=%s",
			__FILE__, myname, __LINE__, n, name);
		return (NULL);
	}

	for (i = 0; i < n; i++) {
		entry = (HTTP_HDR_ENTRY *) acl_array_index(hh->entry_lnk, i);
		if (entry == NULL || entry->off)
			break;
		if (strcasecmp(name, entry->name) == 0)
			return (entry);
	}

	return (NULL);
}

HTTP_HDR_ENTRY *http_hdr_entry(const HTTP_HDR *hh, const char *name)
{
	return (__get_hdr_entry(hh, name));
}

char *http_hdr_entry_value(const HTTP_HDR *hh, const char *name)
{
	HTTP_HDR_ENTRY *hdr_entry;

	hdr_entry = http_hdr_entry(hh, name);
	if (hdr_entry == NULL)
		return (NULL);
	return (hdr_entry->value);
}

int http_hdr_entry_replace(HTTP_HDR *hh, const char *name,
	const char *value, int force)
{
	const char *myname = "http_hdr_entry_replace";
	HTTP_HDR_ENTRY *entry;

	if (hh == NULL || name == NULL || value == NULL) {
		acl_msg_fatal("%s, %s(%d): input invalid",
			__FILE__, myname, __LINE__);
	}

	entry = __get_hdr_entry(hh, name);
	if (entry == NULL) {
		if (force == 0)
			return (-1);
		entry = (HTTP_HDR_ENTRY*) acl_mymalloc(sizeof(HTTP_HDR_ENTRY));
		entry->off = 0;
		entry->name = acl_mystrdup(name);
		http_hdr_append_entry(hh, entry);
	} else {
		acl_myfree(entry->value);
	}

	entry->value = acl_mystrdup(value);
	return (0);
}

int http_hdr_entry_replace2(HTTP_HDR *hh, const char *name,
	const char *from, const char *to, int ignore_case)
{
	HTTP_HDR_ENTRY *entry;
	ACL_ITER iter;
	int   once, n = 0, len = (int) strlen(from);
	char *(*find_fn)(char *, const char*);

	if (strcasecmp(name, "Set-Cookie") == 0)
		once = 0;
	else
		once = 1;
	if (ignore_case)
		find_fn = acl_strcasestr;
	else
		find_fn = (char *(*)(char*, const char*)) strstr;

	acl_foreach(iter, hh->entry_lnk) {
		entry = (HTTP_HDR_ENTRY*) iter.data;
		if (strcasecmp(entry->name, name) == 0) {
			ACL_VSTRING *value = acl_vstring_alloc(256);
			char *ptr_prev = entry->value, *ptr;

			while (*ptr_prev) {
				ptr = find_fn(ptr_prev, from);
				if (ptr == NULL) {
					acl_vstring_strcat(value, ptr_prev);
					break;
				}
				if (ptr > ptr_prev)
					acl_vstring_strncat(value, ptr_prev,
						ptr - ptr_prev);
				acl_vstring_strcat(value, to);
				ptr_prev = ptr + len;
				n++;
			}
			if (n > 0) {
				acl_myfree(entry->value);
				entry->value = acl_vstring_export(value);
			} else {
				acl_vstring_free(value);
			}
			if (once)
				break;
		}
	}

	return (n);
}

void http_hdr_entry_off(HTTP_HDR *hh, const char *name)
{
	const char *myname = "http_hdr_entry_off";
	HTTP_HDR_ENTRY *entry;

	if (hh == NULL || name == NULL)
		acl_msg_fatal("%s, %s(%d): input invalid",
			__FILE__, myname, __LINE__);

	entry = __get_hdr_entry(hh, name);
	if (entry == NULL)
		return;

	/* xxx: 腾讯的视频WEB服务器不支持此种方式 -- 2008-7-23, zsx */
	/* entry->name[0] = '_'; */
	entry->off = 1;
}

int http_hdr_parse(HTTP_HDR *hh)
{
	const char *myname = "http_hdr_parse";
	int   keep_alive = -1;
	HTTP_HDR_ENTRY *entry;
	ACL_ITER  iter;

	struct HEADER_LIST {
		char *name;
	};

#if 0
	static const struct HEADER_LIST header_list[] = {
		{ "Accept-Ranges" },
		{ "Age" },
		{ "Cache-Control" },
		{ "Connection" },
		{ "Content-Encoding" },
		{ "Content-Length" },
		{ "Content-Type" },
		{ "Date" },
		{ "ETag" },
		{ "Last-Modified" },
		{ "Server" },
		{ "Transfer-Encoding" },
	};
	int care_size = sizeof(header_list)/sizeof(struct HEADER_LIST), care_cnt = 0, i, n;

	n = acl_array_size(hh->entry_lnk);
	for (i = 0; i < n; i++) {
		if (care_cnt >= care_size)
			break;

		entry = (HTTP_HDR_ENTRY *) acl_array_index(hh->entry_lnk, i);

		if (strcasecmp(entry->name, "Accept-Ranges") == 0) {
			care_cnt++;
		} else if (strcasecmp(entry->name, "Age") == 0) {
			care_cnt++;
		} else if (strcasecmp(entry->name, "Cache-Control") == 0) {
			care_cnt++;
		} else if (strcasecmp(entry->name, "Connection") == 0) {
			care_cnt++;
			if (strcasecmp(entry->value, "keep-alive") == 0)
				keep_alive = 1;
			else
				keep_alive = 0;
		} else if (strcasecmp(entry->name, "Proxy-Connection") == 0) {
			care_cnt++;
			if (strcasecmp(entry->value, "keep-alive") == 0)
				keep_alive = 1;
			else
				keep_alive = 0;
		} else if (strcasecmp(entry->name, "Content-Encoding") == 0) {
			care_cnt++;
		} else if (strcasecmp(entry->name, "Content-Length") == 0) {
#ifdef MS_VC6
			hh->content_length = acl_atoui64(entry->value);
#else
			hh->content_length = acl_atoui64(entry->value) * 1ll;
#endif
			if (hh->content_length < 0) {
				acl_msg_error("%s: content_length(%s) invalid",
						myname, entry->value);
				return (-1);
			}
			care_cnt++;
		} else if (strcasecmp(entry->name, "Content-Type") == 0) {
			care_cnt++;
		} else if (strcasecmp(entry->name, "Date") == 0) {
			care_cnt++;
		} else if (strcasecmp(entry->name, "ETag") == 0) {
			care_cnt++;
		} else if (strcasecmp(entry->name, "Last-Modified") == 0) {
			care_cnt++;
		} else if (strcasecmp(entry->name, "Server") == 0) {
			care_cnt++;
		} else if (strcasecmp(entry->name, "Transfer-Encoding") == 0) {
			if (strcasecmp(entry->value, "chunked") == 0)
				hh->chunked = 1;
			care_cnt++;
		}
	}
#else
	static const struct HEADER_LIST header_list[] = {
		{ "Connection" },
		{ "Proxy-Connection" },
		{ "Content-Length" },
		{ "Transfer-Encoding" },
	};
	int care_size = sizeof(header_list)/sizeof(struct HEADER_LIST), care_cnt = 0;

	acl_foreach(iter, hh->entry_lnk) {
		entry = (HTTP_HDR_ENTRY *) iter.data;

		if (strcasecmp(entry->name, "Connection") == 0) {
			if (strcasecmp(entry->value, "keep-alive") == 0)
				keep_alive = 1;
			else
				keep_alive = 0;
			care_cnt++;
		} else if (strcasecmp(entry->name, "Proxy-Connection") == 0) {
			if (strcasecmp(entry->value, "keep-alive") == 0)
				keep_alive = 1;
			else
				keep_alive = 0;
			care_cnt++;
		} else if (strcasecmp(entry->name, "Content-Length") == 0) {
#ifdef MS_VC6
			hh->content_length = acl_atoui64(entry->value);
#else
			hh->content_length = acl_atoui64(entry->value) * 1ll;
#endif
			if (hh->content_length < 0) {
				acl_msg_error("%s: content_length(%s) invalid",
						myname, entry->value);
				return (-1);
			}
			care_cnt++;
		} else if (strcasecmp(entry->name, "Transfer-Encoding") == 0) {
			if (strcasecmp(entry->value, "chunked") == 0)
				hh->chunked = 1;
			care_cnt++;
		}
		if (care_cnt >= care_size)
			break;
	}
#endif

	if (hh->chunked) {
		if (keep_alive != 0)
			hh->keep_alive = 1;
		else
			hh->keep_alive = 0;
	} else if (keep_alive == 1) {
		hh->keep_alive = 1;
	} else {
		hh->keep_alive = 0;
	}

	return (0);
}

void http_hdr_print(const HTTP_HDR *hh, const char *msg)
{
	const char *myname = "http_hdr_print";
	HTTP_HDR_ENTRY *entry;
	int   n, i;

	if (hh == NULL)
		acl_msg_fatal("%s, %s(%d): invalid input",
				__FILE__, myname, __LINE__);
	n = acl_array_size(hh->entry_lnk);
	if (n <= 0) {
		acl_msg_info("%s, %s(%d): array empty",
				__FILE__, myname, __LINE__);
		return;
	}

	printf("------------- in %s - msg=(%s)----------------\r\n",
		myname, msg ? msg : "null");
	entry = (HTTP_HDR_ENTRY *) acl_array_index(hh->entry_lnk, 0);
	if (entry)
		printf("%s %s\r\n", entry->name, entry->value);
	for (i = 1; i < n; i++) {
		entry = (HTTP_HDR_ENTRY *) acl_array_index(hh->entry_lnk, i);
		if (entry == NULL)
			break;
		printf("%s: %s\r\n", entry->name, entry->value);
	}
	printf("--------------- end -----------------\r\n");
}

void http_hdr_fprint(ACL_VSTREAM *fp, const HTTP_HDR *hh, const char *msg)
{
	const char *myname = "http_hdr_fprint";
	HTTP_HDR_ENTRY *entry;
	int   n, i;

	if (fp == NULL || hh == NULL) {
		acl_msg_error("%s(%d): input invalid", myname, __LINE__);
		return;
	}

	n = acl_array_size(hh->entry_lnk);
	if (n <= 0) {
		acl_msg_info("%s, %s(%d): array empty",	myname, __FILE__, __LINE__);
		return;
	}

	acl_vstream_fprintf(fp, "------------- in %s - (%s)----------------\r\n",
		myname, msg ? msg : "begin");
	entry = (HTTP_HDR_ENTRY *) acl_array_index(hh->entry_lnk, 0);
	if (entry)
		acl_vstream_fprintf(fp, "%s %s\r\n", entry->name, entry->value);
	for (i = 1; i < n; i++) {
		entry = (HTTP_HDR_ENTRY *) acl_array_index(hh->entry_lnk, i);
		if (entry == NULL)
			break;
		acl_vstream_fprintf(fp, "%s: %s\r\n", entry->name, entry->value);
	}
	acl_vstream_fprintf(fp, "--------------- end -----------------\r\n");
}

