/**
 * @file    gbandutf8_core.h
 * @author  Yang
 * @date    2004-06-18
 * @warning For MSWINDOWS and UNIX; MT_Level: MT-Safe.
 * @brief   在本文件中定义了一系列 GB2312 和 utf-8 相互转换的函数.
 */
/*-------------------------------------------------------------------------*/
#ifndef _GBANDUTF8_CORE_H
#define _GBANDUTF8_CORE_H
/*-------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C"{
#endif
/*-------------------------------------------------------------------------*/
/**
 * utf-8 转换为 GB2312.
 * @param InBuf 要转换的缓冲区
 * @param InBufLen 要转换的缓冲区长度,以 字节(1 byte = 8 位) 为单位
 * @param OutBuf 输出转换后的结果
 * @param OutBufLen 输出缓冲区的长度,以 字节(1 byte = 8 位) 为单位
 * @return 成功返回输出缓冲区的长度,以 字节(1 byte = 8 位) 为单位,失败返回负数
 * <P>InBuf 不能为 NULL,InBufLen 不能为 0,否则会返回错误<BR>
 * OutBuf 不能为 NULL,OutBufLen 不能为 0,否则会返回错误<BR>
 * InBuf 和 OutBuf 指向的缓冲区不能重叠,否则会返回错误</P>
 */
int LC_Utf8ToGB2312(const void * InBuf, const unsigned int InBufLen,
		void * OutBuf, const unsigned int OutBufLen);

/**
 * GB2312 转换为 utf-8.
 * @param InBuf 要转换的缓冲区 
 * @param InBufLen 要转换的缓冲区长度,以 字节(1 byte = 8 位) 为单位
 * @param OutBuf 输出转换后的结果 
 * @param OutBufLen 输出缓冲区的长度,以 字节(1 byte = 8 位) 为单位
 * @return 成功返回输出缓冲区的长度,以 字节(1 byte = 8 位) 为单位,失败返回负数
 * <P>InBuf 不能为 NULL,InBufLen 不能为 0,否则会返回错误<BR>
 * OutBuf 不能为 NULL,OutBufLen 不能为 0,否则会返回错误<BR>
 * InBuf 和 OutBuf 指向的缓冲区不能重叠,否则会返回错误</P>
 */
int LC_GB2312ToUtf8(const void * InBuf, const unsigned int InBufLen,
		void * OutBuf, const unsigned int OutBufLen);
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif
/*-------------------------------------------------------------------------*/
#endif
/*-------------------------------------------------------------------------*/
