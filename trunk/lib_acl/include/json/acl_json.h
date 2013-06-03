#ifndef __ACL_JSON_INCLUDE_H__
#define __ACL_JSON_INCLUDE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdlib/acl_iterator.h"
#include "stdlib/acl_vstring.h"
#include "stdlib/acl_ring.h"
#include "stdlib/acl_array.h"

typedef struct ACL_JSON ACL_JSON;
typedef struct ACL_JSON_NODE ACL_JSON_NODE;

struct ACL_JSON_NODE {
	ACL_VSTRING *ltag;          /**< ��ǩ�� */
	ACL_VSTRING *text;          /**< �����ΪҶ���ʱ���ı����ݷǿ� */
	ACL_JSON_NODE *tag_node;    /**< ����ǩֵΪ json ���ʱ����ǿ� */
	int   type;
#define ACL_JSON_T_LEAF         0   /**< �ý��ı�ǩֵΪҶ��� */
#define ACL_JSON_T_NODE         1   /**< �ý��ı�ǩֵΪ json ��� */
#define ACL_JSON_T_ARRAY        2   /**< �ý��ı�ǩֵΪ json ���� */
#define ACL_JSON_T_OBJ          3   /**< �ý��Ϊһ�� json ���� */

	ACL_JSON_NODE *parent;      /**< ����� */
	ACL_RING children;          /**< �ӽ�㼯�� */
	int  depth;                 /**< ��ǰ������� */

	/* private */
	ACL_JSON *json;             /**< json ���� */
	ACL_RING node;              /**< ��ǰ��� */
	int   quote;                /**< �� 0 ��ʾ ' �� " */
	int   left_ch;              /**< �����ĵ�һ���ַ�: { or [ */
	int   right_ch;             /**< ���������һ���ַ�: } or ] */
	int   backslash;            /**< ת���ַ� \ */
	int   part_word;            /**< ������ֵ���������־λ */

	int   status;               /**< ״̬����ǰ����״̬ */
#define ACL_JSON_S_ROOT     0       /**< ��ʾ�Ǹ���� */
#define ACL_JSON_S_OBJ      1       /**< ��ǩ����ֵ */
#define ACL_JSON_S_NXT      2       /**< ��һ����� */
#define ACL_JSON_S_TAG      3       /**< ��ǩ�� */
#define ACL_JSON_S_VAL      4       /**< �ӽ�㴦����� */
#define ACL_JSON_S_COL      5       /**< ð�� : */

	/* public: for acl_iterator, ͨ�� acl_foreach �г��ý���һ���ӽ�� */

	/* ȡ������ͷ���� */
	ACL_JSON_NODE *(*iter_head)(ACL_ITER*, ACL_JSON_NODE*);
	/* ȡ��������һ������ */
	ACL_JSON_NODE *(*iter_next)(ACL_ITER*, ACL_JSON_NODE*);
	/* ȡ������β���� */
	ACL_JSON_NODE *(*iter_tail)(ACL_ITER*, ACL_JSON_NODE*);
	/* ȡ��������һ������ */
	ACL_JSON_NODE *(*iter_prev)(ACL_ITER*, ACL_JSON_NODE*);
};

struct ACL_JSON {
	int   depth;                /**< ������ */
	int   node_cnt;             /**< �������, ���� root ��� */
	ACL_JSON_NODE *root;        /**< json ����� */
	int   finish;               /**< �Ƿ�������� */
	unsigned flag;              /**< ��־λ */
#define	ACL_JSON_FLAG_PART_WORD	(1 << 0)  /**< �Ƿ���ݰ������ */

	/* public: for acl_iterator, ͨ�� acl_foreach �����г������ӽ�� */

	/* ȡ������ͷ���� */
	ACL_JSON_NODE *(*iter_head)(ACL_ITER*, ACL_JSON*);
	/* ȡ��������һ������ */
	ACL_JSON_NODE *(*iter_next)(ACL_ITER*, ACL_JSON*);
	/* ȡ������β���� */
	ACL_JSON_NODE *(*iter_tail)(ACL_ITER*, ACL_JSON*);
	/* ȡ��������һ������ */
	ACL_JSON_NODE *(*iter_prev)(ACL_ITER*, ACL_JSON*);

