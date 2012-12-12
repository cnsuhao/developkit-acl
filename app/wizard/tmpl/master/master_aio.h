#pragma once
#include "master_aio.hpp"

class acl::aio_socket_stream;

class master_service : public acl::master_aio
{
public:
	master_service();
	~master_service();

protected:
	/**
	 * ���麯���������յ�һ���ͻ�������ʱ���ô˺���
	 * @param stream {aio_socket_stream*} �½��յ��Ŀͻ����첽������
	 * @return {bool} �ú���������� false ��֪ͨ��������ܲ��ٽ���
	 *  Զ�̿ͻ������ӣ�����������տͻ�������
	 */
	bool on_accept(acl::aio_socket_stream* stream);
};
