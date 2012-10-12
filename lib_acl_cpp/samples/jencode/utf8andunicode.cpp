/*-------------------------------------------------------------------------*/
#include "StdAfx.h"
#include "_charsetfunc.h"
#include "utf8andunicode.h"
/*-------------------------------------------------------------------------*/

int LC_Utf8ToUCS2(const void * InBuf, const unsigned int InBufLen,
		void * OutBuf, const unsigned int OutBufLen)
{
	LC_U8 * PUtf = (LC_U8 * )InBuf;
	LC_WIDECHAR2 * PUnicode = (LC_WIDECHAR2 * )OutBuf;
	LC_U8 C = 0;
	unsigned int I = 0, Count = 0;
	LC_U32 WC = 0;
#ifdef LC_BIG_ENDIAN_HOST
	LC_U32 TmpWC = 0;
	LC_U16 Tmp = 0;
#endif

	if((InBuf == NULL) || (OutBuf == NULL))
	{
		return -1;
	}
	else if((InBufLen == 0) || (OutBufLen == 0))
	{
		return -1;
	}
	else if(InBuf == OutBuf)
	{
		return -1;
	}

	memset(OutBuf, 0, OutBufLen);
	while((I < InBufLen) && (Count < OutBufLen))
	{
		WC = PUtf[I];
		I++;
		if((WC & 0x80) != 0)
		{
			/*incomplete multibyte char*/
			if(I >= InBufLen)
			{
				return -1;
			}
			WC = WC & 0x3F;
			if((WC & 0x20) != 0)
			{
				C = PUtf[I];
				I++;
				/*malformed trail byte or out of range char*/
				if((C & 0xC0) != 0x80)
				{
					return -1;
				}
				/*incomplete multibyte char*/
				if(I >= InBufLen)
				{
					return -1;
				}
				WC = (WC << 6) | (C & 0x3F);
			}
			C = PUtf[I];
			I++;
			/*malformed trail byte*/
			if((C & 0xC0) != 0x80)
			{
				return -1;
			}
#ifdef LC_BIG_ENDIAN_HOST
			TmpWC = (WC << 6) | (C & 0x3F);
			((LC_U8 * )&Tmp)[0] = ((LC_U8 * )&TmpWC)[3];
			((LC_U8 * )&Tmp)[1] = ((LC_U8 * )&TmpWC)[2];
			PUnicode[Count] = Tmp;
#else
			PUnicode[Count] = (LC_WIDECHAR2)((WC << 6) | (C & 0x3F));
#endif
		}
		else{
#ifdef LC_BIG_ENDIAN_HOST
			Tmp = 0;
			((LC_U8 * )&Tmp)[0] = ((LC_U8 * )&WC)[3];
			((LC_U8 * )&Tmp)[1] = ((LC_U8 * )&WC)[2];
			PUnicode[Count] = Tmp;
#else
			PUnicode[Count] = (LC_WIDECHAR2)WC;
#endif
		}
		Count++;
	}
	if(Count >= OutBufLen)
	{
		Count = OutBufLen - 1;
	}
	PUnicode[Count] = 0;
	return Count;
}
/*-------------------------------------------------------------------------*/
int LC_UCS2ToUtf8(const void * InBuf, const unsigned int InBufLen,
		void * OutBuf, const unsigned int OutBufLen)
{
	LC_WIDECHAR2 * PUnicode = (LC_WIDECHAR2 * )InBuf;
	LC_U8 * PUtf = (LC_U8 * )OutBuf;
	LC_WIDECHAR2 C = 0;
	unsigned int I = 0, Count = 0;
#ifdef LC_BIG_ENDIAN_HOST
	LC_U32 TmpWC = 0;
	LC_U16 Tmp = 0;
#endif

	if((InBuf == NULL) || (OutBuf == NULL))
	{
		return -1;
	}
	else if((InBufLen == 0) || (OutBufLen == 0))
	{
		return -1;
	}
	else if(InBuf == OutBuf)
	{
		return -1;
	}

	memset(OutBuf, 0, OutBufLen);
	while((I < InBufLen) && (Count < OutBufLen))
	{
#ifdef LC_BIG_ENDIAN_HOST
		TmpWC = PUnicode[I];
		((LC_U8 * )&Tmp)[0] = ((LC_U8 * )&TmpWC)[3];
		((LC_U8 * )&Tmp)[1] = ((LC_U8 * )&TmpWC)[2];
		C = Tmp;
#else
		C = (PUnicode[I]);
#endif
		I++;
		if(C <= 0x7F)
		{
			PUtf[Count] = (LC_U8)C;
			Count++;
		}
		else if(C > 0x7FF)
		{
			if((Count + 3) > OutBufLen)
			{
				break;
			}
			PUtf[Count] = (LC_U8)(0xE0 | (C >> 12));
			PUtf[Count + 1] = (LC_U8)(0x80 | ((C >> 6) & 0x3F));
			PUtf[Count + 2] = (LC_U8)(0x80 | (C & 0x3F));
			Count += 3;
		}
		/*0x7F < Source[I] <= 0x7FF*/
		else{
			if((Count + 2) > OutBufLen)
			{
				break;
			}
			PUtf[Count] = (LC_U8)(0xC0 | (C >> 6));
			PUtf[Count + 1] = (LC_U8)(0x80 | (C & 0x3F));
			Count += 2;
		}
	}
	if(Count >= OutBufLen)
	{
		Count = OutBufLen - 1;
	}
	PUtf[Count] = 0;
	return Count;
}
/*-------------------------------------------------------------------------*/
int LC_Utf8ToUnicode(const void * InBuf, const unsigned int InBufLen,
		void * OutBuf, const unsigned int OutBufLen)
{
	if((InBuf == NULL) || (OutBuf == NULL))
	{
		return -1;
	}
	else if((InBufLen == 0) || (OutBufLen == 0))
	{
		return -1;
	}
	else if(InBuf == OutBuf)
	{
		return -1;
	}

	memset(OutBuf, 0, OutBufLen);
	return LC_Utf8ToUCS2(InBuf, InBufLen, OutBuf, OutBufLen);
}
/*-------------------------------------------------------------------------*/
int LC_UnicodeToUtf8(const void * InBuf, const unsigned int InBufLen,
		void * OutBuf, const unsigned int OutBufLen)
{
	if((InBuf == NULL) || (OutBuf == NULL))
	{
		return -1;
	}
	else if((InBufLen == 0) || (OutBufLen == 0))
	{
		return -1;
	}
	else if(InBuf == OutBuf)
	{
		return -1;
	}

	memset(OutBuf, 0, OutBufLen);
	return LC_UCS2ToUtf8(InBuf, InBufLen, OutBuf, OutBufLen);
}
/*-------------------------------------------------------------------------*/
int LC_CharFromWideChar2(const void * WideChar, const unsigned int WideCharLen,
		void * CharBuf, const unsigned int CharBufLen,
		const char * CodeSet)
{
#ifdef LC_MSWINDOWS
	const char c_err_msg[] = "utf8andunicode_core.LC_CharFromWideChar2 "
		"WideCharToMultiByte: ErrorCode=%d; ErrorMessage=%s";
	int Ret = 0;

	if((WideChar == NULL) || (CharBuf == NULL))
	{
		return -1;
	}
	else if((WideCharLen == 0) || (CharBufLen == 0))
	{
		return -1;
	}
	else if(WideChar == CharBuf)
	{
		return -1;
	}

	memset(CharBuf, 0, CharBufLen);
	if((Ret = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)WideChar, WideCharLen,
					(LPSTR) CharBuf, CharBufLen, NULL, NULL)) == 0)
	{
		return -1;
	}
	return Ret;