	/* private */
	ACL_ARRAY *node_cache;      /**< json ��㻺��� */
	int   max_cache;            /**< json ��㻺��ص�������� */
	ACL_JSON_NODE *curr_node;   /**< ��ǰ���ڴ���� json ��� */
	ACL_SLICE_POOL *slice;      /**< �ڴ�ض��� */
};

/*----------------------------- in acl_json.c -----------------------------*/

/**
 * ����һ�� json ���
 * @param json {ACL_JSON*} json ����
 * @return {ACL_JSON_NODE*} json ������
 */
ACL_API ACL_JSON_NODE *acl_json_node_alloc(ACL_JSON *json);

/**
 * ��ĳ�� json ��㼰���ӽ��� json ������ɾ��, ���ͷŸý�㼰���ӽ��
 * ��ռ�ռ亯�����ͷŸ� json �����ռ�ڴ�
 * @param node {ACL_JSON_NODE*} json ���
 * @return {int} ����ɾ���Ľ�����
 */
ACL_API int acl_json_node_delete(ACL_JSON_NODE *node);

/**
 * ��ĳ�� json �������ֵܽ��(���ֵܽ������Ƕ����� json ���)
 * @param node1 {ACL_JSON_NODE*} �򱾽����� json ���
 * @param node2 {ACL_JSON_NODE*} ����ӵ��ֵ� json ���
 */
ACL_API void acl_json_node_append(ACL_JSON_NODE *node1, ACL_JSON_NODE *node2);

/**
 * ��ĳ�� json �����Ϊ�ӽ�����ĳ�� json �����
 * @param parent {ACL_JSON_NODE*} �����
 * @param child {ACL_JSON_NODE*} �ӽ��
 */
ACL_API void acl_json_node_add_child(ACL_JSON_NODE *parent, ACL_JSON_NODE *child);

/**
 * ���ĳ�� json ���ĸ����
 * @param node {ACL_JSON_NODE*} json ���
 * @return {ACL_JSON_NODE*} �����, ���Ϊ NULL ���ʾ�丸��㲻����
 */
ACL_API ACL_JSON_NODE *acl_json_node_parent(ACL_JSON_NODE *node);

/**
 * ���ĳ�� json ���ĺ�һ���ֵܽ��
 * @param node {ACL_JSON_NODE*} json ���
 * @return {ACL_JSON_NODE*} ���� json ���ĺ�һ���ֵܽ��, ��ΪNULL���ʾ������
 */
ACL_API ACL_JSON_NODE *acl_json_node_next(ACL_JSON_NODE *node);

/**
 * ���ĳ�� json ����ǰһ���ֵܽ��
 * @param node {ACL_JSON_NODE*} json ���
 * @return {ACL_JSON_NODE*} ���� json ����ǰһ���ֵܽ��, ��ΪNULL���ʾ������
 */
ACL_API ACL_JSON_NODE *acl_json_node_prev(ACL_JSON_NODE *node);

/**
 * �����ǰ json ���������ַ���
 * @param node {ACL_JSON_NODE*} json ������
 * @return {const char*} json ���������͵������ַ���
 */
ACL_API const char *acl_json_node_type(const ACL_JSON_NODE *node);

/**
 * ����һ�� json ����
 * @return {ACL_JSON*} �´����� json ����
 */
ACL_API ACL_JSON *acl_json_alloc(void);

/**
 * ����һ�� json ���󣬸� json �������е��ڲ��ڴ���䶼�ڸ��ڴ���Ͻ��з���
 * @param slice {ACL_SLICE_POOL*} �ڴ�ض��󣬿���Ϊ��ָ�룬���������ڴ��
 * @return {ACL_JSON*} �´����� json ����
 */
ACL_API ACL_JSON *acl_json_alloc1(ACL_SLICE_POOL *slice);

