#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/master/master_conf.hpp"

namespace acl
{

class ACL_CPP_API master_base
{
public:
	/**
	 * ���� bool ���͵�������
	 * @param table {master_bool_tbl*}
	 */
	void set_cfg_bool(master_bool_tbl* table);

	/**
	 * ���� int ���͵�������
	 * @param table {master_int_tbl*}
	 */
	void set_cfg_int(master_int_tbl* table);

	/**
	 * ���� int64 ���͵�������
	 * @param table {master_int64_tbl*}
	 */
	void set_cfg_int64(master_int64_tbl* table);

	/**
	 * ���� �ַ��� ���͵�������
	 * @param table {master_str_tbl*}
	 */
	void set_cfg_str(master_str_tbl* table);

	/**
	 * �ж��Ƿ����� acl_master ���Ƶ� daemon ģʽ
	 * @return {bool}
	 */
	bool daemon_mode(void) const;
protected:
	bool daemon_mode_;
	bool proc_inited_;

	master_base();
	virtual ~master_base();

	/**
	 * �������л��û����ǰ���õĻص������������ڴ˺�������һЩ
	 * �û����Ϊ root ��Ȩ�޲���
	 */
	virtual void proc_pre_jail() {}

	/**
	 * �������л��û���ݺ���õĻص��������˺���������ʱ������
	 * ��Ȩ��Ϊ��ͨ���޼���
	 */
	virtual void proc_on_init() {}

	/**
	 * �������˳�ǰ���õĻص�����
	 */
	virtual void proc_on_exit() {}

	// ���ö���
	master_conf conf_;
};

}  // namespace acl
