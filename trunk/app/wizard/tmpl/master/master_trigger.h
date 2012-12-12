#pragma once
#include "master_trigger.hpp"

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
