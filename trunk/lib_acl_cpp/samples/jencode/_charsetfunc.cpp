/*-------------------------------------------------------------------------*/
#include "StdAfx.h"
#include "_charsetfunc.h"
/*-------------------------------------------------------------------------*/
#ifdef LC_UNIX
int _LC_CharacterSizeWideChar2(const void * P, const unsigned int MaxLen)
{
	return sizeof(LC_WIDECHAR2);
}
/*-------------------------------------------------------------------------*/
int _LC_CharacterSizeLocaleChar(const void * P, const unsigned int MaxLen)
{
	int Ret = 0;

	if(P == NULL)
	{
		return -1;
	}

	Ret = mblen((const char * )P, MaxLen);
	if(Ret <= 0)
	{
		/*reset the shift state*/
		mblen(NULL, 0);
		Ret = sizeof(LC_U8);
	}
	return Ret;
}
/*-------------------------------------------------------------------------*/
int _LC_BufConvert(iconv_t Context,
		const void * InBuf, const unsigned int InBufLen,
		void * OutBuf, const unsigned int OutBufLen,
		const unsigned int OutCharSize,
		int( * Func)(const void * , const unsigned int ))
{
	const char c_err_msg1[] = "_charsetfunc_core._LC_BufConvert iconv: "
		"ErrorCode=%d; ErrorMessage=%s";
	const char c_err_msg2[] = "_charsetfunc_core._LC_BufConvert iconv: "
		"ErrorNumber=E2BIG; ErrorCode=%d; ErrorMessage=%s";
	const char c_unknown_chr = '?';

	char * In = (char * )InBuf, * Out = (char * )OutBuf, * PNil = NULL;
	unsigned int Ret = 0;
	int CS = 0;
	size_t InLen = InBufLen, OutLen = OutBufLen, Zero = 0;

	if((InBuf == NULL) || (OutBuf == NULL))
	{
		return -1;
	}
	else if((InBufLen == 0) || (OutBufLen == 0))
	{
		return -1;
	}

	memset(OutBuf, 0, OutBufLen);
	while(1)
	{
		/*success*/
#ifdef LC_LINUX
		if((Ret = iconv(Context,
						&In,
						&InLen,
						&Out,
						&OutLen)) != (size_t)(-1))
#endif
#ifdef LC_SUNOS
		if((Ret = iconv(Context,
						(const char **)&In,
						&InLen,
						&Out,
						&OutLen)) != (size_t)(-1))
#endif
#ifdef LC_HPUX
		if((Ret = iconv(Context,
						&In,
						&InLen,
						&Out,
						&OutLen)) != (size_t)(-1))
#endif
		{
			break;
		}
		else{
			if((errno == E2BIG) && (InLen > 0) && (OutLen > 0))
			{
				continue;
			}
			else if((errno == EILSEQ) || (errno == EINVAL))
			{
				PNil = NULL;
				Zero = 0;
				/*Reset state of context*/
#ifdef LC_LINUX
				iconv(Context,
						&PNil,
						&Zero,
						&PNil,
						&Zero);
#endif
#ifdef LC_SUNOS
				iconv(Context,
						(const char **)&PNil,
						&Zero,
						&PNil,
						&Zero);
#endif
#ifdef LC_HPUX
				iconv(Context,
						&PNil,
						&Zero,
						&PNil,
						&Zero);
#endif
				/* Invalid input character in conversion stream.
				 * Skip input character and write '?' to output stream.
				 * The glibc iconv() implementation also returns EILSEQ
				 * for a valid input character that cannot be converted
				 * into the requested codeset.
				 */
				CS = (*Func)(In, InLen);
				In += CS;
				InLen -= CS;
				if((OutCharSize != 1) && (OutCharSize != 2))
				{
					return -1;
				}
				if(OutCharSize == 1)
				{
					*Out = c_unknown_chr;
					Out++;
					OutLen--;
				}
				else if(OutCharSize == 2)
				{
#ifdef LC_BIG_ENDIAN_HOST
					*Out = c_unknown_chr;
					*(Out + 1) = '\0';
#else
					*((LC_WIDECHAR2 * )Out) = 
						(LC_WIDECHAR2)c_unknown_chr;
#endif
					Out += 2;
					OutLen -= 2;
				}
				continue;
			}
			else{
				if(errno == E2BIG)
				{
					_LC_HandleErr(LC_LOG_OS_ERR, c_err_msg2);
					return -1;
				}
				else{
					_LC_HandleErr(LC_LOG_OS_ERR, c_err_msg1);
					return -1;
				}
			}
		}
	}
	return (OutBufLen - OutLen);
}
#endif
/*-------------------------------------------------------------------------*/
