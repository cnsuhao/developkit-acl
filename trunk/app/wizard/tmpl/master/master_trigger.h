#pragma once
#include "acl_cpp/master/master_trigger.hpp"
#include "acl_cpp/master/master_conf.hpp"

////////////////////////////////////////////////////////////////////////////////
// 配置内容项

extern char *var_cfg_str;
extern acl::master_str_tbl var_conf_str_tab[];

extern int  var_cfg_bool;
extern acl::master_bool_tbl var_conf_bool_tab[];

extern int  var_cfg_int;
extern acl::master_int_tbl var_conf_int_tab[];

extern long long int  var_cfg_int64;
extern acl::master_int64_tbl var_conf_int64_tab[];

////////////////////////////////////////////////////////////////////////////////

class master_service : public acl::master_trigger
{
public:
	master_service();
	~master_service();

protected:
	/**
	 * 当触发器时间到时调用此函数
	 */
	virtual void on_trigger();
};
