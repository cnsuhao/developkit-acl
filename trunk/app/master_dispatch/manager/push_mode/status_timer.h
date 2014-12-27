#pragma once

class status_timer : public acl::event_timer
{
public:
	status_timer();

	/**
	 * 销毁动态对象
	 */
	virtual void destroy();

protected:
	// 基类虚函数
	virtual void timer_callback(unsigned int id);

private:
	~status_timer();
};