#endif
#ifdef LC_UNIX
#ifdef LC_LINUX
	const char c_err_msg1[] = "utf8andunicode_core.LC_CharFromWideChar2 iconv_open: "
		"ErrorCode=%d; ErrorMessage=%s";
	const char c_err_msg2[] = "utf8andunicode_core.LC_CharFromWideChar2 iconv_close: "
		"ErrorCode=%d; ErrorMessage=%s";

	char ToCodeSetBuf[LC_SHORTLINE_LEN], FromCodeSetBuf[LC_SHORTLINE_LEN];
	iconv_t Context;
	int Ret = 0;

	if((WideChar == NULL) || (CharBuf == NULL))
	{
		return -1;
	}
	else if((WideCharLen == 0) || (CharBufLen == 0))
	{
		return -1;
	}
	else if(WideChar == CharBuf)
	{
		return -1;
	}
	memset(CharBuf, 0, CharBufLen);
	memset(ToCodeSetBuf, 0, sizeof(ToCodeSetBuf));
	memset(FromCodeSetBuf, 0, sizeof(FromCodeSetBuf));

	if(CodeSet == NULL)
	{
		strcpy(ToCodeSetBuf, "GB18030");
	}
	else{
		strncpy(ToCodeSetBuf, CodeSet, sizeof(ToCodeSetBuf) - 1);
	}
	strcpy(FromCodeSetBuf, "UNICODELITTLE");
	if((Context = iconv_open(ToCodeSetBuf, FromCodeSetBuf)) == (iconv_t)(-1))
	{
		_LC_HandleErr(LC_LOG_OS_ERR, c_err_msg1);
		return -1;
	}
	if((Ret = _LC_BufConvert(Context,
					WideChar,
					WideCharLen * sizeof(LC_WIDECHAR2),
					CharBuf,
					CharBufLen,
					1,
					_LC_CharacterSizeWideChar2)) < 0)
	{
		iconv_close(Context);
		return -1;
	}
	if(iconv_close(Context) != 0)
	{
		return -1;
	}

	return Ret;
