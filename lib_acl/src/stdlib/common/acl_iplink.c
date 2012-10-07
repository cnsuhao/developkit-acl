#include "StdAfx.h"
#ifndef ACL_PREPARE_COMPILE

#include "stdlib/acl_define.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef ACL_UNIX
#include <ctype.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#endif

#ifdef ACL_BCB_COMPILER
#pragma hdrstop
#endif

#include "stdlib/acl_mymalloc.h"
#include "stdlib/acl_mystring.h"
#include "stdlib/acl_msg.h"
#include "stdlib/acl_array.h"
#include "stdlib/acl_iplink.h"

#endif

static char *__sane_inet_ntoa(unsigned int src, char *dst, int size);

static int ip_scope_pos(const ACL_ARRAY *a, const unsigned int ip);
static int ip_begin_pos(const ACL_ARRAY *a, const unsigned int ip_begin);
static int ip_end_pos(const ACL_ARRAY *a, const unsigned int ip_end);
#ifdef	_USE_PRED_INSERT_
static ACL_IPItem *acl_iplink_pred_insert(	ACL_ARRAY *a,
					const int idx_position,
					const unsigned int ip_begin,
					const unsigned int ip_end );
#endif
static ACL_IPItem *acl_iplink_succ_insert(	ACL_ARRAY *a,
					const int idx_position,
					const unsigned int ip_begin,
					const unsigned int ip_end );
static ACL_IPItem *acl_iplink_append(	ACL_ARRAY *a,
				const unsigned int ip_begin,
				const unsigned int ip_end );
static ACL_IPItem *acl_iplink_prepend(	ACL_ARRAY *a,
				const unsigned int ip_begin,
				const unsigned int ip_end );

static int acl_iplink_node_merge(	ACL_ARRAY *a,
				const int idx_obj_begin,
				const int idx_src_begin);
static ACL_IPItem *acl_iplink_add(	ACL_ARRAY *a,
				const unsigned int ip_begin,
				const unsigned int ip_end );

static int _check_ip_addr(const char *pstrip)
{
	const	char	*ptr;
	int	count = 0;

	if (pstrip == NULL || *pstrip == 0)
		return(-1);

	ptr = pstrip;
	if(*ptr == '.')		/* the first char should not be '.' */
		return(-1);
	while(*ptr) {
		if (*ptr == '.') {
			if (!isdigit((int)*(ptr + 1)))
				return(-1);
			count++;
		} else if (!isdigit((int)*ptr))
			return(-1);
		ptr++;
	}
	if(*(ptr - 1) == '.')	/* the last char should not be '.' */
		return(-1);
	if(count != 3)		/* 192.168.0.1 has the number '.' is 4 */
		return(-1);

	return(0);
}

static void acl_iplink_free_callback(void *arg)
{
	ACL_IPItem	*pitem;

	if(arg == NULL)
		return;
	pitem = (ACL_IPItem *)arg;
	pitem->pnode = NULL;	/* sanity set to be null */
	
	acl_myfree(pitem);
}

/*
 * 功能:	找出某个长整数 IP 地址在数组中的下标范围位置
 * 参数:
 * a:		结构数组指针
 * ip:		长整数 IP 地址
 * 返回值:	下标索引, 该索引满足如下条件:
 *		(idx >= 0 && idx < a->count - 1
 *		 && ip >= a->items[idx]->ip_begin
 *		 && ip <  a->item[a->count - 1]->ip_begin )
 *		or (idx == 0 && ip < a->item[idx]->ip_begin)
 *		or (idx == a->count - 1 && ip >= a->item[idx]->ip_begin)
 * 失败:	-1
 * 成功:	>= 0
 * 说明:	if idx == 0 ----> 说明在数组的开始位置添加或在开始位置前插入
 * 		if idx > 0 && idx <= a->count - 1 -----> 说明在数组的中间的某一位置
 *		if idx > a->count - 1 -----> 说明在数组的最后一位置添加
 */
