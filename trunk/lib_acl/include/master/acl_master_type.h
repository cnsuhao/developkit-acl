#ifndef	__ACL_MASTER_TYPE_INCLUDE_H__
#define	__ACL_MASTER_TYPE_INCLUDE_H__

#ifdef	__cplusplus
extern "C" {
#endif

#include "stdlib/acl_xinetd_cfg.h"

#if 0
typedef struct {
	const char *name;
	int  defval;
	int *target;
	int  min;
	int  max;
} ACL_CONFIG_INT_TABLE;

typedef struct {
	const char *name;
	const char *defval;
	char **target;
} ACL_CONFIG_STR_TABLE;

typedef struct {
	const char *name;
	int   defval;
	int  *target;
} ACL_CONFIG_BOOL_TABLE;
#endif

#define	ACL_CONFIG_INT_TABLE	ACL_CFG_INT_TABLE
#define	ACL_CONFIG_INT64_TABLE	ACL_CFG_INT64_TABLE
#define	ACL_CONFIG_STR_TABLE	ACL_CFG_STR_TABLE
#define	ACL_CONFIG_BOOL_TABLE	ACL_CFG_BOOL_TABLE

#ifdef	__cplusplus
}
#endif

#endif