#endif
#ifdef LC_SUNOS
	const char c_err_msg1[] = "utf8andunicode_core.LC_CharFromWideChar2 iconv_open: "
		"ErrorCode=%d; ErrorMessage=%s";
	const char c_err_msg2[] = "utf8andunicode_core.LC_CharFromWideChar2 iconv_close: "
		"ErrorCode=%d; ErrorMessage=%s";

	LC_U8 * P = NULL; 
	char ToCodeSetBuf[LC_SHORTLINE_LEN], FromCodeSetBuf[LC_SHORTLINE_LEN];
	iconv_t Context;
	int Ret = 0;

	if((WideChar == NULL) || (CharBuf == NULL))
	{
		return -1;
	}
	else if((WideCharLen == 0) || (CharBufLen == 0))
	{
		return -1;
	}
	else if(WideChar == CharBuf)
	{
		return -1;
	}
	memset(CharBuf, 0, CharBufLen);
	memset(ToCodeSetBuf, 0, sizeof(ToCodeSetBuf));
	memset(FromCodeSetBuf, 0, sizeof(FromCodeSetBuf));

	if((P = (LC_U8 * )malloc(WideCharLen * sizeof(LC_WIDECHAR2) + LC_ETC_LEN)) 
			== NULL)
	{
		return -1;
	}
	if((Ret = LC_UCS2ToUtf8(WideChar,
					WideCharLen,
					P,
					WideCharLen * sizeof(LC_WIDECHAR2) + LC_ETC_LEN)) 
			< 0)
	{
		free(P);
		return -1;
	}

	if(CodeSet == NULL)
	{
		strcpy(ToCodeSetBuf, "GBK");
	}
	else{
		strncpy(ToCodeSetBuf, CodeSet, sizeof(ToCodeSetBuf) - 1);
	}
	strcpy(FromCodeSetBuf, "UTF-8");
	if((Context = iconv_open(ToCodeSetBuf, FromCodeSetBuf)) == (iconv_t)(-1))
	{
		free(P);
		_LC_HandleErr(LC_LOG_OS_ERR, c_err_msg1);
		return -1;
	}
	if((Ret = _LC_BufConvert(Context,
					P,
					Ret,
					CharBuf,
					CharBufLen,
					1,
					_LC_CharacterSizeWideChar2)) < 0)
	{
		free(P);
		iconv_close(Context);
		return -1;
	}
	if(iconv_close(Context) != 0)
	{
		free(P);
		return -1;
	}
	free(P);
	return Ret;
