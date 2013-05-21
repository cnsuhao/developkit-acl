// http_rpc_server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/http/http_request.hpp"
#include "acl_cpp/acl_cpp_init.hpp"
#include "acl_cpp/stream/socket_stream.hpp"
#include "google/protobuf/io/http_protobuf.h"
#include "lib_acl.h"
#include "test.pb.h"

using namespace google::protobuf::io;

static void handle_client(ACL_VSTREAM* conn)
{
	acl::socket_stream client;
	(void) client.open(conn);

	acl::http_response res(&client);
	http_response response(&res);

	tutorial::AddressBook addr_req;
	if (response.read_request(&addr_req) == false)
		return;

	tutorial::AddressBook addr_res;

	for (int i = 0; i < addr_req.person_size(); i++)
	{
		const tutorial::Person& person = addr_req.person(i);
		printf("person->name: %s\r\n", person.name().c_str());
		printf("person->id: %d\r\n", person.id());
		printf("person->email: %s\r\n", person.has_email() ?
			person.email().c_str() : "null");

		tutorial::Person* person2 = addr_res.add_person();
		person2->set_name(person.name().c_str());
		person2->set_email(person.has_email() ?
			person.email().c_str() : "null");
		person2->set_id(i);

		// 列出该用户的所有电话
		for (int j = 0; j < person.phone_size(); j++)
		{
			const tutorial::Person::PhoneNumber& phone = person.phone(j);
			printf("\tphone number: %s\r\n", phone.number().c_str());

			tutorial::Person::PhoneNumber* phone2 = person2->add_phone();
			phone2->set_number(phone.number().c_str());
		}
		printf("------------------------------------------\r\n");
	}

	response.send_response(addr_res);
}

int main(int argc, char* argv[])
{
	const char* addr = "127.0.0.1:8088";
#ifdef WIN32
	acl::acl_cpp_init();
#endif
	ACL_VSTREAM* listener = acl_vstream_listen(addr, 128);
	if (listener == NULL)
	{
		printf("can't listen on %s\r\n", addr);
		return 1;
	}

	while(true)
	{
		ACL_VSTREAM* conn = acl_vstream_accept(listener, NULL, 0);
		if (conn == NULL)
			break;
		handle_client(conn);
	}
	return 0;
}
