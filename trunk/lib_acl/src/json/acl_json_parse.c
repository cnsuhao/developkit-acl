#include "StdAfx.h"
#include <stdio.h>
#ifndef ACL_PREPARE_COMPILE
#include "json/acl_json.h"
#endif

#define	LEN	ACL_VSTRING_LEN
#define	STR	acl_vstring_str
#define END	acl_vstring_end
#define ADDCH	ACL_VSTRING_ADDCH

#define IS_QUOTE(x) ((x) == '\"' || (x) == '\'')
#define IS_SPACE(c) ((c) == ' ' || (c) == '\t' || (c) == '\r' || (c) == '\n')
#define SKIP_WHILE(cond, ptr) { while(*(ptr) && (cond)) (ptr)++; }
#define SKIP_SPACE(ptr) { while(IS_SPACE(*(ptr))) (ptr)++; }


/* ����������ֵ�������ҵ� '{' �� '[' */

static const char *json_obj(ACL_JSON *json, const char *data)
{
	ACL_JSON_NODE *node;

	SKIP_SPACE(data);

	/* �����ҵ��ӽ��Ŀ�ʼ�ַ� */
	SKIP_WHILE(*data != '{' && *data != '[', data);

	if (*data == 0)
		return NULL;

	/* �����ӽ�㣬�����ӽ����Ϊ��ǰ��� */

	node = acl_json_node_alloc(json);
	acl_json_node_add_child(json->curr_node, node);
	node->depth = json->curr_node->depth + 1;
	if (node->depth > json->depth)
		json->depth = node->depth;

	/* ���� json ������ǰ׺�Ĳ�ͬ����¼��ͬ�Ķ����׺ */
	node->left_ch = *data;
	if (*data == '{') {
		node->right_ch = '}';
		node->type = ACL_JSON_T_OBJ;
	} else {
		node->right_ch = ']';
		node->type = ACL_JSON_T_ARRAY;
	}

	/* ȥ��ǰ���� '{' �� '[' */
	data++;

	json->curr_node->tag_node = node;

	/* �����µĽ��Ϊ��ǰ������ */
	json->curr_node = node;
	json->status = ACL_JSON_S_MEM;

	return data;
}

static const char *json_member(ACL_JSON *json, const char *data)
{
	ACL_JSON_NODE *node = acl_json_node_alloc(json);

	acl_json_node_add_child(json->curr_node, node);
	node->depth = json->curr_node->depth + 1;
	if (node->depth > json->depth)
		json->depth = node->depth;
	node->type = ACL_JSON_T_MEMBER;
	json->curr_node = node;
	json->status = ACL_JSON_S_PAR;

	return data;
}

/* �������ı�ǩ���ƣ��������û�б�ǩ����Ҷ���û�� { } [ ] �ָ��� */

static const char *json_pair(ACL_JSON *json, const char *data)
{
	ACL_JSON_NODE *parent = acl_json_node_parent(json->curr_node);

	acl_assert(parent);
	SKIP_SPACE(data);
	if (*data == 0)
		return NULL;

	/* �����ǰ�ַ�Ϊ�������ҷָ��������ʾ�������� */
	if (*data == parent->right_ch) {
		data++;  /* ȥ���������ҷָ��� */

		if (parent == json->root) {
			/* ��������������������� json ������� */
			json->finish = 1;
			return NULL;
		}
		/* ��������� */
		json->curr_node = parent;
		/* ��ѯ��������һ���ֵܽ�� */
		json->status = ACL_JSON_S_NXT;
		return data;
	}

	if (*data == '{' || *data == '[') {
		/* Ϊ '{' �� '[' ʱ˵�������˵�ǰ�����ӽ�� */

		json->status = ACL_JSON_S_OBJ;
		return data;
	}

	/* �����ǩ��ǰ�����ţ���¼�¸����� */
	if (IS_QUOTE(*data) && json->curr_node->quote == 0)
		json->curr_node->quote = *data++;

	json->curr_node->type = ACL_JSON_T_PAIR;
	json->status = ACL_JSON_S_TAG;

	return data;
}

