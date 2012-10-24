#pragma once
#include "rpc.hpp"
#include "aio_socket_stream.hpp"
#include "socket_stream.hpp"
//class acl::aio_socket_stream;
//class acl::socket_stream;

class http_rpc : public acl::rpc_request
{
public:
	http_rpc(acl::aio_socket_stream* client);
	~http_rpc() {}

protected:
	// ���̴߳�����
	virtual void rpc_run();

	// ���̴߳�����̣��յ����߳�������ɵ���Ϣ
	virtual void rpc_onover();

private:
	bool proc_quit_;  // �����Ƿ�Ӧ���˳�
	acl::aio_handle& handle_;  // �첽�������
	acl::aio_socket_stream* client_;  // �ͻ���������
	bool keep_alive_; // �Ƿ���ͻ��˱��ֳ�����

	// �����߳�����������ʽ����ͻ�������
	void handle_conn(acl::socket_stream* stream);
};
