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


/* 分析结点对象值，必须找到 '{' 或 '[' */

static const char *json_obj(ACL_JSON *json, const char *data)
{
	ACL_JSON_NODE *node;

	SKIP_SPACE(data);

	/* 必须找到子结点的开始字符 */
	SKIP_WHILE(*data != '{' && *data != '[', data);

	if (*data == 0)
		return NULL;

	/* 创建子结点，并将子结点设为当前结点 */

	node = acl_json_node_alloc(json);
	acl_json_node_add_child(json->curr_node, node);
	node->depth = json->curr_node->depth + 1;
	if (node->depth > json->depth)
		json->depth = node->depth;

	/* 根据 json 结点对象前缀的不同，记录不同的对象后缀 */
	node->left_ch = *data;
	if (*data == '{') {
		node->right_ch = '}';
		node->type = ACL_JSON_T_OBJ;
	} else {
		node->right_ch = ']';
		node->type = ACL_JSON_T_ARRAY;
	}

	/* 去掉前导符 '{' 或 '[' */
	data++;

	json->curr_node->tag_node = node;

	/* 设置新的结点为当前处理结点 */
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

/* 解析结点的标签名称，结点允许没有标签名；叶结点没有 { } [ ] 分隔符 */

static const char *json_pair(ACL_JSON *json, const char *data)
{
	ACL_JSON_NODE *parent = acl_json_node_parent(json->curr_node);

	acl_assert(parent);
	SKIP_SPACE(data);
	if (*data == 0)
		return NULL;

	/* 如果当前字符为父结点的右分隔符，则表示父结点结束 */
	if (*data == parent->right_ch) {
		data++;  /* 去掉父结点的右分隔符 */

		if (parent == json->root) {
			/* 如果根结点分析结束则整个 json 分析完毕 */
			json->finish = 1;
			return NULL;
		}
		/* 弹出父结点 */
		json->curr_node = parent;
		/* 查询父结点的下一个兄弟结点 */
		json->status = ACL_JSON_S_NXT;
		return data;
	}

	if (*data == '{' || *data == '[') {
		/* 为 '{' 或 '[' 时说明遇到了当前结点的子结点 */

		json->status = ACL_JSON_S_OBJ;
		return data;
	}

	/* 如果标签名前有引号，记录下该引号 */
	if (IS_QUOTE(*data) && json->curr_node->quote == 0)
		json->curr_node->quote = *data++;

	json->curr_node->type = ACL_JSON_T_PAIR;
	json->status = ACL_JSON_S_TAG;

	return data;
}

/* 尝试分析本结点的下一个兄弟结点，必须能找到分隔符 ',' 或 ';' */

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

	/* 如果到达根结点的结束符，则 json 解析过程完毕 */
	node = acl_json_node_parent(json->curr_node);
	if (*data == node->right_ch) {
		data++;
		if (node == json->root) {
			json->finish = 1;
			return NULL;
		}

		/* 弹出爷爷结点 */
		node = acl_json_node_parent(node);
		json->curr_node = node;
		/* 查询父结点的下一个兄弟结点 */
		json->status = ACL_JSON_S_NXT;
		return data;
	}

	/* 必须得先找到分隔符: ',' 或 ';' 才能启动下一个兄弟结点的解析过程 */

	SKIP_WHILE(*data != ',' && *data != ';', data);
	if (*data == 0)
		return NULL;

	data++;

	/* 创建新结点，并将其作为当前结点的兄弟结点 */

	node = acl_json_node_alloc(json);
	acl_json_node_append(json->curr_node, node);
	node->depth = json->curr_node->depth;
	json->curr_node = node;

	/* 下一步需要分析标签名 */
	json->status = ACL_JSON_S_PAR;
	return data;
}

/* 解析结点的标签名称，结点允许没有标签名；叶结点没有 { } [ ] 分隔符 */

static const char *json_tag(ACL_JSON *json, const char *data)
{
	ACL_JSON_NODE *node = json->curr_node;
	int   ch;

	while ((ch = *data) != 0) {
		/* 如果前面有引号，则需要找到结尾引号 */
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

			/* 当为双字节汉字时，第一个字节为的高位为 1，
			 * 第二个字节为 92，正好与转义字符相同
			 */
			else if (ch == '\\') {
				/* 处理半个汉字的情形 */
				if (node->part_word) {
					ADDCH(node->ltag, ch);
					node->part_word = 0;
				} else
					node->backslash = 1;
			} else if (ch == node->quote) {
				ACL_JSON_NODE *parent;

				parent = acl_json_node_parent(node);
				if (parent->left_ch == '[')
					json->status = ACL_JSON_S_NXT;
				/* 标签值分析结束，下一步需要找到冒号 */
				else
					json->status = ACL_JSON_S_COL;
				node->quote = 0;
				node->part_word = 0;
				data++;
				break;
			} else {
				ADDCH(node->ltag, ch);

				/* 是否兼容后半个汉字为转义符 '\' 的情况 */
				if ((json->flag & ACL_JSON_FLAG_PART_WORD)) {
					/* 处理半个汉字的情形 */
					if (node->part_word)
						node->part_word = 0;
					else if (ch < 0)
						node->part_word = 1;
				}
			}
		}

		/* 分析标签名前没有引号的情况 */

		else if (node->backslash) {
			ADDCH(node->ltag, ch);
			node->backslash = 0;
		}

		/* 当为双字节汉字时，第一个字节为的高位为 1，
		 * 第二个字节为 92，正好与转义字符相同
		 */
		else if (ch == '\\') {
			/* 处理半个汉字的情形 */
			if (node->part_word) {
				ADDCH(node->ltag, ch);
				node->part_word = 0;
			} else
				node->backslash = 1;
		} else if (IS_SPACE(ch) || ch == ':') {
			/* 标签名分析结束，下一步需要找到冒号 */
			json->status = ACL_JSON_S_COL;
			node->part_word = 0;
			break;
		} else {
			ADDCH(node->ltag, ch);

			/* 是否兼容后半个汉字为转义符 '\' 的情况 */
			if ((json->flag & ACL_JSON_FLAG_PART_WORD)) {
				/* 处理半个汉字的情形 */
				if (node->part_word)
					node->part_word = 0;
				else if (ch < 0)
					node->part_word = 1;
			}
		}
		data++;
	}

	/* 如果标签名非空，则需要保证以 0 结尾 */
	if (LEN(node->ltag) > 0)
		ACL_VSTRING_TERMINATE(node->ltag);

	return data;
}

