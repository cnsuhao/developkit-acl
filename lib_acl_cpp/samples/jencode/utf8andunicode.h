/**
 * @file    utf8andunicode_core.h
 * @author  Yang
 * @date    2003-04-15
 * @warning For MSWINDOWS and UNIX; MT_Level: MT-Safe.
 * @brief   在本文件中定义了一系列和 utf8 和 unicode 相互转换的函数,此处是指 UNICODELITTLE.
 */
/*-------------------------------------------------------------------------*/
#ifndef _UTF8ANDUNICODE_CORE_H
#define _UTF8ANDUNICODE_CORE_H
/*-------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C"{
#endif
/*-------------------------------------------------------------------------*/
/**
 * 将 utf-8 转换为 ucs-2.
 * @param InBuf 待转换的输入缓冲区
 * @param InBufLen 输入缓冲区中要转换的字节数(即: byte 数,1 byte = 8 位)
 * @param OutBuf 输出的缓冲区
 * @param OutBufLen 输出缓冲区的大小,以 字节(1 byte = 8 位)为单位
 * @return 成功返回 OutBuf 中以 ucs-2 为单位的字符个数,失败返回负数
 * <P>InBuf 不能为 NULL,InBufLen 不能为 0,否则会返回错误<BR>
 * OutBuf 不能为 NULL,OutBufLen 不能为 0,否则会返回错误<BR>
 * InBuf 和 OutBuf 指向的缓冲区不能重叠,否则会返回错误</P>
 */
int LC_Utf8ToUCS2(const void * InBuf, const unsigned int InBufLen,
		void * OutBuf, const unsigned int OutBufLen);

/**
 * 将 ucs-2 转换为 utf-8.
 * @param InBuf 待转换的输入缓冲区
 * @param InBufLen 输入缓冲区中要转换的字节数(即: byte 数,1 byte = 8 位)
 * @param OutBuf 输出的缓冲区
 * @param OutBufLen 输出缓冲区的大小,以字节(1 byte = 8 位)为单位
 * @return 成功返回 OutBuf 中以 字节(1 byte = 8 位) 为单位的字符个数,失败返回负数
 * <P>InBuf 不能为 NULL,InBufLen 不能为 0,否则会返回错误<BR>
 * OutBuf 不能为 NULL,OutBufLen 不能为 0,否则会返回错误<BR>
 * InBuf 和 OutBuf 指向的缓冲区不能重叠,否则会返回错误</P>
 */
int LC_UCS2ToUtf8(const void * InBuf, const unsigned int InBufLen,
		void * OutBuf, const unsigned int OutBufLen);

/**
 * 将 utf-8 转换为 unicode.
 * @param InBuf 待转换的输入缓冲区
 * @param InBufLen 输入缓冲区中要转换的字节数(即: byte 数,1 byte = 8 位)
 * @param OutBuf 输出的缓冲区
 * @param OutBufLen 输出缓冲区的大小,以 字节(1 byte = 8 位)为单位
 * @return 成功返回 OutBuf 中以 unicode(ucs-2) 为单位的字符个数,失败返回负数
 * <P>InBuf 不能为 NULL,InBufLen 不能为 0,否则会返回错误<BR>
 * OutBuf 不能为 NULL,OutBufLen 不能为 0,否则会返回错误<BR>
 * InBuf 和 OutBuf 指向的缓冲区不能重叠,否则会返回错误<BR>
 * 本函数所指的 unicode 为 ucs-2</P>
 */
int LC_Utf8ToUnicode(const void * InBuf, const unsigned int InBufLen,
		void * OutBuf, const unsigned int OutBufLen);

/**
 * 将 unicode 转换为 utf-8.
 * @param InBuf 待转换的输入缓冲区
 * @param InBufLen 输入缓冲区中要转换的字节数(即: byte 数,1 byte = 8 位)
 * @param OutBuf 输出的缓冲区
 * @param OutBufLen 输出缓冲区的大小,以 字节(1 byte = 8 位)为单位
 * @return 成功返回 OutBuf 中以 字节(1 byte = 8 位) 为单位的字符个数,失败返回负数
 * <P>InBuf 不能为 NULL,InBufLen 不能为 0,否则会返回错误<BR>
 * OutBuf 不能为 NULL,OutBufLen 不能为 0,否则会返回错误<BR>
 * InBuf 和 OutBuf 指向的缓冲区不能重叠,否则会返回错误<BR>
 * 本函数所指的 unicode 为 ucs-2</P>
 */
int LC_UnicodeToUtf8(const void * InBuf, const unsigned int InBufLen,
		void * OutBuf, const unsigned int OutBufLen);
/*-------------------------------------------------------------------------*/

/**
 * 内部函数,不建议直接使用.
 * 将 ucs-2 转换为 单字节字符串.
 * @param WideChar 待转换的输入字符串
 * @param WideCharLen 输入字符串中以 unicode(ucs-2) 为单位的字符个数
 * @param CharBuf 输出的缓冲区
 * @param CharBufLen 输出缓冲区的大小,以 字节(1 byte = 8 位)为单位
 * @param CodeSet 输出内容使用的字符集.
 * 当 CodeSet == NULL 时使用 GB2312,即最后的单字节字符为 GB2312
 * @return 成功返回 CharBuf 中以 字节(1 byte = 8 位) 为单位的字符个数,失败返回负数
 * <P>WideChar 不能为 NULL,WideCharLen 不能为 0,否则会返回错误<BR>
 * CharBuf 不能为 NULL,CharBufLen 不能为 0,否则会返回错误<BR>
 * WideChar 和 CharBuf 指向的缓冲区不能重叠,否则会返回错误</P>
 */
int LC_CharFromWideChar2(const void * WideChar, const unsigned int WideCharLen,
		void * CharBuf, const unsigned int CharBufLen,
		const char * CodeSet);

/**
 * 内部函数,不建议直接使用.
 * 将 单字节字符串 转换为 ucs-2.
 * @param CharBuf 待转换的输入字符串
 * @param CharBufLen 输入字符串中以 字节(1 byte = 8 位) 为单位的字符个数
 * @param WideChar 输出的缓冲区
 * @param WideCharBufLen 输出缓冲区的大小,以 unicode(ucs-2) 为单位
 * @param CodeSet 输入内容使用的字符集.
 * 当 CodeSet == NULL 时使用 GB2312,即输入的单字节字符为 GB2312 
 * @return 成功返回 CharBuf 中以 unicode(ucs-2) 为单位的字符个数,失败返回负数
 * <P>CharBuf 不能为 NULL,CharBufLen 不能为 0,否则会返回错误<BR>
 * WideChar 不能为 NULL,WideCharBufLen 不能为 0,否则会返回错误<BR>
 * CharBuf 和 WideChar 指向的缓冲区不能重叠,否则会返回错误</P>
 */
int LC_WideChar2FromChar(const void * CharBuf, const unsigned int CharBufLen,
		void * WideChar, const unsigned int WideCharBufLen,
		const char * CodeSet);
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif
/*-------------------------------------------------------------------------*/
#endif
/*-------------------------------------------------------------------------*/
