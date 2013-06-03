#include "StdAfx.h"
#include <stdio.h>
#ifndef ACL_PREPARE_COMPILE
#include "json/acl_json.h"
#endif

#define	LEN	ACL_VSTRING_LEN
#define	STR	acl_vstring_str
#define END	acl_vstring_end
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
		return (NULL);

	/* ��Ҫ�ѵ�ǰ����״̬�� ACL_JSON_S_NXT �Ա��ӽ��
	 * ������Ϸ��ظý��ʱ���Լ������Ҹý����ֵܽ��
	 */
	if (json->curr_node != json->root)
		json->curr_node->status = ACL_JSON_S_NXT;

	/* ���� json ������ǰ׺�Ĳ�ͬ����¼��ͬ�Ķ����׺ */

	json->curr_node->left_ch = *data;
	if (*data == '{') {
		json->curr_node->right_ch = '}';
		json->curr_node->type = ACL_JSON_T_NODE;
	} else {
		json->curr_node->right_ch = ']';
		json->curr_node->type = ACL_JSON_T_ARRAY;
	}

	/* ȥ��ǰ���� '{' �� '[' */
	data++;

	/* �����ӽ�㣬�����ӽ����Ϊ��ǰ��� */

	node = acl_json_node_alloc(json);
	acl_json_node_add_child(json->curr_node, node);
	node->depth = json->curr_node->depth + 1;
	if (node->depth > json->depth)
		json->depth = node->depth;
	node->status = ACL_JSON_S_TAG;

	/* ��ǰ���ı�ǩ�ǿ�ʱ��ʾ�½��Ϊ��ǰ����ǩ��json ��� */
	if (LEN(json->curr_node->ltag) > 0)
		json->curr_node->tag_node = node;

	/* �����µĽ��Ϊ��ǰ������ */
	json->curr_node = node;

	return (data);
}

/* �Ӹ���㿪ʼ���� */

static const char *json_root(ACL_JSON *json, const char *data)
{
	acl_assert(json->root == json->curr_node);

	/* �����û�б�ǩ����ֻ���ӽ�� */
	return (json_obj(json, data));
}

/* ���Է�����������һ���ֵܽ�㣬�������ҵ��ָ��� ',' �� ';' */

static const char *json_next(ACL_JSON *json, const char *data)
{
	ACL_JSON_NODE *node;

	/* ����㲻�������ֵܽ�� */
	acl_assert(json->curr_node != json->root);

	SKIP_SPACE(data);
	if (*data == 0)
		return (NULL);

	/* �����������Ľ��������� json ����������� */
	node = acl_json_node_parent(json->curr_node);
	if (*data == node->right_ch) {
		data++;
		if (node == json->root) {
			json->finish = 1;
			return (NULL);
		}
		/* ��������� */
		json->curr_node = node;
		/* ��ѯ��������һ���ֵܽ�� */
		json->curr_node->status = ACL_JSON_S_NXT;
		return (data);
	}

	/* ��������ҵ��ָ���: ',' �� ';' ����������һ���ֵܽ��Ľ������� */

	SKIP_WHILE(*data != ',' && *data != ';', data);
	if (*data == 0)
		return (NULL);

	data++;

	/* �����½�㣬��������Ϊ��ǰ�����ֵܽ�� */

	node = acl_json_node_alloc(json);
	acl_json_node_append(json->curr_node, node);
	node->depth = json->curr_node->depth;
	json->curr_node = node;

	/* ��һ����Ҫ������ǩ�� */
	json->curr_node->status = ACL_JSON_S_TAG;
	return (data);
}

/* �������ı�ǩ���ƣ��������û�б�ǩ����Ҷ���û�� { } [ ] �ָ��� */

