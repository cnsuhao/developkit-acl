// http_rpc_client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "acl_cpp/acl_cpp_init.hpp"
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/http/http_request.hpp"
#include "google/protobuf/io/http_protobuf.h"
#include "test.pb.h"

using namespace google::protobuf::io;

int main(int argc, char* argv[])
{
#ifdef WIN32
	acl::acl_cpp_init();
#endif

	//////////////////////////////////////////////////////////////////
	// 请求过程

	tutorial::AddressBook address;
	size_t  person_count = 5;

	acl::string buf;

	// 给地址簿中添加用户列表
	for (size_t i = 0; i < person_count; i++)
	{
		tutorial::Person* person = address.add_person();
		buf.format("zsxxsz-%d", i);
		person->set_name(buf.c_str());
		buf.format("zsxxsz-%d@test.com", i);
		person->set_email(buf.c_str());
		person->set_id(i);

		// 给一个用户添加多个电话号码
		for (size_t j = 0; j < tutorial::Person::WORK; j++)
		{
			tutorial::Person::PhoneNumber* phone = person->add_phone();
			buf.format("11111111-%d-%d", i, j);
			phone->set_number(buf.c_str());
		}
	}

	//////////////////////////////////////////////////////////////////
	// 发送请求数据至服务端，并读取服务端的响应

	tutorial::AddressBook address_result;
	http_request rpc("127.0.0.1:8088");
	if (rpc.rpc_request(address, &address_result) == false)
	{
		printf("rpc_request error\r\n");
		return 0;
	}

	//////////////////////////////////////////////////////////////////
	// 分析响应数据

	// 列出地址簿中所有用户信息
	for (int i = 0; i < address_result.person_size(); i++)
	{
		const tutorial::Person& person = address_result.person(i);
		printf("person->name: %s\r\n", person.name().c_str());
		printf("person->id: %d\r\n", person.id());
		printf("person->email: %s\r\n", person.has_email() ?
			person.email().c_str() : "null");

		// 列出该用户的所有电话
		for (int j = 0; j < person.phone_size(); j++)
		{
			const tutorial::Person::PhoneNumber& phone = person.phone(j);
			printf("\tphone number: %s\r\n", phone.number().c_str());
		}
		printf("------------------------------------------\r\n");
	}

	// Optional:  Delete all global objects allocated by libprotobuf.
	google::protobuf::ShutdownProtobufLibrary();

#ifdef WIN32
	printf("Enter any key to exit\r\n");
	getchar();
#endif
	return 0;
}
