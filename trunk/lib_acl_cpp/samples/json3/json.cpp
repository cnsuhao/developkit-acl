#include "stdafx.h"
/**
 * ����json�����������Ʊ���������⣬\t \n��
 */

int main()
{
	const char* type = "set";
	const char* tablename = "\txmailuser";
	const char* name = "	chenzhen";
	
	printf(">>>>{%s}\r\n", tablename);
	acl::json json;
	acl::json_node& root = json.get_root().add_child(false, true);
	root.add_child("type", type);
	root.add_child("tablename", tablename);
	root.add_child("name", name);

	// ����json�ַ���
	acl::string buf;
	json.build_json(buf);
	printf("buf:\r\n%s\r\n%s\r\n", buf.c_str(), json.to_string().c_str());

	// �������ɵ��ַ�����ȡ��ֵ
	acl::json json2(buf.c_str());
	acl::json_node& root2 = json2.get_root();
	acl::json_node* child = root2.first_child();

	const char* tag, *txt;
	while (child != NULL)
	{
		if ((tag = child->tag_name()) != NULL && *tag != 0
			&& (txt = child->get_text()) != NULL && *txt != 0)
		{
			printf("tag: %s, txt: %s\n", tag, txt);
		}
		else
			printf("no tag name or no txt in json node");

		child = root2.next_child();
	}
	printf("\r\n");
	return 0;
}
