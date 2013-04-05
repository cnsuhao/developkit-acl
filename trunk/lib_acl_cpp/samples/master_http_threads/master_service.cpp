#include "stdafx.h"
#include "master_service.h"

////////////////////////////////////////////////////////////////////////////////
// ����������

char *var_cfg_str;
acl::master_str_tbl var_conf_str_tab[] = {
	{ "str", "test_msg", &var_cfg_str },

	{ 0, 0, 0 }
};

int  var_cfg_bool;
acl::master_bool_tbl var_conf_bool_tab[] = {
	{ "bool", 1, &var_cfg_bool },

	{ 0, 0, 0 }
};

int  var_cfg_buf_size;
acl::master_int_tbl var_conf_int_tab[] = {
	{ "buf_size", 1024, &var_cfg_buf_size, 0, 0 },

	{ 0, 0 , 0 , 0, 0 }
};

long long int  var_cfg_int64;
acl::master_int64_tbl var_conf_int64_tab[] = {
	{ "int64", 120, &var_cfg_int64, 0, 0 },

	{ 0, 0 , 0 , 0, 0 }
};

////////////////////////////////////////////////////////////////////////////////

master_service::master_service()
: res_buf_(NULL)
{
}

master_service::~master_service()
{
	if (res_buf_)
		acl_myfree(res_buf_);
}

bool master_service::thread_on_read(acl::socket_stream* stream)
{
	acl::http_response res(stream);
	// ��Ӧ������Ϊ xml ��ʽ
	res.response_header().set_content_type("text/html");

	// �� HTTP ����ͷ
	if (res.read_header() == false)
		return false;

	acl::string buf;
	// �� HTTP ����������
	if (res.get_body(buf) == false)
		return false;

	acl::http_client* client = res.get_client();

	// �жϿͻ����Ƿ�ϣ�����ֳ�����
	bool keep_alive = client->keep_alive();

	// �������ݸ��ͻ���

	if (res.response(res_buf_, var_cfg_buf_size, 200, keep_alive) == false)
		return false;

	return keep_alive ? true : false;
}

bool master_service::thread_on_accept(acl::socket_stream*)
{
	return true;
}

bool master_service::thread_on_timeout(acl::socket_stream*)
{
	return false;
}

void master_service::thread_on_close(acl::socket_stream*)
{
}

void master_service::thread_on_init()
{
}

void master_service::thread_on_exit()
{
}

void master_service::proc_on_init()
{
	if (var_cfg_buf_size <= 0)
		var_cfg_buf_size = 1024;
	res_buf_ = (char*) acl_mymalloc(var_cfg_buf_size + 1);
	int i;
	for (i = 0; i < var_cfg_buf_size; i++)
		res_buf_[i] = 'X';
	res_buf_[i] = 0;
}

void master_service::proc_on_exit()
{
}
