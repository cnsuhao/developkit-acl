#pragma once
#include "master_trigger.hpp"

class master_service : public acl::master_trigger
{
public:
	master_service();
	~master_service();

protected:
	/**
	 * ��������ʱ�䵽ʱ���ô˺���
	 */
	virtual void on_trigger();
};
