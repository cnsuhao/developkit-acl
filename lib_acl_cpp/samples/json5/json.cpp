// json.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <list>
#include <vector>
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/stdlib/json.hpp"

using namespace std;

#if 0
static const char* default_data = \
    "{ DataValue: { menuitem2: [\r\n"
    "    {'value': 'New', 'onclick': 'CreateNewDoc()'},\r\n"
    "    {'value': 'Open', 'onclick': 'OpenDoc()'},\r\n"
    "    {'value': 'Close', 'onclick': 'CloseDoc()'},\r\n"
    "    [{'value': 'Save', 'onclick': 'SaveDoc()'}]"
    "  ]}\r\n"
    "}\r\n";
#else
static const char* default_data = "{'DataValue': {'RemoteLoginRemind': 'true1', 'ModifyPasswdRemind': 'true2', 'MailForwardRemind': 'true3', 'SecureLoginVerification': 'remote'}}";
#endif

static void test(void)
{
	acl::json json(default_data);

	printf("-------------------------------------------------\r\n");
	printf("build json: %s\r\n", json.to_string().c_str());

	const char* tags;

	tags = "DataValue";
	printf("-------------------------------------------------\r\n");
	printf("source data: %s\r\n", default_data);
	printf("-------------------------------------------------\r\n");
	printf(">>>tags: %s\r\n", tags);

	const std::vector<acl::json_node*>& nodes = json.getElementsByTags(tags);
	if (nodes.empty())
	{
		printf("NOT FOUND\r\n");
		return;
	}

	acl::json_node* first = nodes[0];
	if (first == NULL)
		return;

	printf(">>first node string: %s\r\n", first->to_string().c_str());
	ACL_VSTRING *bf = acl_vstring_alloc(1);
	ACL_JSON_NODE* node = nodes[0]->get_json_node()->tag_node;
	if (node)
	{
		acl_json_node_build(node, bf);
		printf("--- the node string>>>>>%s\r\n", acl_vstring_str(bf));
		if (node->parent == nodes[0]->get_json_node())
			printf("-==============\r\n");
		else
			printf("---------------\r\n");
	}
	else
		printf("%s: null, txt: %s\r\n", first->tag_name(), first->get_text() ? first->get_text():"null");

//	printf("first json node: %s\r\n", first->to_string().c_str());
	acl_vstring_free(bf);

	/*
	/////////////////////////////////////////////////////////////////////
	acl::json_node* iter = json.first_node();
	printf("\r\n");
	while (iter)
	{
//		printf("iter: %s, %s\r\n", iter->tag_name(), iter->to_string().c_str());
		ACL_VSTRING *bf = acl_vstring_alloc(1024);
		acl_json_node_build(iter->get_json_node()->tag_node, bf);
		printf(">>>>>>>>%s\r\n", acl_vstring_str(bf));
		acl_vstring_free(bf);
		break;
		iter = json.next_node();
	}
	*/
}

int main(void)
{
	test();
	printf("enter any key to exit\r\n");
	getchar();
	return 0;
}

