#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/stream/istream.hpp"
#include "acl_cpp/stream/ostream.hpp"

namespace acl {

	class string;

	class ACL_CPP_API fstream
		: public istream
		, public ostream
	{
	public:
		fstream(void);
		virtual ~fstream(void);

		/**
		 * �����ļ�·�����ļ���, ����������Ĵ��ļ��ķ�ʽ
		 * @param path {const char*} �ļ���
		 * @param oflags {unsigned int} ��־λ, We're assuming that O_RDONLY: 0x0000,
		 *  O_WRONLY: 0x0001, O_RDWR: 0x0002, O_APPEND: 0x0008, O_CREAT: 0x0100,
		 *  O_TRUNC: 0x0200, O_EXCL: 0x0400; just for win32, O_TEXT: 0x4000,
		 *  O_BINARY: 0x8000, O_RAW: O_BINARY, O_SEQUENTIAL: 0x0020, O_RANDOM: 0x0010.
		 * @param mode {int} ���ļ����ʱ��ģʽ(��: 0600)
		 * @return {bool} ���ļ��Ƿ�ɹ�
		 */
		bool open(const char* path, unsigned int oflags, int mode);

		/**
		 * �Զ�/д��ʽ���ļ��������ļ�������ʱ�򴴽����ļ������ļ�����ʱ��
		 * ���ļ����, �ļ�����Ϊ 0700
		 * @param path {const char*} �ļ���
		 * @return {bool} ���ļ��Ƿ�ɹ�
		 */
		bool open_trunc(const char* path);

		/**
		 * �Զ�/д��ʽ�����ļ����ļ�����Ϊ 0700, ���ļ��������򴴽����ļ�����������
		 * �򿪾��ļ�
		 * @return {bool} �ļ������Ƿ�ɹ�
		 */
		bool create(const char* path);

		/**
		 * �ر��ļ�����ͬʱ�ر��ļ����
		 * @return {bool} �ر��Ƿ�ɹ�
		 */
		bool close();

#ifdef WIN32
		void open(void* fh, unsigned int oflags);
		__int64 fseek(__int64 offset, int whence);
		bool ftruncate(__int64 length);
		__int64 fsize(void) const;
		void* file_handle() const;
#else
		void open(int fh, unsigned int oflags);
		long long int fseek(long long int offset, int whence);
		bool ftruncate(long long int length);
		long long int fsize(void) const;
		int file_handle() const;
#endif
		/**
		 * ����ļ���ȫ·��
		 * @return {const char*} �����ؿ����ʾ�ļ���δ�򿪻����
		 */
		const char* file_path() const;
	};

} // namespace acl
