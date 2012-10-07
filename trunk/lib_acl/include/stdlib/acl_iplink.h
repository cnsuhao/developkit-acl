/*
 * Name: iplink.h
 * Author: zsx
 * Date: 2003/11/30
 * Version: 1.0
 *
*/
#ifndef	_IPLINK_INCLUDE_H_
#define	_IPLINK_INCLUDE_H_

#ifdef  __cplusplus
extern "C" {
#endif

#include "acl_define.h"
#include "acl_array.h"
#include "acl_iterator.h"

#ifndef	IPLEN
#define	IPLEN	32
#endif

#define	F_ITEM_NEW	1
#define	F_ITEM_OLD	2

#define	IS_NEW_ITEM(x)	(x->flag == F_ITEM_NEW)
#define	SET_ITEM_NEW(x)	(x->flag = F_ITEM_NEW)
#define	SET_ITEM_OLD(x)	(x->flag = F_ITEM_OLD)

typedef	struct {
	unsigned int ip_begin;
	unsigned int ip_end;
	int	flag;	/* 是否是一个新的结点, defined avbove: F_ITEM_ */
	void *pnode;
} ACL_IPItem;

typedef	struct ACL_IPLink {
	ACL_ARRAY	*parray;
	void	*call_back_data;

	/* for acl_iterator */

	/* 取迭代器头函数 */
	void *(*iter_head)(ACL_ITER*, struct ACL_IPLink*);
	/* 取迭代器下一个函数 */
	void *(*iter_next)(ACL_ITER*, struct ACL_IPLink*);
	/* 取迭代器尾函数 */
	void *(*iter_tail)(ACL_ITER*, struct ACL_IPLink*);
	/* 取迭代器上一个函数 */
	void *(*iter_prev)(ACL_ITER*, struct ACL_IPLink*);
} ACL_IPLink;

ACL_API ACL_IPLink *acl_iplink_create(int nsize);

ACL_API void acl_iplink_free(ACL_IPLink *plink);

ACL_API const ACL_IPItem *acl_iplink_lookup_item(const ACL_IPLink *plink, const ACL_IPItem *pitem);

ACL_API ACL_IPItem *acl_iplink_lookup_bin(const ACL_IPLink *plink, unsigned int ip);

ACL_API ACL_IPItem *acl_iplink_lookup_str(const ACL_IPLink *plink, const char *pstrip);

ACL_API ACL_IPItem *acl_iplink_insert_bin(ACL_IPLink *plink, unsigned int ip_begin, unsigned int ip_end);
ACL_API ACL_IPItem *acl_iplink_insert(ACL_IPLink *plink, const char *pstrip_begin, const char *pstrip_end);

ACL_API int acl_iplink_delete_by_ip(ACL_IPLink *plink, const char *pstrip_begin);

ACL_API int acl_iplink_delete_by_item(ACL_IPLink *plink, ACL_IPItem *pitem);

ACL_API ACL_IPItem *acl_iplink_modify(	ACL_IPLink	*plink,
			const	char *pstrip_id,
			const	char *pstrip_begin,
			const	char *pstrip_end );

ACL_API ACL_IPItem *acl_iplink_select_item(ACL_IPLink *plink, int idx);

ACL_API int acl_iplink_count_item(ACL_IPLink *plink);

ACL_API int acl_iplink_list(const ACL_IPLink *plink);

#ifdef  __cplusplus
}
#endif

#endif