/**
 * ��ĳһ�� ACL_JSON_NODE �����Ϊһ�� json ����ĸ���㣬
 * �Ӷ����Է���ر������ý��ĸ����ӽ��(�ڱ��������е�����
 * ��㲻�����������)���ñ�����ʽ�б��ڵ���
 * ����ĳһ�� ACL_JSON_NODE ���ʱ���ܱ�����һ���ӽ�������
 * @param json {ACL_JSON*} json ����
 * @param node {ACL_JSON_NODE*} ACL_JSON_NODE ���
 */
ACL_API void acl_json_foreach_init(ACL_JSON *json, ACL_JSON_NODE *node);

/**
 * �򿪻�ر� json �Ļ��湦�ܣ������� ACL_JSON ����ʱ��
 * json �Ľ�㻺�湦���������Ч��
 * @param json {ACL_JSON*} json ����
 * @param max_cache {int} ��������ֵ������ֵ > 0 ʱ��� json ������
 *  �� json ���Ļ��湦�ܣ������ر� json �������� json ���Ļ��湦��
 */
ACL_API void acl_json_cache(ACL_JSON *json, int max_cache);

/**
 * �ͷ� JSON �л���� JSON ������
 * @param json {ACL_JSON*} json ����
 */
ACL_API void acl_json_cache_free(ACL_JSON *json);

/**
 * �ͷ�һ�� json ����, ͬʱ�ͷŸö��������ɵ����� json ���
 * @param json {ACL_JSON*} json ����
 */
ACL_API int acl_json_free(ACL_JSON *json);

/**
 * ���� json ����������
 * @param json {ACL_JSON*} json ����
 */
ACL_API void acl_json_reset(ACL_JSON *json);

/*------------------------- in acl_json_parse.c ---------------------------*/

/**
 * ���� json ����, ���������Զ����� json �����
 * @param json {ACL_JSON*} json ����
 * @param data {const char*} �� '\0' ��β�������ַ���, ������������ json ����;
 *  Ҳ�����ǲ������� json ����, ����ѭ�����ô˺���, �����������ݳ���������
 */
ACL_API void acl_json_update(ACL_JSON *json, const char *data);

/*------------------------- in acl_json_util.c ----------------------------*/

/**
 * �ͷ��� acl_json_getElementsByTagName, acl_json_getElementsByName,
 * �Ⱥ������صĶ�̬�������, ��Ϊ�ö�̬�����е�
 * Ԫ�ض��� ACL_JSON ������Ԫ�ص�����, �����ͷŵ��ö�̬�����, ֻҪ ACL_JSON
 * �����ͷ�, ��ԭ�����ڸ������е�Ԫ����Ȼ����ʹ��.
 * �������ͷ������ xml ���Ԫ��
 * @param a {ACL_ARRAY*} ��̬�������
 */
ACL_API void acl_json_free_array(ACL_ARRAY *a);

/**
 * �� json �����л�����е���������ǩ����ͬ�� json ���ļ���
 * @param json {ACL_JSON*} json ����
 * @param tag {const char*} ��ǩ����
 * @return {ACL_ARRAY*} ���������� json ��㼯��, ���� ��̬������, ������ NULL ��
 *  ��ʾû�з��������� json ���, �ǿ�ֵ��Ҫ���� acl_json_free_array �ͷ�
 */
ACL_API ACL_ARRAY *acl_json_getElementsByTagName(ACL_JSON *json, const char *tag);

/**
 * �� json �����л�����е�������༶��ǩ����ͬ�� json ���ļ���
 * @param json {ACL_JSON*} json ����
 * @param tags {const char*} �༶��ǩ������ '/' �ָ�������ǩ��������� json ���ݣ�
 *  { 'root': [
 *      'first': { 'second': { 'third': 'test1' } },
 *      'first': { 'second': { 'third': 'test2' } },
 *      'first': { 'second': { 'third': 'test3' } }
 *    ]
 *  }
 *  ����ͨ���༶��ǩ����root/first/second/third һ���Բ�����з��������Ľ��
 * @return {ACL_ARRAY*} ���������� json ��㼯��, ���� ��̬������, ������ NULL ��
 *  ��ʾû�з��������� json ���, �ǿ�ֵ��Ҫ���� acl_json_free_array �ͷ�
 */