#endif
#ifdef LC_HPUX
	const char c_err_msg1[] = "utf8andunicode_core.LC_CharFromWideChar2 iconv_open: "
		"ErrorCode=%d; ErrorMessage=%s";
	const char c_err_msg2[] = "utf8andunicode_core.LC_CharFromWideChar2 iconv_close: "
		"ErrorCode=%d; ErrorMessage=%s";

	char ToCodeSetBuf[LC_SHORTLINE_LEN], FromCodeSetBuf[LC_SHORTLINE_LEN];
	iconv_t Context;
	int Ret = 0;

	if((WideChar == NULL) || (CharBuf == NULL))
	{
		return -1;
	}
	else if((WideCharLen == 0) || (CharBufLen == 0))
	{
		return -1;
	}
	else if(WideChar == CharBuf)
	{
		return -1;
	}
	memset(CharBuf, 0, CharBufLen);
	memset(ToCodeSetBuf, 0, sizeof(ToCodeSetBuf));
	memset(FromCodeSetBuf, 0, sizeof(FromCodeSetBuf));

	if(CodeSet == NULL)
	{
		strcpy(ToCodeSetBuf, "GB18030");
	}
	else{
		strncpy(ToCodeSetBuf, CodeSet, sizeof(ToCodeSetBuf) - 1);
	}
	strcpy(FromCodeSetBuf, "UNICODELITTLE");
	if((Context = iconv_open(ToCodeSetBuf, FromCodeSetBuf)) == (iconv_t)(-1))
	{
		return -1;
	}
	if((Ret = _LC_BufConvert(Context,
					WideChar,
					WideCharLen * sizeof(LC_WIDECHAR2),
					CharBuf,
					CharBufLen,
					1,
					_LC_CharacterSizeWideChar2)) < 0)
	{
		iconv_close(Context);
		return -1;
	}
	if(iconv_close(Context) != 0)
	{
		return -1;
	}

	return Ret;