/* ���Է�����������һ���ֵܽ�㣬�������ҵ��ָ��� ',' �� ';' */

static const char *json_next(ACL_JSON *json, const char *data)
{
	ACL_JSON_NODE *node;

	if (json->curr_node == json->root) {
		json->finish = 1;
		return NULL;
	}

	SKIP_SPACE(data);
	if (*data == 0)
		return NULL;

	/* �����������Ľ��������� json ����������� */
	node = acl_json_node_parent(json->curr_node);
	if (*data == node->right_ch) {
		data++;
		if (node == json->root) {
			json->finish = 1;
			return NULL;
		}

		/* ����үү��� */
		node = acl_json_node_parent(node);
		json->curr_node = node;
		/* ��ѯ��������һ���ֵܽ�� */
		json->status = ACL_JSON_S_NXT;
		return data;
	}

	/* ��������ҵ��ָ���: ',' �� ';' ����������һ���ֵܽ��Ľ������� */

	SKIP_WHILE(*data != ',' && *data != ';', data);
	if (*data == 0)
		return NULL;

	data++;

	/* �����½�㣬��������Ϊ��ǰ�����ֵܽ�� */

	node = acl_json_node_alloc(json);
	acl_json_node_append(json->curr_node, node);
	node->depth = json->curr_node->depth;
	json->curr_node = node;

	/* ��һ����Ҫ������ǩ�� */
	json->status = ACL_JSON_S_PAR;
	return data;
}

/* �������ı�ǩ���ƣ��������û�б�ǩ����Ҷ���û�� { } [ ] �ָ��� */

static const char *json_tag(ACL_JSON *json, const char *data)
{
	ACL_JSON_NODE *node = json->curr_node;
	int   ch;

	while ((ch = *data) != 0) {
		/* ���ǰ�������ţ�����Ҫ�ҵ���β���� */
		if (node->quote) {
			if (node->backslash) {
				if (ch == 'b')
					ADDCH(node->ltag, '\b');
				else if (ch == 'f')
					ADDCH(node->ltag, '\f');
				else if (ch == 'n')
					ADDCH(node->ltag, '\n');
				else if (ch == 'r')
					ADDCH(node->ltag, '\r');
				else if (ch == 't')
					ADDCH(node->ltag, '\t');
				else
					ADDCH(node->ltag, ch);
				node->backslash = 0;
			}

			/* ��Ϊ˫�ֽں���ʱ����һ���ֽ�Ϊ�ĸ�λΪ 1��
			 * �ڶ����ֽ�Ϊ 92��������ת���ַ���ͬ
			 */
			else if (ch == '\\') {
				/* ���������ֵ����� */
				if (node->part_word) {
					ADDCH(node->ltag, ch);
					node->part_word = 0;
				} else
					node->backslash = 1;
			} else if (ch == node->quote) {
				ACL_JSON_NODE *parent;

				parent = acl_json_node_parent(node);

				/* ���������ӽ������Ϊ�������ַ�������� */
				if (parent->left_ch == '[')
					json->status = ACL_JSON_S_NXT;

				/* ��ǩֵ������������һ����Ҫ�ҵ�ð�� */
				else
					json->status = ACL_JSON_S_COL;
				node->quote = 0;
				node->part_word = 0;
				data++;
				break;
			}

			/* �Ƿ���ݺ�������Ϊת��� '\' ����� */
			else if ((json->flag & ACL_JSON_FLAG_PART_WORD)) {
				ADDCH(node->ltag, ch);

				/* ���������ֵ����� */
				if (node->part_word)
					node->part_word = 0;
				else if (ch < 0)
					node->part_word = 1;
			} else {
				ADDCH(node->ltag, ch);
			}
		}

		/* ������ǩ��ǰû�����ŵ���� */

		else if (node->backslash) {
			ADDCH(node->ltag, ch);
			node->backslash = 0;
		}

		/* ��Ϊ˫�ֽں���ʱ����һ���ֽ�Ϊ�ĸ�λΪ 1��
		 * �ڶ����ֽ�Ϊ 92��������ת���ַ���ͬ
		 */
		else if (ch == '\\') {
			/* ���������ֵ����� */
			if (node->part_word) {
				ADDCH(node->ltag, ch);
				node->part_word = 0;
			} else
				node->backslash = 1;
		} else if (IS_SPACE(ch) || ch == ':') {
			/* ��ǩ��������������һ����Ҫ�ҵ�ð�� */
			json->status = ACL_JSON_S_COL;
			node->part_word = 0;
			break;
		}

		/* �Ƿ���ݺ�������Ϊת��� '\' ����� */
		else if ((json->flag & ACL_JSON_FLAG_PART_WORD)) {
			ADDCH(node->ltag, ch);

			/* ���������ֵ����� */
			if (node->part_word)
				node->part_word = 0;
			else if (ch < 0)
				node->part_word = 1;
		} else {
			ADDCH(node->ltag, ch);
		}
		data++;
	}

	/* �����ǩ���ǿգ�����Ҫ��֤�� 0 ��β */
	if (LEN(node->ltag) > 0)
		ACL_VSTRING_TERMINATE(node->ltag);

	return data;
}