static int ip_scope_pos(const ACL_ARRAY *a, const unsigned int ip)
{
	ACL_IPItem	*pitem_left, *pitem_right;
	int	lidx, hidx, midx, ridx, idx;

	lidx  = 0;
#if 0
	hidx = a->count - 1;
#endif
	hidx = acl_array_size(a) - 1;
	ridx = 0;
	idx  = 0;
	while(lidx <= hidx) {
		/*
		 * find the postion where ip >= item_low->ip_begin
		 * and ip < item_high->ip_begin
		 * NOTICE:
		 * item_low->ip_begin <= item_low->ip_end < item_high->ip_begin !!!!
		 * because the ip-link is sorted correctly, or some error must happen
		 * if(item_low->ip_begin == item_high->ip_begin
		 *   || item_low->ip_end == item_high->ip_begin)
		 * then we should merge them first
		 */

		midx = (lidx + hidx)/2;
		ridx = midx + 1;
		if(ridx > hidx)	{
			/*
			 * here hidx == 0 or hidx == a->count - 1, we've been
			 * out of the search scope now, so break out of the loop
			 */

			if(hidx == 0)
				idx = 0;
#if 0
			else if (hidx == a->count - 1)
#endif
			else if (hidx == acl_array_size(a) - 1)
#if 0
				idx = a->count - 1;
#endif
				idx = acl_array_size(a) - 1;
			else	/* an error happens */
				idx = -1;
			break;
		}
#if 0
		pitem_left  = (ACL_IPItem *)a->items[midx];
		pitem_right = (ACL_IPItem *)a->items[ridx];
#endif
		pitem_left = (ACL_IPItem *) acl_array_index(a, midx);
		pitem_right = (ACL_IPItem *) acl_array_index(a, ridx);

		if(ip >= pitem_left->ip_begin && ip < pitem_right->ip_begin) {
			idx = midx;	/* find it :) */
			break;
		}
		/* not find, continue...... */
		if(ip >= pitem_right->ip_begin)
			lidx = ridx;
		else if(ip < pitem_left->ip_begin)
			hidx = midx;
		else {	/* why does the array not to be sorted ? */
			idx = -1;
			break;
		}
	}

	return(idx);
}

static int ip_begin_pos(const ACL_ARRAY *a, const unsigned int ip_begin)
{
	int	ridx;

	ridx = ip_scope_pos(a, ip_begin);
	return(ridx);
}

static int ip_end_pos(const ACL_ARRAY *a, const unsigned int ip_end)
{
	int	ridx;

	ridx = ip_scope_pos(a, ip_end);
	return(ridx);
}

#ifdef	_USE_PRED_INSERT_
static ACL_IPItem *acl_iplink_pred_insert(	ACL_ARRAY *a,
					const int idx_position,
					const unsigned int ip_begin,
					const unsigned int ip_end )
{
	ACL_IPItem	*pitem;
	int	ret;

	pitem = acl_mymalloc(sizeof(ACL_IPItem));
	if(pitem == NULL)
		return(NULL);
	pitem->ip_begin = ip_begin;
	pitem->ip_end   = ip_end;
	ret = acl_array_pred_insert(a, idx_position, pitem);
	if(ret < 0) {
		acl_myfree(pitem);
		return(NULL);
	}
	pitem->pnode = NULL;
	SET_ITEM_NEW(pitem);

	return(pitem);
}
#endif

static ACL_IPItem *acl_iplink_succ_insert(	ACL_ARRAY *a,
					const int idx_position,
					const unsigned int ip_begin,
					const unsigned int ip_end )
{
	ACL_IPItem	*pitem = NULL;
	int	ret;

	pitem = (ACL_IPItem *) acl_mymalloc(sizeof(ACL_IPItem));
	if(pitem == NULL)
		return(NULL);
	pitem->ip_begin = ip_begin;
	pitem->ip_end   = ip_end;
	ret = acl_array_succ_insert(a, idx_position, pitem);
	if(ret < 0) {
		acl_myfree(pitem);
		return(NULL);
	}
	pitem->pnode = NULL;
	SET_ITEM_NEW(pitem);

	return(pitem);
}