/* 一直查到冒号为止，然后切换至分析标签值过程 */

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

	/* 下一步分析标签名所对应的标签值，有可能为字符串，
	 * 也有可能为子结点对象
	 */
	json->status = ACL_JSON_S_VAL;

	return data;
}

/* 分析标签值，该值有可能是纯文本(即该结点为叶结点)，也有可能是一个子结点 */

static const char *json_val(ACL_JSON *json, const char *data)
{
	ACL_JSON_NODE *node = json->curr_node;
	int   ch;

	/* 当文本长度为 0 时，可以认为还未遇到有效的字符 */

	if (LEN(node->text) == 0 && node->quote == 0) {
		/* 先过滤开头没用的空格 */
		SKIP_SPACE(data);
		SKIP_WHILE(*data == ':', data);
		if (*data == 0)
			return NULL;

		if (*data == '{' || *data == '[') {
			/* 为 '{' 或 '[' 时说明遇到了当前结点的子结点 */

			json->status = ACL_JSON_S_OBJ;
			return data;
		}

		/* 兼容一下有些数据格式为 "xxx: ," 的方式 */
		if (*data == ',' || *data == ';') {
			/* 切换至查询该结点的兄弟结点的过程 */
			json->status = ACL_JSON_S_NXT;
			return data;
		}

		/* 说明标签名后面的标签值为字符串或数字 */

		/* 如果标签值前有引号，记录下该引号 */
		if (IS_QUOTE(*data) && node->quote == 0)
			node->quote = *data++;
	}

	/* 说明本结点是叶结点 */

	while ((ch = *data) != 0) {
		/* 如果开始有引号，则需要以该引号作为结尾符 */
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

			/* 当为双字节汉字时，第一个字节为的高位为 1，
			 * 第二个字节有可能为 92，正好与转义字符相同
			 */
			else if (ch == '\\') {
				/* 处理半个汉字的情况，如果前一个字节是前半个汉字，
				 * 则当前的转义符当作后半个汉字
				 */
				if (node->part_word) {
					ADDCH(node->text, ch);
					node->part_word = 0;
				} else
					node->backslash = 1;
			} else if (ch == node->quote) {
				node->quote = 0;

				/* 切换至查询该结点的兄弟结点的过程 */
				json->status = ACL_JSON_S_NXT;
				node->part_word = 0;
				data++;
				break;
			} else {
				ADDCH(node->text, ch);

				/* 是否兼容后半个汉字为转义符 '\' 的情况 */
				if ((json->flag & ACL_JSON_FLAG_PART_WORD)) {
					/* 若前一个字节为前半个汉字，则当前字节
					 * 为后半个汉字，正好为一个完整的汉字
					 */
					if (node->part_word)
						node->part_word = 0;

					/* 前一个字节非前半个汉字且当前字节高位为 1，
					 * 则表明当前字节为前半个汉字
					 */
					else if (ch < 0)
						node->part_word = 1;
				}
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
			/* 切换至查询该结点的兄弟结点的过程 */
			json->status = ACL_JSON_S_NXT;
			break;
		} else {
			ADDCH(node->text, ch);

			/* 是否兼容后半个汉字为转义符 '\' 的情况 */
			if ((json->flag & ACL_JSON_FLAG_PART_WORD)) {
				/* 处理半个汉字的情形 */
				if (node->part_word)
					node->part_word = 0;
				else if (ch < 0)
					node->part_word = 1;
			}
		}
		data++;
	}

	if (LEN(node->text) > 0)
		ACL_VSTRING_TERMINATE(node->text);

	return data;
}

/* 状态机数据结构类型 */

struct JSON_STATUS_MACHINE {
	/* 状态码 */
	int   status;

	/* 状态机处理函数 */
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

	/* 检查是否已经解析完毕 */
	if (json->finish)
		return;

	/* json 解析器状态机循环处理过程 */

	while (ptr && *ptr) {
		ptr = status_tab[json->status].callback(json, ptr);
	}
}
