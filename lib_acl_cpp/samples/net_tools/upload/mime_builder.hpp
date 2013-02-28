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
	 * 获得邮件头
	 * @return {mime_head&}
	 */
	mime_head& primary_header(void)
	{
		return (primary_header_);
	}

	/**
	 * 设置邮件正文内容，为纯文本格式
	 * @param src {const char*) 正文内容
	 * @param len {size_t} src 内容长度
	 * @return {mime_builder&}
	 */
	mime_builder& set_body_text(const char* src, size_t len);

	/**
	 * 设置邮件正文内容，为 HTML 格式
	 * @param src {const char*) 正文内容
	 * @param len {size_t} src 内容长度
	 * @return {mime_builder&}
	 */
	mime_builder& set_body_html(const char* src, size_t len);

	/**
	 * 添加附件
	 * @param filepath {const char*} 附件文件路径
	 * @return {mime_builder&}
	 */
	mime_builder& add_attach(const char* filepath);

private:
	mime_head primary_header_;
	std::vector<char*> attachs_;
};

} // namespace acl