static const char *json_tag(ACL_JSON *json, const char *data)
{
	int   ch;

	/* ��ʼ������ǩʱ����Ҫ���ñ�ǩ��ǰ�Ƿ������ţ����������ո� */
	if (LEN(json->curr_node->ltag) == 0 && json->curr_node->quote == 0) {
		ACL_JSON_NODE *node = acl_json_node_parent(json->curr_node);

		acl_assert(node);
		SKIP_SPACE(data);

		/* �����ǰ�ַ�Ϊ�������ҷָ��������ʾ�������� */
		if (*data == node->right_ch) {
			data++;  /* ȥ���������ҷָ��� */

			if (node == json->root) {
				/* ��������������������� json ������� */
				json->finish = 1;
				return (NULL);
			}
			/* ��������� */
			json->curr_node = node;
			/* ��ѯ��������һ���ֵܽ�� */
			json->curr_node->status = ACL_JSON_S_NXT;
			return (data);
		}

		if (*data == '{' || *data == '[') {
			/* Ϊ '{' �� '[' ʱ˵�������˵�ǰ�����ӽ�� */

			json->curr_node->status = ACL_JSON_S_OBJ;
			return (data);
		}

		/* �����ǩ��ǰ�����ţ���¼�¸����� */
		if (IS_QUOTE(*data) && json->curr_node->quote == 0)
			json->curr_node->quote = *data++;
	}

	while ((ch = *data) != 0) {
		/* ���ǰ�������ţ�����Ҫ�ҵ���β���� */
		if (json->curr_node->quote) {
			if (json->curr_node->backslash) {
				ACL_VSTRING_ADDCH(json->curr_node->ltag, ch);
				json->curr_node->backslash = 0;
			}

			/* ��Ϊ˫�ֽں���ʱ����һ���ֽ�Ϊ�ĸ�λΪ 1��
			 * �ڶ����ֽ�Ϊ 92��������ת���ַ���ͬ
			 */
			else if (ch == '\\') {
				/* ���������ֵ����� */
				if (json->curr_node->part_word) {
					ACL_VSTRING_ADDCH(json->curr_node->ltag, ch);
					json->curr_node->part_word = 0;
				} else
					json->curr_node->backslash = 1;
			} else if (ch == json->curr_node->quote) {
				ACL_JSON_NODE *parent =
					acl_json_node_parent(json->curr_node);
				if (parent->left_ch == '[')
					json->curr_node->status = ACL_JSON_S_NXT;
				/* ��ǩֵ������������һ����Ҫ�ҵ�ð�� */
				else
					json->curr_node->status = ACL_JSON_S_COL;
				json->curr_node->quote = 0;
				json->curr_node->part_word = 0;
				data++;
				break;
			} else {
				ACL_VSTRING_ADDCH(json->curr_node->ltag, ch);

				/* �Ƿ���ݺ�������Ϊת��� '\' ����� */
				if ((json->flag & ACL_JSON_FLAG_PART_WORD)) {
					/* ���������ֵ����� */
					if (json->curr_node->part_word)
						json->curr_node->part_word = 0;
					else if (ch < 0)
						json->curr_node->part_word = 1;
				}
			}
		}

		/* ������ǩ��ǰû�����ŵ���� */

		else if (json->curr_node->backslash) {
			ACL_VSTRING_ADDCH(json->curr_node->ltag, ch);
			json->curr_node->backslash = 0;
		}

		/* ��Ϊ˫�ֽں���ʱ����һ���ֽ�Ϊ�ĸ�λΪ 1��
		 * �ڶ����ֽ�Ϊ 92��������ת���ַ���ͬ
		 */
		else if (ch == '\\') {
			/* ���������ֵ����� */
			if (json->curr_node->part_word) {
				ACL_VSTRING_ADDCH(json->curr_node->ltag, ch);
				json->curr_node->part_word = 0;
			} else
				json->curr_node->backslash = 1;
		} else if (IS_SPACE(ch) || ch == ':') {
			/* ��ǩ��������������һ����Ҫ�ҵ�ð�� */
			json->curr_node->status = ACL_JSON_S_COL;
			json->curr_node->part_word = 0;
			break;
		} else {
			ACL_VSTRING_ADDCH(json->curr_node->ltag, ch);

			/* �Ƿ���ݺ�������Ϊת��� '\' ����� */
			if ((json->flag & ACL_JSON_FLAG_PART_WORD)) {
				/* ���������ֵ����� */
				if (json->curr_node->part_word)
					json->curr_node->part_word = 0;
				else if (ch < 0)
					json->curr_node->part_word = 1;
			}
		}
		data++;
	}

	/* �����ǩ���ǿգ�����Ҫ��֤�� 0 ��β */
	if (LEN(json->curr_node->ltag) > 0)
		ACL_VSTRING_TERMINATE(json->curr_node->ltag);
	return (data);
}

/* һֱ�鵽ð��Ϊֹ��Ȼ���л���������ǩֵ���� */

static const char *json_colon(ACL_JSON *json, const char *data)
{
	SKIP_SPACE(data);
	if (*data == 0)
		return (NULL);
	else if (*data != ':') {
		data++;
		return (data);
	} else {
		data++;
		/* ��һ��������ǩ������Ӧ�ı�ǩֵ���п���Ϊ�ַ�����
		 * Ҳ�п���Ϊ�ӽ�����
		 */
		json->curr_node->status = ACL_JSON_S_VAL;
		return (data);
	}
}

/* ������ǩֵ����ֵ�п����Ǵ��ı�(���ý��ΪҶ���)��Ҳ�п�����һ���ӽ�� */

