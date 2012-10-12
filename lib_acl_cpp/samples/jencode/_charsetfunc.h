/** 
 * @file    _charsetfunc_core.h
 * @author  Yang
 * @date    2003-04-15
 * @warning For MSWINDOWS and UNIX; MT_Level: MT-Safe.
 * @brief   
 */
/*-------------------------------------------------------------------------*/
#ifndef __CHARSETFUNC_CORE_H
#define __CHARSETFUNC_CORE_H

#define LC_MSWINDOWS

/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C"{
#endif
/*-------------------------------------------------------------------------*/
typedef unsigned int LC_U32;
typedef unsigned short LC_U16;
typedef unsigned char LC_U8;

/*有可能已定义,如果已定义,则不再定义*/
#ifndef HAVE_LC_WIDECHAR2_TYPEDEF
#undef LC_WIDECHAR2
#ifdef LC_MSWINDOWS
typedef wchar_t LC_WIDECHAR2;
#endif
#ifdef LC_UNIX
typedef LC_U16 LC_WIDECHAR2;
#endif
#define HAVE_LC_WIDECHAR2_TYPEDEF
#endif

#ifdef LC_ETC_LEN
#undef LC_ETC_LEN
#endif
#define LC_ETC_LEN 16
/*-------------------------------------------------------------------------*/
#ifdef LC_UNIX
/*
 * 内部函数,不建议直接使用.
 */
int _LC_CharacterSizeWideChar2(const void * P, const unsigned int MaxLen);

/*
 * 内部函数,不建议直接使用.
 */
int _LC_CharacterSizeLocaleChar(const void * P, const unsigned int MaxLen);

/*
 * 内部函数,不建议直接使用.
 */
int _LC_BufConvert(iconv_t Context,
		const void * InBuf, const unsigned int InBufLen,
		void * OutBuf, const unsigned int OutBufLen,
		const unsigned int OutCharSize,
		int( * Func)(const void * , const unsigned int ));
#endif
/*-------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif
/*-------------------------------------------------------------------------*/
#endif
/*-------------------------------------------------------------------------*/
