#pragma once

class StatusConnection;
class HttpStatusRpc;

/**
 * ��������� IO ����Ļص�������
 */
class StatusIOCallback : public acl::aio_callback
{
public:
	StatusIOCallback(StatusConnection* conn);

protected:
	// ���� aio_callback �麯��

	bool read_wakeup();
	void close_callback();
	bool timeout_callback();

private:
	StatusConnection* conn_;
	HttpStatusRpc* rpc_;

	~StatusIOCallback();
};