ACL_API ACL_ARRAY *acl_json_getElementsByTags(ACL_JSON *json, const char *tags);

/**
 * ���� json ����ʱ���� json Ҷ���
 * @param json {ACL_JSON*} �� acl_json_alloc / acl_json_alloc1 ����
 * @param name {const char*} ��ǩ�����ǿ�
 * @param text {const char*} ��ǩֵ���ǿ�
 * @return {ACL_JSON_NODE*} �´����Ľ��������ͷ� ACL_JSON ����ʱ
 *  һ���ͷţ����Բ���Ҫ�����ͷ�
 */
ACL_API ACL_JSON_NODE *acl_json_create_leaf(ACL_JSON *json,
	const char *name, const char *text);

/**
 * ���� json ����ʱ���� json ����(�������� {} �Ķ���)
 * @param json {ACL_JSON*} �� acl_json_alloc / acl_json_alloc1 ����
 * @return {ACL_JSON_NODE*} �´����Ľ��������ͷ� ACL_JSON ����ʱ
 *  һ���ͷţ����Բ���Ҫ�����ͷ�
 */
ACL_API ACL_JSON_NODE *acl_json_create_obj(ACL_JSON *json);

/**
 * ���� json ����ʱ���� json �������(�������� [] �Ķ���)
 * @param json {ACL_JSON*} �� acl_json_alloc / acl_json_alloc1 ����
 * @return {ACL_JSON_NODE*} �´����Ľ��������ͷ� ACL_JSON ����ʱ
 *  һ���ͷţ����Բ���Ҫ�����ͷ�
 */
ACL_API ACL_JSON_NODE *acl_json_create_array(ACL_JSON *json);

/**
 * ���� json ����ʱ���� json ������(�� tagname: ACL_JSON_NODE)
 * @param json {ACL_JSON*} �� acl_json_alloc / acl_json_alloc1 ����
 * @param name {const char*} json ���ı�ǩ��
 * @param value {ACL_JSON_NODE*} json ��������Ϊ��ǩֵ
 * @return {ACL_JSON_NODE*} �´����Ľ��������ͷ� ACL_JSON ����ʱ
 *  һ���ͷţ����Բ���Ҫ�����ͷ�
 */
ACL_API ACL_JSON_NODE *acl_json_create_node(ACL_JSON *json,
	const char *name, ACL_JSON_NODE *value);

/**
 * ���� json ����ʱ����һ���� acl_json_create_obj �� acl_json_create_array
 * ������ json �������ӽ�㣬���ӽ������������½ӿڴ����Ľ��:
 * acl_json_create_leaf, acl_json_create_obj, acl_json_create_array
 */
ACL_API void acl_json_node_append_child(ACL_JSON_NODE *parent,
	ACL_JSON_NODE *child);

/**
 * �� json ����ת���ַ�������
 * @param json {ACL_JSON*} json ����
 * @param buf {ACL_VSTRING*} �洢������Ļ����������ò���Ϊ��ʱ�����ڲ���
 *  �Զ�����һ�λ�������Ӧ���������Ҫ�ͷŵ����ǿպ����ڲ���ֱ�ӽ�����洢����
 * @return {ACL_VSTRING*} json ����ת�����ַ�����Ĵ洢���������÷���ֵ��Զ�ǿգ�
 *  ʹ���߿���ͨ�� ACL_VSTRING_LEN(x) �����ж������Ƿ�Ϊ�գ����ص� ACL_VSTRING
 *  ָ�����Ϊ�ú����ڲ������ģ����û��������� acl_vstring_free �����ͷ�
 */
ACL_API ACL_VSTRING *acl_json_build(ACL_JSON *json, ACL_VSTRING *buf);

#ifdef __cplusplus
}
#endif

#endif
