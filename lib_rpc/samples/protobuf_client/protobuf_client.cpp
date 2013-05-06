// protobuf_client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "acl_cpp/stream/ifstream.hpp"
#include "acl_cpp/stream/ofstream.hpp"
#include "acl_cpp/stdlib/string.hpp"
#include "google/protobuf/io/acl_protobuf_stream.h"
#include "test.pb.h"

using namespace google::protobuf::io;

int main(int argc, char* argv[])
{
	const char* path = "test.txt";

	// 将用户的信息序列化输出至文件中

	acl::ofstream out_fp;
	if (out_fp.open_write(path) == false)
	{
		printf("open file %s error\r\n", path);
		return 1;
	}
	acl_protobuf_ostream out(&out_fp);

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
		person->set_id(1);

		// 给一个用户添加多个电话号码
		for (size_t j = 0; j < tutorial::Person::WORK; j++)
		{
			tutorial::Person::PhoneNumber* phone = person->add_phone();
			buf.format("11111111-%d", j);
			phone->set_number(buf.c_str());
		}
	}

	// 将地址簿数据序列化至磁盘文件流中
	address.SerializeToZeroCopyStream(&out);
	out_fp.close();

	/////////////////////////////////////////////////////////////////////////

	// 从序列化文件中读取用户信息

	acl::ifstream in_fp;
	if (in_fp.open_read(path) == false)
	{
		printf("open file %s error\r\n", path);
		return 1;
	}
	acl_protobuf_istream in(&in_fp);
	address.Clear();

	// 从文件流中解析地址簿信息
	if (!address.ParseFromZeroCopyStream(&in))
	{
		printf("parse file %s failed\r\n", path);
		return 1;
	}

	// 列出地址簿中所有用户信息
	for (int i = 0; i < address.person_size(); i++)
	{
		const tutorial::Person& person = address.person(i);
		printf("person->name: %s\r\n", person.name().c_str());
		printf("person->id: %d\r\n", person.id());
		printf("person->email: %s\r\n", person.has_email() ?
			person.email().c_str() : "null");

		// 列出该用户的所有电话
		for (int j = 0; j < person.phone_size(); j++)
		{
			const tutorial::Person::PhoneNumber& phone = person.phone(j);
			printf("phone number: %s\r\n", phone.number().c_str());
		}
	}


	 // Optional:  Delete all global objects allocated by libprotobuf.
	google::protobuf::ShutdownProtobufLibrary();
	return 0;
}