static ACL_IPItem *acl_iplink_append(	ACL_ARRAY *a,
				const unsigned int ip_begin,
				const unsigned int ip_end )
{
	ACL_IPItem	*pitem = NULL;
	int	ret;

	pitem = (ACL_IPItem *) acl_mymalloc(sizeof(ACL_IPItem));
	if(pitem == NULL)
		return(NULL);
	pitem->ip_begin = ip_begin;
	pitem->ip_end   = ip_end;
	ret = acl_array_append(a, pitem);
	if(ret < 0) {
		acl_myfree(pitem);
		return(NULL);
	}
	pitem->pnode = NULL;
	SET_ITEM_NEW(pitem);

	return(pitem);
}

static ACL_IPItem *acl_iplink_prepend(	ACL_ARRAY *a,
				const unsigned int ip_begin,
				const unsigned int ip_end )
{
	ACL_IPItem	*pitem = NULL;
	int	ret;

	pitem = (ACL_IPItem *) acl_mymalloc(sizeof(ACL_IPItem));
	if(pitem == NULL)
		return(NULL);
	pitem->ip_begin = ip_begin;
	pitem->ip_end   = ip_end;
	ret = acl_array_prepend(a, pitem);
	if(ret < 0) {
		acl_myfree(pitem);
		return(NULL);
	}
	pitem->pnode = NULL;
	SET_ITEM_NEW(pitem);

	return(pitem);
}

static int acl_iplink_node_merge(	ACL_ARRAY *a,
				const int idx_obj_begin,
				const int idx_src_begin)
{
	int	ret;

	if(idx_obj_begin >= idx_src_begin)
		return(0);

	ret = acl_array_mv_idx(a, idx_obj_begin, idx_src_begin, acl_iplink_free_callback);
	if(ret < 0)
		return(-1);

	return(0);
}

