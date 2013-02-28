#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/mime/mime_head.hpp"

namespace acl
{

class mime_builder
{
public:
	mime_builder();
	~mime_builder();

	/**
	 * ����ʼ�ͷ
	 * @return {mime_head&}
	 */
	mime_head& primary_header(void)
	{
		return (primary_header_);
	}

	/**
	 * �����ʼ��������ݣ�Ϊ���ı���ʽ
	 * @param src {const char*) ��������
	 * @param len {size_t} src ���ݳ���
	 * @return {mime_builder&}
	 */
	mime_builder& set_body_text(const char* src, size_t len);

	/**
	 * �����ʼ��������ݣ�Ϊ HTML ��ʽ
	 * @param src {const char*) ��������
	 * @param len {size_t} src ���ݳ���
	 * @return {mime_builder&}
	 */
	mime_builder& set_body_html(const char* src, size_t len);

	/**
	 * ��Ӹ���
	 * @param filepath {const char*} �����ļ�·��
	 * @return {mime_builder&}
	 */
	mime_builder& add_attach(const char* filepath);

private:
	mime_head primary_header_;
	std::vector<char*> attachs_;
};

} // namespace acl