#endif
#endif
}
/*-------------------------------------------------------------------------*/
int LC_WideChar2FromChar(const void * CharBuf, const unsigned int CharBufLen,
		void * WideChar, const unsigned int WideCharBufLen,
		const char * CodeSet)
{
#ifdef LC_MSWINDOWS
	const char c_err_msg[] = "utf8andunicode_core.LC_WideChar2FromChar "
		"MultiByteToWideChar: ErrorCode=%d; ErrorMessage=%s";
	int Ret = 0;

	if((WideChar == NULL) || (CharBuf == NULL))
	{
		return -1;
	}
	else if((WideCharBufLen == 0) || (CharBufLen == 0))
	{
		return -1;
	}
	else if(CharBuf == WideChar)
	{
		return -1;
	}

	memset(WideChar, 0, WideCharBufLen * sizeof(LC_WIDECHAR2));
	if((Ret = MultiByteToWideChar(CP_ACP,
					0,
					(LPCSTR) CharBuf,
					CharBufLen,
					(LPWSTR) WideChar,
					WideCharBufLen)) == 0)
	{
		return -1;
	}
	return Ret;
#endif
#ifdef LC_UNIX
#ifdef LC_LINUX
	const char c_err_msg1[] = "utf8andunicode_core.LC_WideChar2FromChar iconv_open: "
		"ErrorCode=%d; ErrorMessage=%s";
	const char c_err_msg2[] = "utf8andunicode_core.LC_WideChar2FromChar iconv_close: "
		"ErrorCode=%d; ErrorMessage=%s";

	char FromCodeSetBuf[LC_SHORTLINE_LEN], ToCodeSetBuf[LC_SHORTLINE_LEN];
	iconv_t Context;
	int Ret = 0;

	if((WideChar == NULL) || (CharBuf == NULL))
	{
		return -1;
	}
	else if((WideCharBufLen == 0) || (CharBufLen == 0))
	{
		return -1;
	}
	else if(CharBuf == WideChar)
	{
		return -1;
	}

	memset(WideChar, 0, WideCharBufLen * sizeof(LC_WIDECHAR2));
	memset(FromCodeSetBuf, 0, sizeof(FromCodeSetBuf));
	memset(ToCodeSetBuf, 0, sizeof(ToCodeSetBuf));

	if(CodeSet == NULL)
	{
		strcpy(FromCodeSetBuf, "GB18030");
	}
	else{
		strncpy(FromCodeSetBuf, CodeSet, sizeof(FromCodeSetBuf) - 1);
	}
	strcpy(ToCodeSetBuf, "UNICODELITTLE");
	if((Context = iconv_open(ToCodeSetBuf, FromCodeSetBuf)) == (iconv_t)(-1))
	{
		return -1;
	}
	if((Ret = _LC_BufConvert(Context,
					CharBuf,
					CharBufLen,
					WideChar,
					WideCharBufLen * sizeof(LC_WIDECHAR2),
					2,
					_LC_CharacterSizeLocaleChar)) < 0)
	{
		iconv_close(Context);
		return -1;
	}
	if(iconv_close(Context) != 0)
	{
		return -1;
	}
	return (Ret / sizeof(LC_WIDECHAR2));
#endif
#ifdef LC_SUNOS
	const char c_err_msg1[] = "utf8andunicode_core.LC_WideChar2FromChar iconv_open: "
		"ErrorCode=%d; ErrorMessage=%s";
	const char c_err_msg2[] = "utf8andunicode_core.LC_WideChar2FromChar iconv_close: "
		"ErrorCode=%d; ErrorMessage=%s";

	LC_U8 * P = NULL;    
	char FromCodeSetBuf[LC_SHORTLINE_LEN], ToCodeSetBuf[LC_SHORTLINE_LEN];
	iconv_t Context;
	int Ret = 0;

	if((WideChar == NULL) || (CharBuf == NULL))
	{
		return -1;
	}
	else if((WideCharBufLen == 0) || (CharBufLen == 0))
	{
		return -1;
	}
	else if(CharBuf == WideChar)
	{
		return -1;
	}

	memset(WideChar, 0, WideCharBufLen * sizeof(LC_WIDECHAR2));
	memset(FromCodeSetBuf, 0, sizeof(FromCodeSetBuf));
	memset(ToCodeSetBuf, 0, sizeof(ToCodeSetBuf));
	if((P = (LC_U8 * )LC_MemoryAlloc(WideCharBufLen * sizeof(LC_WIDECHAR2))) == NULL)
	{
		return -1;
	}
	if(CodeSet == NULL)
	{
		strcpy(FromCodeSetBuf, "GBK");
	}
	else{
		strncpy(FromCodeSetBuf, CodeSet, sizeof(FromCodeSetBuf) - 1);
	}
	strcpy(ToCodeSetBuf, "UTF-8");
	if((Context = iconv_open(ToCodeSetBuf, FromCodeSetBuf)) == (iconv_t)(-1))
	{
		free(P);
		return -1;
	}
	if((Ret = _LC_BufConvert(Context,
					CharBuf,
					CharBufLen,
					P,
					WideCharBufLen * sizeof(LC_WIDECHAR2),
					2,
					_LC_CharacterSizeLocaleChar)) < 0)
	{
		free(P);
		iconv_close(Context);
		return -1;
	}
	if(iconv_close(Context) != 0)
	{
		free(P);
		return -1;
	}

	if((Ret = LC_Utf8ToUCS2(P,
					Ret,
					WideChar,
					WideCharBufLen * sizeof(LC_WIDECHAR2))) < 0)
	{
		free(P);
		return -1;
	}
	free(P);
	return Ret;
#endif
#ifdef LC_HPUX
	const char c_err_msg1[] = "utf8andunicode_core.LC_WideChar2FromChar iconv_open: "
		"ErrorCode=%d; ErrorMessage=%s";
	const char c_err_msg2[] = "utf8andunicode_core.LC_WideChar2FromChar iconv_close: "
		"ErrorCode=%d; ErrorMessage=%s";

	char FromCodeSetBuf[LC_SHORTLINE_LEN], ToCodeSetBuf[LC_SHORTLINE_LEN];
	iconv_t Context;
	int Ret = 0;

	if((WideChar == NULL) || (CharBuf == NULL))
	{
		return -1;
	}
	else if((WideCharBufLen == 0) || (CharBufLen == 0))
	{
		return -1;
	}
	else if(CharBuf == WideChar)
	{
		return -1;
	}

	memset(WideChar, 0, WideCharBufLen * sizeof(LC_WIDECHAR2));
	memset(FromCodeSetBuf, 0, sizeof(FromCodeSetBuf));
	memset(ToCodeSetBuf, 0, sizeof(ToCodeSetBuf));

	if(CodeSet == NULL)
	{
		strcpy(FromCodeSetBuf, "GB18030");
	}
	else{
		strncpy(FromCodeSetBuf, CodeSet, sizeof(FromCodeSetBuf) - 1);
	}
	strcpy(ToCodeSetBuf, "UNICODELITTLE");
	if((Context = iconv_open(ToCodeSetBuf, FromCodeSetBuf)) == (iconv_t)(-1))
	{
		return -1;
	}
	if((Ret = _LC_BufConvert(Context,
					CharBuf,
					CharBufLen,
					WideChar,
					WideCharBufLen * sizeof(LC_WIDECHAR2),
					2,
					_LC_CharacterSizeLocaleChar)) < 0)
	{
		iconv_close(Context);
		return -1;
	}
	if(iconv_close(Context) != 0)
	{
		return -1;
	}
	return (Ret / sizeof(LC_WIDECHAR2));
#endif
#endif
}
/*-------------------------------------------------------------------------*/