static ACL_IPItem *acl_iplink_add(	ACL_ARRAY *a,
				const unsigned int ip_begin,
				const unsigned int ip_end )
{
	ACL_IPItem	*pitem_right, *pitem_left, *pitem;
	int	idx_begin, idx_end;
	int	ret;

	/* sanity check, maybe useless */
	/* because it's used internal */
	if(ip_begin > ip_end)
		return(NULL);

	idx_begin = ip_begin_pos(a, ip_begin);
#if 0
	if(idx_begin < 0 || idx_begin >= a->count)	/* an error happened */
#endif
	if(idx_begin < 0 || idx_begin >= acl_array_size(a))	/* an error happened */
		return(NULL);

	idx_end   = ip_end_pos(a, ip_end);
#if 0
	if(idx_end < 0 || idx_end >= a->count)		/* an error happened */
#endif
	if(idx_end < 0 || idx_end >= acl_array_size(a))	/* an error happened */
		return(NULL);

	if(idx_begin > idx_end)	/* an error happened */
		return(NULL);

#if 0
	if(a->count == 0) {	/* the ip-link is empty so just add one :) */
#endif
	if(acl_array_size(a) == 0) {	/* the ip-link is empty so just add one :) */
		pitem = acl_iplink_append(a, ip_begin, ip_end);
		return(pitem);
	}

#if 0
	pitem_left  = (ACL_IPItem *)a->items[idx_begin];
	pitem_right = (ACL_IPItem *)a->items[idx_end];
#endif
	pitem_left  = (ACL_IPItem *) acl_array_index(a, idx_begin);
	pitem_right = (ACL_IPItem *) acl_array_index(a, idx_end);

	/* if idx_end == 0 then idx_begin must be equal to 0, I'm sure it :) */
	if (idx_begin == idx_end) {
		/*
		 * pitem_left == pitem_right
		 * here idx_begin maybe one of: 0, a->count - 1,
		 * or the one between 0 and a->count
		 * this is to say the ip_begin and ip_end is on the same ip-link
		 */

		if (ip_end < pitem_left->ip_begin) {
			/*
			 * here idx_begin == idx_end must be equal to 0
			 * the ip_begin and the ip_end must be less
			 * than the next node's ip_begin
			 * add one new node before the one
			 */
			pitem = acl_iplink_prepend(a, ip_begin, ip_end);
			return(pitem);
		}

		if (ip_begin > pitem_left->ip_end) {
			/*
			 * this is to say ip_begin and ip_end
			 * between the current node's ip_end
			 * the next node's ip_begin, and we just
			 * insert one new node between the
			 * current node and the next node, when
			 * the next node is NULL(which say that
			 * idx_begin == idx_end == a->count - 1),
			 * just insert one new node after
			 * the the last node
			 */
			pitem = acl_iplink_succ_insert(a, idx_begin, ip_begin, ip_end);
			return(pitem);
		}

		/*
		 * here idx_begin == idx_end must in the middle
		 * here ip_begin <= pitem_left->ip_end,
		 * and ip_end < the next node's ip_begin,
		 * and ip_end >= pitem_left->ip_begin
		 */

		if (ip_begin < pitem_left->ip_begin)
			/*
			 * just merge, which happens when
			 * idx_begin == idx_end == 0 && ip_begin < pitem_left->ip_begin
			 * && ip_end >= pitem_left->ip_begin
			 */
			pitem_left->ip_begin = ip_begin;

		if (ip_end > pitem_left->ip_end)
			/*
			 * just merge, pitem_left->ip_begin <= pitem_left->ip_end
			 * pitem_left->ip_begin <= ip_end < pitem_right->ip_end
			 * or pitem_right == NULL
			 */
			pitem_left->ip_end = ip_end;

		return(pitem_left);
	}

	/*
	 * idx_end > idx_begin, idx_begin >= 0, idx_end <= a->count - 1
	 * ===> 0 <= idx_begin < idx_end <= a->count - 1;
	 *
	 * idx_end > idx_begin, so the ip_begin and ip_end
	 * are separately in defferent node,
	 * because the ip-link is sored correctly:).
	 * ===> I just only merge them--the reason is shown below:
	 * idx_end > idx_begin, ip-link is sorted correctly
	 * ===>	pitem_left->ip_begin < pitem_right->ip_begin <= p_end,
	 *	ip_begin < pitem_right->ip_begin;
	 */

	if(ip_begin < pitem_left->ip_begin) {
		/*
		 * in the first position of the array
		 * idx_begin == 0 and idx_end >= 1
		 * just merge :)
		 */

		pitem_left->ip_begin = ip_begin;
	} 

	/*
	 * ===>  pitem_left->ip_begin
	 * ===>  <= ip_begin
	 * ===>  ......
	 * ===>  <  pitem_right->ip_begin
	 * ===>  <= ip_end
	 */

	if(ip_begin <= pitem_left->ip_end) {
		/*
		 * ===>  pitem_left->ip_begin
		 * ===>  <= ip_begin
		 * +++>  <= pitem_left->ip_end
		 * ===>  <  pitem_right->ip_begin
		 * ===>  <= ip_end
		 * ===>  so, just merge the nodes between the
		 * ===>  pitem_left node and the pitem_right node, and include both of them
		 */

		/*
		 * merge the pitem_left node, ip_begin ---> ip_end into pitem_right node,
		 * and merge all nodes into one node which are between 
		 * the pitem_left node and the pitem_right node,
		 * include both of pitem_left node and pitem_right node
		 */

		if(ip_end > pitem_right->ip_end) {
			/*
			 * ===>  pitem_left->ip_begin
			 * ===>  <= ip_begin
			 * ===>  <= pitem_left->ip_end
			 * ===>  <  pitem_right->ip_begin
			 * +++>  <= pitem_right->ip_end
			 * ===>  <  ip_end
			 */

			pitem_right->ip_end = ip_end;
		}
		/*
		 * else
		 * ===>  pitem_left->ip_begin
		 * ===>  <= ip_begin
		 * ===>  <= pitem_left->ip_end
		 * ===>  <  pitem_right->ip_begin
		 * ===>  <= ip_end
		 * +++>  <= pitem_right->ip_end
		 */

		pitem_right->ip_begin = pitem_left->ip_begin;

		ret = acl_iplink_node_merge(a, idx_begin, idx_end);
		if(ret < 0)
			return(NULL);

		return(pitem_right);
	}

	/*
	 * ===>  pitem_left->ip_begin
	 * ===>  <= pitem_left->ip_end
	 * ===>  <  ip_begin
	 * ===>  <  pitem_right->ip_begin
	 * ===>  <= ip_end
	 *
	 * merge the ip_begin--->ip_end into pitem_right node
	 * and at the same time, merge all the nodes into idx_begin + 1
	 * which between the idx_begin + 1 node and the pitem_right node,
	 * include idx_begin + 1 node and pitem_righ node
	 */

	if(ip_end > pitem_right->ip_end) {
		/*
		 * ===>  pitem_left->ip_begin
		 * ===>  <= pitem_left->ip_end
		 * ===>  <  ip_begin
		 * ===>  <  pitem_right->ip_begin
		 * +++>  <= pitem_right->ip_end
		 * ===>  <  ip_end
		 */

		pitem_right->ip_end = ip_end;
	}
	/*
	 * else
	 * ===>  pitem_left->ip_begin
	 * ===>  <= pitem_left->ip_end
	 * ===>  <  ip_begin
	 * ===>  <  pitem_right->ip_begin
	 * ===>  <  ip_end
	 * +++>  <= pitem_right->ip_end
	 */

	pitem_right->ip_begin = ip_begin;

	ret = acl_iplink_node_merge(a, idx_begin + 1, idx_end);
	if(ret < 0)
		return(NULL);

	return(pitem_right);
}

static void *iplink_iter_head(ACL_ITER *iter, struct ACL_IPLink *iplink)
{
	return (iplink->parray->iter_head(iter, iplink->parray));
}

static void *iplink_iter_next(ACL_ITER *iter, struct ACL_IPLink *iplink)
{
	return (iplink->parray->iter_next(iter, iplink->parray));
}

static void *iplink_iter_tail(ACL_ITER *iter, struct ACL_IPLink *iplink)
{
	return (iplink->parray->iter_tail(iter, iplink->parray));
}

static void *iplink_iter_prev(ACL_ITER *iter, struct ACL_IPLink *iplink)
{
	return (iplink->parray->iter_prev(iter, iplink->parray));
}

ACL_IPLink *acl_iplink_create(int nsize)
{
	ACL_IPLink	*plink = NULL;

	plink = (ACL_IPLink *) acl_mymalloc(sizeof(ACL_IPLink));
	plink->parray = NULL;
	plink->call_back_data = NULL;
	nsize = nsize > 0 ? nsize : 1;
	plink->parray = acl_array_create(nsize);
	if(plink->parray == NULL) {
		acl_myfree(plink);
		return(NULL);
	}

	plink->iter_head = iplink_iter_head;
	plink->iter_next = iplink_iter_next;
	plink->iter_tail = iplink_iter_tail;
	plink->iter_prev = iplink_iter_prev;

	return(plink);
}

void acl_iplink_free(ACL_IPLink *plink)
{
	if(plink == NULL)
		return;
	if(plink->parray) {
		acl_array_destroy(plink->parray, acl_iplink_free_callback);
		plink->parray = NULL;
	}
	acl_myfree(plink);
}

const ACL_IPItem *acl_iplink_lookup_item(const ACL_IPLink *plink, const ACL_IPItem *pitem)
{
	int	i;
	ACL_ARRAY	*parray;

	if(plink == NULL || pitem == NULL)
		return(NULL);
	parray = plink->parray;
	if(parray == NULL)
		return(NULL);
#if 0
	for(i = 0; i < parray->count - 1; i++) {
#endif
	for(i = 0; i < acl_array_size(parray) - 1; i++) {
#if 0
		if((ACL_IPItem *)parray->items[i] == pitem)
#endif
		if((ACL_IPItem *) acl_array_index(parray, i) == pitem)
			return(pitem);
	}
	return(NULL);
}

ACL_IPItem *acl_iplink_lookup_bin(const ACL_IPLink *plink, unsigned int ip)
{
	ACL_ARRAY	*parray;
	ACL_IPItem	*pitem;
	int	lidx, midx, hidx;

	if(plink == NULL || (parray = plink->parray) == NULL)
		return(NULL);

	lidx = 0;
#if 0
	hidx = parray->count - 1;
#endif
	hidx = acl_array_size(parray) - 1;
	while(lidx <= hidx) {
		midx  = (lidx + hidx)/2;
#if 0
		pitem = (ACL_IPItem *)parray->items[midx];
#endif
		pitem = (ACL_IPItem *) acl_array_index(parray, midx);
		if(ip >= pitem->ip_begin && ip <= pitem->ip_end)
			return(pitem);
		if(ip < pitem->ip_begin)
			hidx = midx - 1;
		else if(ip > pitem->ip_end)
			lidx = midx + 1;
		else			/* why does this array not to be sorted ? */
			break;
	}

	return(NULL);	/*not in the ip_link scope */
}

ACL_IPItem *acl_iplink_lookup_str(const ACL_IPLink *plink, const char *pstrip)
{
	ACL_IPItem	*pitem;
	unsigned int ip;

	if(plink == NULL || plink->parray == NULL || pstrip == NULL)
		return(NULL);

	ip = ntohl(inet_addr(pstrip));
	pitem = acl_iplink_lookup_bin(plink, ip);

	return(pitem);
}

ACL_IPItem *acl_iplink_insert_bin(ACL_IPLink *plink,
				unsigned int ip_begin,
				unsigned int ip_end )
{
	ACL_ARRAY	*parray;
	ACL_IPItem	*pitem;
	unsigned int ip_tmp;

	if(plink == NULL)
		printf("plink null\n");

	if (plink == NULL || (parray = plink->parray) == NULL)
		return(NULL);

	if (ip_begin > ip_end) {
		/* swap the ip_begin and ip_end if ip_end < ip_begin */
		ip_tmp   = ip_begin;
		ip_begin = ip_end;
		ip_end   = ip_tmp;
	}

	if(acl_array_size(parray) == 0)	{
		/* this is the first item of the array */
		pitem = acl_iplink_append(parray, ip_begin, ip_end);
		return(pitem);
	}
	pitem = acl_iplink_add(parray, ip_begin, ip_end);
	/* 此函数内有可能进行了结点项的合并 */

	return(pitem);
}

ACL_IPItem *acl_iplink_insert(	ACL_IPLink *plink,
			const char *pstrip_begin,
			const char *pstrip_end )
{
	const char *myname = "acl_iplink_insert";
	unsigned int ip_begin, ip_end;

	if(plink == NULL) {
		acl_msg_error("%s: plink null", myname);
		return (NULL);
	}

	if (plink->parray == NULL) {
		acl_msg_error("%s: array null", myname);
		return(NULL);
	}

	if (_check_ip_addr(pstrip_begin) < 0) {
		acl_msg_error("%s: invalid ip begin(%s)", myname, pstrip_begin);
		return(NULL);
	}
	if (_check_ip_addr(pstrip_end) < 0) {
		acl_msg_error("%s: invalid ip end(%s)", myname, pstrip_end);
		return (NULL);
	}

	ip_begin = ntohl(inet_addr(pstrip_begin));
	ip_end   = ntohl(inet_addr(pstrip_end));

	return (acl_iplink_insert_bin(plink, ip_begin, ip_end));
}

int acl_iplink_delete_by_ip(ACL_IPLink *plink, const char *pstrip_begin)
{
	ACL_IPItem	*pitem;
	ACL_ARRAY	*parray;
	int	ret;

	if (plink == NULL || (parray = plink->parray) == NULL)
		return(-1);
	if (_check_ip_addr(pstrip_begin) < 0)
		return(-1);

	pitem = acl_iplink_lookup_str(plink, pstrip_begin);
	if (pitem == NULL)
		return(-1);
	ret = acl_array_delete_obj(parray, pitem, acl_iplink_free_callback);
	if (ret < 0)	/* this is impossile, but a sanity check */
		return(-1);
	return(0);
}

int acl_iplink_delete_by_item(ACL_IPLink *plink, ACL_IPItem *pitem)
{
	ACL_ARRAY	*parray;
	int	ret;

	if(plink == NULL || (parray = plink->parray) == NULL || pitem == NULL)
		return(-1);
	ret = acl_array_delete_obj(parray, pitem, acl_iplink_free_callback);
	if (ret < 0)	/* this is impossile, but a sanity check */
		return(-1);
	return(0);
}

ACL_IPItem *acl_iplink_modify(	ACL_IPLink	*plink,
			const	char *pstrip_id,
			const	char *pstrip_begin,
			const	char *pstrip_end )
{
	ACL_IPItem	*pitem = NULL;
	ACL_ARRAY	*parray;
	unsigned int ip_begin, ip_end, ip_tmp;

	if (plink == NULL || (parray = plink->parray) == NULL)
		return(NULL);

	if (_check_ip_addr(pstrip_id) < 0
	    || _check_ip_addr(pstrip_begin) < 0
	    || _check_ip_addr(pstrip_end) < 0)
		return(NULL);

	ip_begin = ntohl(inet_addr(pstrip_begin));
	ip_end   = ntohl(inet_addr(pstrip_end));
	if (ip_begin > ip_end) {
		/* swap the ip_begin and ip_end if ip_end < ip_begin */
		ip_tmp   = ip_begin;
		ip_begin = ip_end;
		ip_end   = ip_tmp;
	}

	/*
	 * 先删除这个结点, 然后再增加一个结点,
	 * 这样做主要是为了当管理员将一个 IP
	 * 地址段减小时而进行的改变
	 * ret = acl_iplink_delete_by_ip(plink, pstrip_id);
	 * if(ret < 0)
	 *	return(NULL);
	 */

	pitem = acl_iplink_add(parray, ip_begin, ip_end);

	return(pitem);
}

ACL_IPItem *acl_iplink_select_item(ACL_IPLink *plink, int idx)
{
	ACL_ARRAY	*parray;
	ACL_IPItem	*pitem;

	if(plink == NULL || (parray = plink->parray) == NULL)
		return(NULL);
	pitem = (ACL_IPItem *)acl_array_index(parray, idx);

	return(pitem);
}

int acl_iplink_count_item(ACL_IPLink *plink)
{
	int	ret;
	ACL_ARRAY	*parray;

	if(plink == NULL || (parray = plink->parray) == NULL)
		return(-1);
	ret = acl_array_size(parray);
	if(ret < 0)
		return(-1);
	return(ret);
}

/* ++++++++++++++++++++++++++below functions are used only for test ++++++++++++ */
static char *__sane_inet_ntoa(unsigned int src, char *dst, int size)
{
	unsigned char *bytes;
	struct in_addr in;

	if (size < 16)
		return(NULL);

	/* XXX: may be bugfix here---zsx */
/*	in.s_addr = ntohl(src); */
	in.s_addr = src;
	bytes = (unsigned char *) &in;
#ifdef  LINUX2
	snprintf (dst, 18, "%d.%d.%d.%d", bytes[3], bytes[2], bytes[1], bytes[0]);
#elif defined(SUNOS5)
	snprintf (dst, 18, "%d.%d.%d.%d", bytes[0], bytes[1], bytes[2], bytes[3]);
#elif defined(HP_UX)
	snprintf (dst, 18, "%d.%d.%d.%d", bytes[0], bytes[1], bytes[2], bytes[3]);
#elif defined(ACL_MS_WINDOWS)
	snprintf (dst, 18, "%d.%d.%d.%d", bytes[3], bytes[2], bytes[1], bytes[0]);
#else
	snprintf (dst, 18, "%d.%d.%d.%d", bytes[0], bytes[1], bytes[2], bytes[3]);
#endif

	return(dst);
}
int acl_iplink_list(const ACL_IPLink *plink)
{
	const  char	where[] = "acl_iplink_list()";
	int   i, n;
	ACL_IPItem *item;
	char  buf[64];

	if(plink == NULL || plink->parray == NULL) {
		printf("Error, %s, input error\n", where);
		return(-1);
	}

	n = acl_array_size(plink->parray);
	for (i = 0; i < n; i++) {
		item = (ACL_IPItem *) acl_array_index(plink->parray, i);
		if (item == NULL)
			break;
		__sane_inet_ntoa(item->ip_begin, buf, sizeof(buf));
		printf("ipbegin=%s", buf);
		__sane_inet_ntoa(item->ip_end, buf, sizeof(buf));
		printf(", ipend=%s\n", buf);
	}
	return (0);
}