static const char *json_val(ACL_JSON *json, const char *data)
{
	int   ch;

	/* ���ı�����Ϊ 0 ʱ��������Ϊ��δ������Ч���ַ� */

	if (LEN(json->curr_node->text) == 0 && json->curr_node->quote == 0) {
		/* �ȹ��˿�ͷû�õĿո� */
		SKIP_SPACE(data);
		SKIP_WHILE(*data == ':', data);
		if (*data == 0)
			return (NULL);

		if (*data == '{' || *data == '[') {
			/* Ϊ '{' �� '[' ʱ˵�������˵�ǰ�����ӽ�� */

			json->curr_node->status = ACL_JSON_S_OBJ;
			return (data);
		}

		/* ����һ����Щ���ݸ�ʽΪ "xxx: ," �ķ�ʽ */
		if (*data == ',' || *data == ';') {
			/* �л�����ѯ�ý����ֵܽ��Ĺ��� */
			json->curr_node->status = ACL_JSON_S_NXT;
			return (data);
		}

		/* ˵����ǩ������ı�ǩֵΪ�ַ��������� */

		/* �����ǩֵǰ�����ţ���¼�¸����� */
		if (IS_QUOTE(*data) && json->curr_node->quote == 0)
			json->curr_node->quote = *data++;
	}

	/* ˵���������Ҷ��� */

	while ((ch = *data) != 0) {
		/* �����ʼ�����ţ�����Ҫ�Ը�������Ϊ��β�� */
		if (json->curr_node->quote) {
			if (json->curr_node->backslash) {
				ACL_VSTRING_ADDCH(json->curr_node->text, ch);
				json->curr_node->backslash = 0;
			}

			/* ��Ϊ˫�ֽں���ʱ����һ���ֽ�Ϊ�ĸ�λΪ 1��
			 * �ڶ����ֽ��п���Ϊ 92��������ת���ַ���ͬ
			 */
			else if (ch == '\\') {
				/* ���������ֵ���������ǰһ���ֽ���ǰ������֣�
				 * ��ǰ��ת���������������
				 */
				if (json->curr_node->part_word) {
					ACL_VSTRING_ADDCH(json->curr_node->text, ch);
					json->curr_node->part_word = 0;
				} else
					json->curr_node->backslash = 1;
			} else if (ch == json->curr_node->quote) {
				json->curr_node->quote = 0;

				/* �л�����ѯ�ý����ֵܽ��Ĺ��� */
				json->curr_node->status = ACL_JSON_S_NXT;
				json->curr_node->part_word = 0;
				data++;
				break;
			} else {
				ACL_VSTRING_ADDCH(json->curr_node->text, ch);

				/* �Ƿ���ݺ�������Ϊת��� '\' ����� */
				if ((json->flag & ACL_JSON_FLAG_PART_WORD)) {
					/* ��ǰһ���ֽ�Ϊǰ������֣���ǰ�ֽ�
					 * Ϊ�������֣�����Ϊһ�������ĺ���
					 */
					if (json->curr_node->part_word)
						json->curr_node->part_word = 0;

					/* ǰһ���ֽڷ�ǰ��������ҵ�ǰ�ֽڸ�λΪ 1��
					 * �������ǰ�ֽ�Ϊǰ�������
					 */
					else if (ch < 0)
						json->curr_node->part_word = 1;
				}
			}
		} else if (json->curr_node->backslash) {
			ACL_VSTRING_ADDCH(json->curr_node->text, ch);
			json->curr_node->backslash = 0;
		} else if (ch == '\\') {
			if (json->curr_node->part_word) {
				ACL_VSTRING_ADDCH(json->curr_node->text, ch);
				json->curr_node->part_word = 0;
			} else
				json->curr_node->backslash = 1;
		} else if (IS_SPACE(ch) || ch == ',' || ch == ';'
			|| ch == '}' || ch == ']')
		{
			/* �л�����ѯ�ý����ֵܽ��Ĺ��� */
			json->curr_node->status = ACL_JSON_S_NXT;
			break;
		} else {
			ACL_VSTRING_ADDCH(json->curr_node->text, ch);

			/* �Ƿ���ݺ�������Ϊת��� '\' ����� */
			if ((json->flag & ACL_JSON_FLAG_PART_WORD)) {
				/* ���������ֵ����� */
				if (json->curr_node->part_word)
					json->curr_node->part_word = 0;
				else if (ch < 0)
					json->curr_node->part_word = 1;
			}
		}
		data++;
	}

	if (LEN(json->curr_node->text) > 0)
		ACL_VSTRING_TERMINATE(json->curr_node->text);
	return (data);
}

/* ״̬�����ݽṹ���� */

struct JSON_STATUS_MACHINE {
	int   status;	/**< ״̬�� */
	const char *(*callback) (ACL_JSON*, const char*);	/**< ״̬�������� */
};

static struct JSON_STATUS_MACHINE status_tab[] = {
	{ ACL_JSON_S_ROOT, json_root },                 /* parse root */
	{ ACL_JSON_S_OBJ, json_obj },                   /* begin json node */
	{ ACL_JSON_S_NXT, json_next },                  /* parse brother node */
	{ ACL_JSON_S_TAG, json_tag },                   /* for tag name */
	{ ACL_JSON_S_VAL, json_val },                   /* for tag value */
	{ ACL_JSON_S_COL, json_colon },                 /* for ':' */
};

void acl_json_update(ACL_JSON *json, const char *data)
{
	const char *ptr = data;

	/* ����Ƿ��Ѿ�������� */
	if (json->finish)
		return;

	/* json ������״̬��ѭ��������� */

	while (ptr && *ptr) {
		ptr = status_tab[json->curr_node->status].callback(json, ptr);
	}
}