/* һֱ�鵽ð��Ϊֹ��Ȼ���л���������ǩֵ���� */

static const char *json_colon(ACL_JSON *json, const char *data)
{
	SKIP_SPACE(data);
	if (*data == 0)
		return NULL;

	if (*data != ':') {
		data++;
		return data;
	}

	data++;

	/* ��һ��������ǩ������Ӧ�ı�ǩֵ���п���Ϊ�ַ�����
	 * Ҳ�п���Ϊ�ӽ�����
	 */
	json->status = ACL_JSON_S_VAL;

	return data;
}

/* ������ǩֵ����ֵ�п����Ǵ��ı�(���ý��ΪҶ���)��Ҳ�п�����һ���ӽ�� */

static const char *json_val(ACL_JSON *json, const char *data)
{
	ACL_JSON_NODE *node = json->curr_node;
	int   ch;

	/* ���ı�����Ϊ 0 ʱ��������Ϊ��δ������Ч���ַ� */

	if (LEN(node->text) == 0 && node->quote == 0) {
		/* �ȹ��˿�ͷû�õĿո� */
		SKIP_SPACE(data);
		SKIP_WHILE(*data == ':', data);
		if (*data == 0)
			return NULL;

		if (*data == '{' || *data == '[') {
			/* Ϊ '{' �� '[' ʱ˵�������˵�ǰ�����ӽ�� */

			json->status = ACL_JSON_S_OBJ;
			return data;
		}

		/* ����һ����Щ���ݸ�ʽΪ "xxx: ," �ķ�ʽ */
		if (*data == ',' || *data == ';') {
			/* �л�����ѯ�ý����ֵܽ��Ĺ��� */
			json->status = ACL_JSON_S_NXT;
			return data;
		}

		/* ˵����ǩ������ı�ǩֵΪ�ַ��������� */

		/* �����ǩֵǰ�����ţ���¼�¸����� */
		if (IS_QUOTE(*data) && node->quote == 0)
			node->quote = *data++;
	}

	/* ˵���������Ҷ��� */

	while ((ch = *data) != 0) {
		/* �����ʼ�����ţ�����Ҫ�Ը�������Ϊ��β�� */
		if (node->quote) {
			if (node->backslash) {
				if (ch == 'b')
					ADDCH(node->text, '\b');
				else if (ch == 'f')
					ADDCH(node->text, '\f');
				else if (ch == 'n')
					ADDCH(node->text, '\n');
				else if (ch == 'r')
					ADDCH(node->text, '\r');
				else if (ch == 't')
					ADDCH(node->text, '\t');
				else
					ADDCH(node->text, ch);
				node->backslash = 0;
			}

			/* ��Ϊ˫�ֽں���ʱ����һ���ֽ�Ϊ�ĸ�λΪ 1��
			 * �ڶ����ֽ��п���Ϊ 92��������ת���ַ���ͬ
			 */
			else if (ch == '\\') {
				/* ���������ֵ���������ǰһ���ֽ���ǰ
				 * ������֣���ǰ��ת���������������
				 */
				if (node->part_word) {
					ADDCH(node->text, ch);
					node->part_word = 0;
				} else
					node->backslash = 1;
			} else if (ch == node->quote) {
				node->quote = 0;

				/* �л�����ѯ�ý����ֵܽ��Ĺ��� */
				json->status = ACL_JSON_S_NXT;
				node->part_word = 0;
				data++;
				break;
			}

			/* �Ƿ���ݺ�������Ϊת��� '\' ����� */
			else if ((json->flag & ACL_JSON_FLAG_PART_WORD)) {
				ADDCH(node->text, ch);

				/* ��ǰһ���ֽ�Ϊǰ������֣���ǰ�ֽ�
				 * Ϊ�������֣�����Ϊһ�������ĺ���
				 */
				if (node->part_word)
					node->part_word = 0;

				/* ǰһ���ֽڷ�ǰ��������ҵ�ǰ�ֽڸ�λ
				 * Ϊ 1���������ǰ�ֽ�Ϊǰ�������
				 */
				else if (ch < 0)
					node->part_word = 1;
			} else {
				ADDCH(node->text, ch);
			}
		} else if (node->backslash) {
			ADDCH(node->text, ch);
			node->backslash = 0;
		} else if (ch == '\\') {
			if (node->part_word) {
				ADDCH(node->text, ch);
				node->part_word = 0;
			} else
				node->backslash = 1;
		} else if (IS_SPACE(ch) || ch == ',' || ch == ';'
			|| ch == '}' || ch == ']')
		{
			/* �л�����ѯ�ý����ֵܽ��Ĺ��� */
			json->status = ACL_JSON_S_NXT;
			break;
		}

		/* �Ƿ���ݺ�������Ϊת��� '\' ����� */
		else if ((json->flag & ACL_JSON_FLAG_PART_WORD)) {
			ADDCH(node->text, ch);

			/* ���������ֵ����� */
			if (node->part_word)
				node->part_word = 0;
			else if (ch < 0)
				node->part_word = 1;
		} else {
			ADDCH(node->text, ch);
		}
		data++;
	}

	if (LEN(node->text) > 0)
		ACL_VSTRING_TERMINATE(node->text);

	return data;
}

/* ״̬�����ݽṹ���� */

struct JSON_STATUS_MACHINE {
	/* ״̬�� */
	int   status;

	/* ״̬�������� */
	const char *(*callback) (ACL_JSON*, const char*);
};

static struct JSON_STATUS_MACHINE status_tab[] = {
	{ ACL_JSON_S_OBJ,  json_obj },                   /* begin json node */
	{ ACL_JSON_S_MEM,  json_member },                /* begin json node */
	{ ACL_JSON_S_PAR,  json_pair },                  /* for tag name */
	{ ACL_JSON_S_NXT,  json_next },                  /* parse brother node */
	{ ACL_JSON_S_TAG,  json_tag },                   /* for tag name */
	{ ACL_JSON_S_VAL,  json_val },                   /* for tag value */
	{ ACL_JSON_S_COL,  json_colon },                 /* for ':' */
};

void acl_json_update(ACL_JSON *json, const char *data)
{
	const char *ptr = data;

	/* ����Ƿ��Ѿ�������� */
	if (json->finish)
		return;

	/* json ������״̬��ѭ��������� */

	while (ptr && *ptr) {
		ptr = status_tab[json->status].callback(json, ptr);
	}
}
