#pragma once
#include "master_proc.hpp"

class acl::socket_stream;

class master_service : public acl::master_proc
{
public:
	master_service();
	~master_service();

protected:
	/**
	 * ���麯���������յ�һ���ͻ�������ʱ���ô˺���
	 * @param stream {aio_socket_stream*} �½��յ��Ŀͻ����첽������
	 * ע���ú������غ������ӽ��ᱻ�رգ��û���Ӧ�����رո���
	 */
	virtual void on_accept(acl::socket_stream* stream);
};
